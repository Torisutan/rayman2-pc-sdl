/*
 ----------------------------------------------------------------------------------------
  ogl_draw.c : rendering of the OpenGL 3.3 driver.

  The Direct3D 6 driver draws pre-transformed vertices (GLI_tdstTLVertex: screen x, y, z,
  rhw, colour, specular with the fog factor in its alpha, u, v) as triangle and line lists,
  under a few render and texture-stage states. Its device-independent files (the clip
  pipeline, the render-state helpers, the opaque texture selection) reach the device only
  through GLI_DRV_M_SetRenderState, _SetTextureStageState, _SetTexture and _DrawPrimitive
  (GliDX6vf_State.h). This file is the GL branch of those four: it keeps the states as the
  device does, and maps them to GL state and to one shader program when a list is drawn.

  State mapping (the states the Direct3D 6 driver sets):
    ZENABLE / ZWRITEENABLE / ZFUNC          depth test, depth mask, depth function
    ALPHABLENDENABLE / SRCBLEND / DESTBLEND blending
    ALPHATESTENABLE / ALPHAFUNC / ALPHAREF  discard in the fragment shader
    COLORKEYENABLE                          discard of key texels (alpha 0, see ogl_tex.c)
    FOGENABLE / FOGCOLOR                    vertex fog: specular alpha mixes the fog colour
    COLOROP / ALPHAOP (stage 0)             SELECTARG1 (texture), MODULATE (texture * diffuse)
    ADDRESSU / ADDRESSV                     wrap modes of the bound texture
    CULLMODE (none), DITHERENABLE, SPECULARENABLE (off), filters (linear): fixed
 ----------------------------------------------------------------------------------------
 */
#include "GliGL3vf/GliGL3vf.h"
#include "GliDX6vf/GLI_DX6Constants.h"

#include <SDL3/SDL.h>
#include <string.h>

GLI_tdstGLFunctions GLI_g_stGL;

#define M_GL(Name)  (GLI_g_stGL.p_fn_##Name)

/* Render states kept as the device keeps them, by state number */
#define C_lNbRenderStates   0x100
#define C_lNbStageStates    0x20

#define C_lAttribPosition   0
#define C_lAttribColor      1
#define C_lAttribSpecular   2
#define C_lAttribUV         3

static DWORD gs_a_ulRenderState[C_lNbRenderStates];
static DWORD gs_a_ulStageState[C_lNbStageStates];
static GLI_tdstGLTexture *gs_p_stTexture = NULL;

static GLuint gs_uiProgram = 0;
static GLuint gs_uiVertexArray = 0;
static GLuint gs_uiVertexBuffer = 0;
static GLuint gs_uiBitmapTexture = 0;

/* Uniform locations */
static GLint gs_iUScreenSize;
static GLint gs_iUTexture;
static GLint gs_iUHasTexture;
static GLint gs_iUAlphaFormat;
static GLint gs_iUColorOp;
static GLint gs_iUAlphaOp;
static GLint gs_iUColorKey;
static GLint gs_iUAlphaTest;
static GLint gs_iUAlphaRef;
static GLint gs_iUFog;
static GLint gs_iUFogColor;

/* Size of the screen the engine draws in (the client rect), the coordinate space of the
   vertices; the drawable may be larger (high-DPI): the viewport scales */
static float gs_xScreenWidth = 640.0f;
static float gs_xScreenHeight = 480.0f;

/* Vertex shader: screen coordinates back to clip space. Direct3D 6 puts pixel centres on
   integer coordinates, GL half a pixel further; w = 1 / rhw keeps texturing and fog
   perspective-correct; the depth keeps Direct3D's [0, 1] meaning (cleared to 0, GEQUAL). */
static const char gs_szVertexShader[] =
    "#version 330 core\n"
    "in vec4 a_Position;\n"
    "in vec4 a_Color;\n"
    "in vec4 a_Specular;\n"
    "in vec2 a_UV;\n"
    "uniform vec2 u_ScreenSize;\n"
    "out vec4 v_Color;\n"
    "out vec2 v_UV;\n"
    "out float v_Fog;\n"
    "void main()\n"
    "{\n"
    "    float w = 1.0 / a_Position.w;\n"
    "    vec2 ndc = vec2((a_Position.x + 0.5) / u_ScreenSize.x * 2.0 - 1.0,\n"
    "                    1.0 - (a_Position.y + 0.5) / u_ScreenSize.y * 2.0);\n"
    "    gl_Position = vec4(ndc * w, (a_Position.z * 2.0 - 1.0) * w, w);\n"
    "    v_Color = a_Color.zyxw;\n"
    "    v_UV = a_UV;\n"
    "    v_Fog = a_Specular.w;\n"
    "}\n";

/* Fragment shader: texture stage 0, alpha test, colour key and fog of the Direct3D 6
   device. Without a texture the texture argument reads white, as on the device. */
static const char gs_szFragmentShader[] =
    "#version 330 core\n"
    "in vec4 v_Color;\n"
    "in vec2 v_UV;\n"
    "in float v_Fog;\n"
    "uniform sampler2D u_Texture;\n"
    "uniform int u_HasTexture;\n"
    "uniform int u_AlphaFormat;\n"
    "uniform int u_ColorOp;\n"
    "uniform int u_AlphaOp;\n"
    "uniform int u_ColorKey;\n"
    "uniform int u_AlphaTest;\n"
    "uniform float u_AlphaRef;\n"
    "uniform int u_Fog;\n"
    "uniform vec4 u_FogColor;\n"
    "out vec4 o_Color;\n"
    "void main()\n"
    "{\n"
    "    vec4 t = vec4(1.0);\n"
    "    vec4 c;\n"
    "    if (u_HasTexture != 0) {\n"
    "        t = texture(u_Texture, v_UV);\n"
    "        if (u_ColorKey != 0 && t.a < 0.5 / 255.0) discard;\n"
    "        if (u_AlphaFormat == 0) t.a = 1.0;\n"
    "    }\n"
    "    if (u_ColorOp == 2) c.rgb = t.rgb;\n"
    "    else if (u_ColorOp == 4) c.rgb = t.rgb * v_Color.rgb;\n"
    "    else c.rgb = v_Color.rgb;\n"
    "    if (u_AlphaOp == 2) c.a = t.a;\n"
    "    else if (u_AlphaOp == 4) c.a = t.a * v_Color.a;\n"
    "    else c.a = v_Color.a;\n"
    "    if (u_AlphaTest != 0 && c.a < u_AlphaRef) discard;\n"
    "    if (u_Fog != 0) c.rgb = mix(u_FogColor.rgb, c.rgb, v_Fog);\n"
    "    o_Color = c;\n"
    "}\n";

/*
 ----------------------------------------------------------------------------------------
  Description : the GL functions of GLI_M_GLFUNCTIONS, from the current context
 ----------------------------------------------------------------------------------------
 */
static long fn_bLoadGLFunctions(void)
{
    long bOk = 1;

#define GLI_M_GLF(Ret, Name, Args)                                                          \
    *(SDL_FunctionPointer *)&GLI_g_stGL.p_fn_##Name = SDL_GL_GetProcAddress("gl" #Name);  \
    if ( GLI_g_stGL.p_fn_##Name == NULL ) { bOk = 0; }
    GLI_M_GLFUNCTIONS
#undef GLI_M_GLF

    return bOk;
}

static GLuint fn_uiCompileShader(GLenum eType, const char *szSource)
{
    GLuint uiShader;
    GLint iStatus = 0;
    char szLog[512];

    uiShader = M_GL(CreateShader)(eType);
    M_GL(ShaderSource)(uiShader, 1, &szSource, NULL);
    M_GL(CompileShader)(uiShader);
    M_GL(GetShaderiv)(uiShader, GL_COMPILE_STATUS, &iStatus);
    if ( !iStatus )
    {
        M_GL(GetShaderInfoLog)(uiShader, sizeof(szLog), NULL, szLog);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GliGL3: shader: %s", szLog);
        M_GL(DeleteShader)(uiShader);
        return 0;
    }
    return uiShader;
}

/*
 ----------------------------------------------------------------------------------------
  Description : GL functions, shader program, vertex buffer; with the context current
 ----------------------------------------------------------------------------------------
 */
long __cdecl GLI_fn_bInitRenderer(void)
{
    GLuint uiVertex, uiFragment;
    GLint iStatus = 0;
    char szLog[512];

    if ( !fn_bLoadGLFunctions() )
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GliGL3: OpenGL 3.3 functions missing");
        return 0;
    }

    uiVertex = fn_uiCompileShader(GL_VERTEX_SHADER, gs_szVertexShader);
    uiFragment = fn_uiCompileShader(GL_FRAGMENT_SHADER, gs_szFragmentShader);
    if ( uiVertex == 0 || uiFragment == 0 )
    {
        return 0;
    }
    gs_uiProgram = M_GL(CreateProgram)();
    M_GL(AttachShader)(gs_uiProgram, uiVertex);
    M_GL(AttachShader)(gs_uiProgram, uiFragment);
    M_GL(BindAttribLocation)(gs_uiProgram, C_lAttribPosition, "a_Position");
    M_GL(BindAttribLocation)(gs_uiProgram, C_lAttribColor, "a_Color");
    M_GL(BindAttribLocation)(gs_uiProgram, C_lAttribSpecular, "a_Specular");
    M_GL(BindAttribLocation)(gs_uiProgram, C_lAttribUV, "a_UV");
    M_GL(LinkProgram)(gs_uiProgram);
    M_GL(DeleteShader)(uiVertex);
    M_GL(DeleteShader)(uiFragment);
    M_GL(GetProgramiv)(gs_uiProgram, GL_LINK_STATUS, &iStatus);
    if ( !iStatus )
    {
        M_GL(GetProgramInfoLog)(gs_uiProgram, sizeof(szLog), NULL, szLog);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GliGL3: program: %s", szLog);
        return 0;
    }
    M_GL(UseProgram)(gs_uiProgram);

    gs_iUScreenSize  = M_GL(GetUniformLocation)(gs_uiProgram, "u_ScreenSize");
    gs_iUTexture     = M_GL(GetUniformLocation)(gs_uiProgram, "u_Texture");
    gs_iUHasTexture  = M_GL(GetUniformLocation)(gs_uiProgram, "u_HasTexture");
    gs_iUAlphaFormat = M_GL(GetUniformLocation)(gs_uiProgram, "u_AlphaFormat");
    gs_iUColorOp     = M_GL(GetUniformLocation)(gs_uiProgram, "u_ColorOp");
    gs_iUAlphaOp     = M_GL(GetUniformLocation)(gs_uiProgram, "u_AlphaOp");
    gs_iUColorKey    = M_GL(GetUniformLocation)(gs_uiProgram, "u_ColorKey");
    gs_iUAlphaTest   = M_GL(GetUniformLocation)(gs_uiProgram, "u_AlphaTest");
    gs_iUAlphaRef    = M_GL(GetUniformLocation)(gs_uiProgram, "u_AlphaRef");
    gs_iUFog         = M_GL(GetUniformLocation)(gs_uiProgram, "u_Fog");
    gs_iUFogColor    = M_GL(GetUniformLocation)(gs_uiProgram, "u_FogColor");
    M_GL(Uniform1i)(gs_iUTexture, 0);

    M_GL(GenVertexArrays)(1, &gs_uiVertexArray);
    M_GL(BindVertexArray)(gs_uiVertexArray);
    M_GL(GenBuffers)(1, &gs_uiVertexBuffer);
    M_GL(BindBuffer)(GL_ARRAY_BUFFER, gs_uiVertexBuffer);
    M_GL(VertexAttribPointer)(C_lAttribPosition, 4, GL_FLOAT, GL_FALSE, sizeof(GLI_tdstTLVertex),
                              (const void *)offsetof(GLI_tdstTLVertex, sx));
    M_GL(VertexAttribPointer)(C_lAttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLI_tdstTLVertex),
                              (const void *)offsetof(GLI_tdstTLVertex, color));
    M_GL(VertexAttribPointer)(C_lAttribSpecular, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GLI_tdstTLVertex),
                              (const void *)offsetof(GLI_tdstTLVertex, specular));
    M_GL(VertexAttribPointer)(C_lAttribUV, 2, GL_FLOAT, GL_FALSE, sizeof(GLI_tdstTLVertex),
                              (const void *)offsetof(GLI_tdstTLVertex, tu));
    M_GL(EnableVertexAttribArray)(C_lAttribPosition);
    M_GL(EnableVertexAttribArray)(C_lAttribColor);
    M_GL(EnableVertexAttribArray)(C_lAttribSpecular);
    M_GL(EnableVertexAttribArray)(C_lAttribUV);

    M_GL(GenTextures)(1, &gs_uiBitmapTexture);
    M_GL(PixelStorei)(GL_UNPACK_ALIGNMENT, 1);
    M_GL(Disable)(GL_CULL_FACE);
    M_GL(Disable)(GL_DITHER);

    GLI_fn_vResetRenderStates();
    return 1;
}

void __cdecl GLI_fn_vCloseRenderer(void)
{
    if ( gs_uiProgram != 0 )
    {
        M_GL(DeleteProgram)(gs_uiProgram);
        M_GL(DeleteBuffers)(1, &gs_uiVertexBuffer);
        M_GL(DeleteVertexArrays)(1, &gs_uiVertexArray);
        M_GL(DeleteTextures)(1, &gs_uiBitmapTexture);
    }
    gs_uiProgram = 0;
    gs_uiVertexBuffer = 0;
    gs_uiVertexArray = 0;
    gs_uiBitmapTexture = 0;
    gs_p_stTexture = NULL;
}

/*
 ----------------------------------------------------------------------------------------
  Description : the states a new Direct3D 6 device starts with, then the ones the driver
                sets on it after creating it (GLI_CreateOrResetDevice of that driver)
 ----------------------------------------------------------------------------------------
 */
void __cdecl GLI_fn_vResetRenderStates(void)
{
    memset(gs_a_ulRenderState, 0, sizeof(gs_a_ulRenderState));
    memset(gs_a_ulStageState, 0, sizeof(gs_a_ulStageState));
    gs_a_ulRenderState[GLI_D3DRS_ZWRITEENABLE] = 1;
    gs_a_ulRenderState[GLI_D3DRS_SRCBLEND] = GLI_D3DBLEND_ONE;
    gs_a_ulRenderState[GLI_D3DRS_DESTBLEND] = 1;            /* D3DBLEND_ZERO */
    gs_a_ulRenderState[GLI_D3DRS_ALPHAFUNC] = GLI_D3DCMP_ALWAYS;

    gs_a_ulRenderState[GLI_D3DRS_ZENABLE] = 1;
    gs_a_ulRenderState[GLI_D3DRS_ZFUNC] = GLI_D3DCMP_GREATEREQUAL;
    gs_a_ulRenderState[GLI_D3DRS_DITHERENABLE] = 1;
    gs_a_ulRenderState[GLI_D3DRS_SPECULARENABLE] = 0;
    gs_a_ulRenderState[GLI_D3DRS_TEXTUREPERSPECTIVE] = 1;
    gs_a_ulRenderState[GLI_D3DRS_CULLMODE] = GLI_D3DCULL_NONE;
    gs_a_ulStageState[GLI_D3DTSS_COLORARG1] = GLI_D3DTA_TEXTURE;
    gs_a_ulStageState[GLI_D3DTSS_COLORARG2] = GLI_D3DTA_DIFFUSE;
    gs_a_ulStageState[GLI_D3DTSS_COLOROP] = GLI_D3DTOP_MODULATE;
    gs_a_ulStageState[GLI_D3DTSS_ALPHAARG1] = GLI_D3DTA_TEXTURE;
    gs_a_ulStageState[GLI_D3DTSS_ALPHAARG2] = GLI_D3DTA_DIFFUSE;
    gs_a_ulStageState[GLI_D3DTSS_ALPHAOP] = 2;              /* D3DTOP_SELECTARG1 */
    gs_a_ulStageState[GLI_D3DTSS_ADDRESSU] = GLI_D3DTADDRESS_WRAP;
    gs_a_ulStageState[GLI_D3DTSS_ADDRESSV] = GLI_D3DTADDRESS_WRAP;
    gs_a_ulRenderState[GLI_D3DRS_ALPHATESTENABLE] = 0;
    gs_a_ulRenderState[GLI_D3DRS_ALPHAFUNC] = GLI_D3DCMP_GREATEREQUAL;
    gs_a_ulRenderState[GLI_D3DRS_ALPHAREF] = 0xc0;
    gs_p_stTexture = NULL;
}

void __cdecl GLI_fn_vSetViewport(long lScreenWidth, long lScreenHeight, long lDrawableWidth, long lDrawableHeight)
{
    gs_xScreenWidth = (float)(lScreenWidth > 0 ? lScreenWidth : 1);
    gs_xScreenHeight = (float)(lScreenHeight > 0 ? lScreenHeight : 1);
    M_GL(Viewport)(0, 0, lDrawableWidth, lDrawableHeight);
    M_GL(Uniform2f)(gs_iUScreenSize, gs_xScreenWidth, gs_xScreenHeight);
}

/*
 ----------------------------------------------------------------------------------------
  Description : the four device calls of the shared files (GLI_DRV_M_*, USE_GLI_GL)
 ----------------------------------------------------------------------------------------
 */
void __cdecl GLI_fn_vSetRenderState(DWORD ulState, DWORD ulValue)
{
    if ( ulState < C_lNbRenderStates )
    {
        gs_a_ulRenderState[ulState] = ulValue;
    }
}

void __cdecl GLI_fn_vSetTextureStageState(DWORD ulStage, DWORD ulType, DWORD ulValue)
{
    if ( ulStage == 0 && ulType < C_lNbStageStates )
    {
        gs_a_ulStageState[ulType] = ulValue;
    }
}

void __cdecl GLI_fn_vSetTexture(DWORD ulStage, void *p_vTexture)
{
    if ( ulStage == 0 )
    {
        gs_p_stTexture = (GLI_tdstGLTexture *)p_vTexture;
    }
}

static GLenum fn_eCompareFunc(DWORD ulD3DCmp)
{
    /* D3DCMP_NEVER (1) .. D3DCMP_ALWAYS (8) are GL_NEVER .. GL_ALWAYS in the same order */
    if ( ulD3DCmp < 1 || ulD3DCmp > 8 )
    {
        return GL_ALWAYS;
    }
    return (GLenum)(GL_NEVER + ulD3DCmp - 1);
}

static GLenum fn_eBlendFactor(DWORD ulD3DBlend)
{
    switch ( ulD3DBlend )
    {
    case 1:  return GL_ZERO;
    case 2:  return GL_ONE;
    case 3:  return GL_SRC_COLOR;
    case 4:  return GL_ONE_MINUS_SRC_COLOR;
    case 5:  return GL_SRC_ALPHA;
    case 6:  return GL_ONE_MINUS_SRC_ALPHA;
    case 7:  return GL_DST_ALPHA;
    case 8:  return GL_ONE_MINUS_DST_ALPHA;
    case 9:  return GL_DST_COLOR;
    case 10: return GL_ONE_MINUS_DST_COLOR;
    default: return GL_ONE;
    }
}

static GLint fn_iWrapMode(DWORD ulD3DAddress)
{
    return ulD3DAddress == GLI_D3DTADDRESS_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT;
}

/* GL state and uniforms from the device states, before a draw */
static void fn_vApplyStates(void)
{
    DWORD ulFog;

    if ( gs_a_ulRenderState[GLI_D3DRS_ZENABLE] )
    {
        M_GL(Enable)(GL_DEPTH_TEST);
        M_GL(DepthFunc)(fn_eCompareFunc(gs_a_ulRenderState[GLI_D3DRS_ZFUNC]));
    }
    else
    {
        M_GL(Disable)(GL_DEPTH_TEST);
    }
    M_GL(DepthMask)(gs_a_ulRenderState[GLI_D3DRS_ZWRITEENABLE] ? GL_TRUE : GL_FALSE);

    if ( gs_a_ulRenderState[GLI_D3DRS_ALPHABLENDENABLE] )
    {
        M_GL(Enable)(GL_BLEND);
        M_GL(BlendFunc)(fn_eBlendFactor(gs_a_ulRenderState[GLI_D3DRS_SRCBLEND]),
                        fn_eBlendFactor(gs_a_ulRenderState[GLI_D3DRS_DESTBLEND]));
    }
    else
    {
        M_GL(Disable)(GL_BLEND);
    }

    M_GL(Uniform1i)(gs_iUAlphaTest, gs_a_ulRenderState[GLI_D3DRS_ALPHATESTENABLE] != 0 &&
                                    gs_a_ulRenderState[GLI_D3DRS_ALPHAFUNC] == GLI_D3DCMP_GREATEREQUAL);
    M_GL(Uniform1f)(gs_iUAlphaRef, (float)(gs_a_ulRenderState[GLI_D3DRS_ALPHAREF] & 0xff) / 255.0f);
    M_GL(Uniform1i)(gs_iUColorOp, (GLint)gs_a_ulStageState[GLI_D3DTSS_COLOROP]);
    M_GL(Uniform1i)(gs_iUAlphaOp, (GLint)gs_a_ulStageState[GLI_D3DTSS_ALPHAOP]);

    M_GL(Uniform1i)(gs_iUFog, gs_a_ulRenderState[GLI_D3DRS_FOGENABLE] != 0);
    ulFog = gs_a_ulRenderState[GLI_D3DRS_FOGCOLOR];
    M_GL(Uniform4f)(gs_iUFogColor, (float)((ulFog >> 16) & 0xff) / 255.0f,
                    (float)((ulFog >> 8) & 0xff) / 255.0f, (float)(ulFog & 0xff) / 255.0f, 1.0f);

    if ( gs_p_stTexture != NULL )
    {
        long lAddress;

        M_GL(BindTexture)(GL_TEXTURE_2D, gs_p_stTexture->uiName);
        lAddress = (long)((gs_a_ulStageState[GLI_D3DTSS_ADDRESSU] << 8) | gs_a_ulStageState[GLI_D3DTSS_ADDRESSV]);
        if ( gs_p_stTexture->lAddressMode != lAddress )
        {
            M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, fn_iWrapMode(gs_a_ulStageState[GLI_D3DTSS_ADDRESSU]));
            M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, fn_iWrapMode(gs_a_ulStageState[GLI_D3DTSS_ADDRESSV]));
            gs_p_stTexture->lAddressMode = lAddress;
        }
        M_GL(Uniform1i)(gs_iUHasTexture, 1);
        M_GL(Uniform1i)(gs_iUAlphaFormat, gs_p_stTexture->bAlphaFormat != 0);
        M_GL(Uniform1i)(gs_iUColorKey, gs_a_ulRenderState[GLI_D3DRS_COLORKEYENABLE] != 0);
    }
    else
    {
        M_GL(Uniform1i)(gs_iUHasTexture, 0);
        M_GL(Uniform1i)(gs_iUColorKey, 0);
    }
}

void __cdecl GLI_fn_vDrawPrimitive(DWORD ulType, DWORD ulVertexType, void *p_vVertices, DWORD ulCount, DWORD ulFlags)
{
    (void)ulVertexType;     /* always D3DFVF_TLVERTEX */
    (void)ulFlags;          /* clip / light flags: the vertices are transformed and lit */

    if ( gs_uiProgram == 0 || ulCount == 0 )
    {
        return;
    }
    fn_vApplyStates();
    M_GL(BufferData)(GL_ARRAY_BUFFER, (GLsizeiptr)(ulCount * sizeof(GLI_tdstTLVertex)), p_vVertices, GL_STREAM_DRAW);
    if ( ulType == GLI_D3DPT_LINELIST )
    {
        M_GL(DrawArrays)(GL_LINES, 0, (GLsizei)ulCount);
    }
    else
    {
        M_GL(DrawArrays)(GL_TRIANGLES, 0, (GLsizei)ulCount);
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description : clear of the whole target (Direct3D 6: viewport Clear2 on the client
                rect). The colour comes as 0x00BBGGRR; Z clears to 0 (GEQUAL test).
                GL clears respect the depth mask: it is opened for the clear.
 ----------------------------------------------------------------------------------------
 */
void __cdecl GLI_fn_vClearBuffers(long bZBuffer, long bColor, unsigned long ulColor)
{
    GLbitfield ulMask = 0;

    if ( gs_uiProgram == 0 )
    {
        return;
    }
    if ( bColor )
    {
        M_GL(ClearColor)((float)(ulColor & 0xff) / 255.0f, (float)((ulColor >> 8) & 0xff) / 255.0f,
                         (float)((ulColor >> 16) & 0xff) / 255.0f, 1.0f);
        ulMask |= GL_COLOR_BUFFER_BIT;
    }
    if ( bZBuffer )
    {
        M_GL(DepthMask)(GL_TRUE);
        M_GL(ClearDepth)(0.0);
        ulMask |= GL_DEPTH_BUFFER_BIT;
    }
    if ( ulMask != 0 )
    {
        M_GL(Clear)(ulMask);
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description : a 32-bit bitmap (bytes R, G, B, A) drawn over the rectangle (inclusive
                corners) of the screen, outside of the device states (the Direct3D 6
                driver writes such bitmaps into the back buffer directly)
 ----------------------------------------------------------------------------------------
 */
void __cdecl GLI_fn_vDrawBitmap(const unsigned long *p_ulRGBA, long lWidth, long lHeight,
                                long lLeft, long lTop, long lRight, long lBottom)
{
    GLI_tdstTLVertex a_stQuad[6];
    float xL = (float)lLeft - 0.5f, xT = (float)lTop - 0.5f;
    float xR = (float)lRight + 0.5f, xB = (float)lBottom + 0.5f;
    long i;

    if ( gs_uiProgram == 0 )
    {
        return;
    }
    M_GL(BindTexture)(GL_TEXTURE_2D, gs_uiBitmapTexture);
    M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    M_GL(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    M_GL(TexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA8, lWidth, lHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_ulRGBA);

    memset(a_stQuad, 0, sizeof(a_stQuad));
    for ( i = 0 ; i < 6 ; i++ )
    {
        a_stQuad[i].rhw = 1.0f;
        a_stQuad[i].color = 0xffffffffu;
        a_stQuad[i].specular = 0xff000000u;
    }
    a_stQuad[0].sx = xL; a_stQuad[0].sy = xT; a_stQuad[0].tu = 0.0f; a_stQuad[0].tv = 0.0f;
    a_stQuad[1].sx = xR; a_stQuad[1].sy = xT; a_stQuad[1].tu = 1.0f; a_stQuad[1].tv = 0.0f;
    a_stQuad[2].sx = xR; a_stQuad[2].sy = xB; a_stQuad[2].tu = 1.0f; a_stQuad[2].tv = 1.0f;
    a_stQuad[3] = a_stQuad[0];
    a_stQuad[4] = a_stQuad[2];
    a_stQuad[5].sx = xL; a_stQuad[5].sy = xB; a_stQuad[5].tu = 0.0f; a_stQuad[5].tv = 1.0f;

    M_GL(Disable)(GL_DEPTH_TEST);
    M_GL(Disable)(GL_BLEND);
    M_GL(DepthMask)(GL_FALSE);
    M_GL(Uniform1i)(gs_iUHasTexture, 1);
    M_GL(Uniform1i)(gs_iUAlphaFormat, 0);
    M_GL(Uniform1i)(gs_iUColorOp, GLI_D3DTOP_SELECTARG1);
    M_GL(Uniform1i)(gs_iUAlphaOp, GLI_D3DTOP_SELECTARG1);
    M_GL(Uniform1i)(gs_iUColorKey, 0);
    M_GL(Uniform1i)(gs_iUAlphaTest, 0);
    M_GL(Uniform1i)(gs_iUFog, 0);
    M_GL(BufferData)(GL_ARRAY_BUFFER, sizeof(a_stQuad), a_stQuad, GL_STREAM_DRAW);
    M_GL(DrawArrays)(GL_TRIANGLES, 0, 6);
    /* the device states are applied again before the next draw (fn_vApplyStates) */
    if ( gs_p_stTexture != NULL )
    {
        gs_p_stTexture->lAddressMode = -1;
    }
}
