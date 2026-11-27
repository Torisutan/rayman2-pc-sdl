/*
 ----------------------------------------------------------------------------------------
  GliGL3vf.h : the OpenGL 3.3 (core profile) renderer driver.

  A driver module beside the Direct3D 6 one, with the same entry points (GLI_DRV_*, bound
  by name by GLI_fnl_InitDll). It compiles the Direct3D 6 driver's device-independent files
  (clip pipeline, render states, texture formats and costs, common functions and data) and
  supplies the device layer they call: device creation and reset, flips, clears, texture
  download and the four device calls of GliDX6vf_State.h (GLI_DRV_M_*, USE_GLI_GL branch).
  The window and the GL context come from SDL, on the window GAM/sdl_main.c owns.
 ----------------------------------------------------------------------------------------
 */
#ifndef GLIGL3VF_H
#define GLIGL3VF_H

#include <stddef.h>

#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GLI/devinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- GL types and constants used by the driver (OpenGL 3.3 core) ------------------- */
typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef int             GLint;
typedef unsigned int    GLuint;
typedef int             GLsizei;
typedef float           GLfloat;
typedef double          GLdouble;
typedef char            GLchar;
typedef unsigned char   GLubyte;
typedef ptrdiff_t       GLsizeiptr;
typedef ptrdiff_t       GLintptr;

#define GL_FALSE                    0
#define GL_TRUE                     1
#define GL_TRIANGLES                0x0004
#define GL_LINES                    0x0001
#define GL_NEVER                    0x0200
#define GL_LESS                     0x0201
#define GL_EQUAL                    0x0202
#define GL_LEQUAL                   0x0203
#define GL_GREATER                  0x0204
#define GL_NOTEQUAL                 0x0205
#define GL_GEQUAL                   0x0206
#define GL_ALWAYS                   0x0207
#define GL_ZERO                     0
#define GL_ONE                      1
#define GL_SRC_COLOR                0x0300
#define GL_ONE_MINUS_SRC_COLOR      0x0301
#define GL_SRC_ALPHA                0x0302
#define GL_ONE_MINUS_SRC_ALPHA      0x0303
#define GL_DST_ALPHA                0x0304
#define GL_ONE_MINUS_DST_ALPHA      0x0305
#define GL_DST_COLOR                0x0306
#define GL_ONE_MINUS_DST_COLOR      0x0307
#define GL_DEPTH_TEST               0x0B71
#define GL_BLEND                    0x0BE2
#define GL_SCISSOR_TEST             0x0C11
#define GL_CULL_FACE                0x0B44
#define GL_DITHER                   0x0BD0
#define GL_UNPACK_ALIGNMENT         0x0CF5
#define GL_TEXTURE_2D               0x0DE1
#define GL_UNSIGNED_BYTE            0x1401
#define GL_UNSIGNED_SHORT           0x1403
#define GL_FLOAT                    0x1406
#define GL_RGBA                     0x1908
#define GL_RGBA8                    0x8058
#define GL_VENDOR                   0x1F00
#define GL_RENDERER                 0x1F01
#define GL_VERSION                  0x1F02
#define GL_NEAREST                  0x2600
#define GL_LINEAR                   0x2601
#define GL_LINEAR_MIPMAP_LINEAR     0x2703
#define GL_TEXTURE_MAG_FILTER       0x2800
#define GL_TEXTURE_MIN_FILTER       0x2801
#define GL_TEXTURE_WRAP_S           0x2802
#define GL_TEXTURE_WRAP_T           0x2803
#define GL_REPEAT                   0x2901
#define GL_CLAMP_TO_EDGE            0x812F
#define GL_TEXTURE_MAX_LEVEL        0x813D
#define GL_MAX_TEXTURE_SIZE         0x0D33
#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_DEPTH_BUFFER_BIT         0x00000100
#define GL_TEXTURE0                 0x84C0
#define GL_ARRAY_BUFFER             0x8892
#define GL_STREAM_DRAW              0x88E0
#define GL_FRAGMENT_SHADER          0x8B30
#define GL_VERTEX_SHADER            0x8B31
#define GL_COMPILE_STATUS           0x8B81
#define GL_LINK_STATUS              0x8B82
#define GL_INFO_LOG_LENGTH          0x8B84
#define GL_NO_ERROR                 0

#if defined(WIN32)
#define GLI_GL_CALL __stdcall
#else
#define GLI_GL_CALL
#endif /* WIN32 */

/* The GL functions the driver calls, looked up once the context exists (no loader library:
   SDL_GL_GetProcAddress). One list, expanded into the table and its lookup. */
#define GLI_M_GLFUNCTIONS                                                                       \
    GLI_M_GLF(void,   Viewport,                (GLint x, GLint y, GLsizei w, GLsizei h))        \
    GLI_M_GLF(void,   Scissor,                 (GLint x, GLint y, GLsizei w, GLsizei h))        \
    GLI_M_GLF(void,   Enable,                  (GLenum cap))                                    \
    GLI_M_GLF(void,   Disable,                 (GLenum cap))                                    \
    GLI_M_GLF(void,   ClearColor,              (GLfloat r, GLfloat g, GLfloat b, GLfloat a))    \
    GLI_M_GLF(void,   ClearDepth,              (GLdouble d))                                    \
    GLI_M_GLF(void,   Clear,                   (GLbitfield mask))                               \
    GLI_M_GLF(void,   DepthFunc,               (GLenum func))                                   \
    GLI_M_GLF(void,   DepthMask,               (GLboolean flag))                                \
    GLI_M_GLF(void,   BlendFunc,               (GLenum s, GLenum d))                            \
    GLI_M_GLF(void,   PixelStorei,             (GLenum name, GLint param))                      \
    GLI_M_GLF(void,   GetIntegerv,             (GLenum name, GLint *data))                      \
    GLI_M_GLF(const GLubyte *, GetString,      (GLenum name))                                   \
    GLI_M_GLF(GLenum, GetError,                (void))                                          \
    GLI_M_GLF(void,   GenTextures,             (GLsizei n, GLuint *names))                      \
    GLI_M_GLF(void,   DeleteTextures,          (GLsizei n, const GLuint *names))                \
    GLI_M_GLF(void,   BindTexture,             (GLenum target, GLuint name))                    \
    GLI_M_GLF(void,   TexImage2D,              (GLenum target, GLint level, GLint ifmt, GLsizei w, GLsizei h, GLint border, GLenum fmt, GLenum type, const void *data)) \
    GLI_M_GLF(void,   TexSubImage2D,           (GLenum target, GLint level, GLint x, GLint y, GLsizei w, GLsizei h, GLenum fmt, GLenum type, const void *data)) \
    GLI_M_GLF(void,   TexParameteri,           (GLenum target, GLenum name, GLint param))       \
    GLI_M_GLF(void,   ActiveTexture,           (GLenum unit))                                   \
    GLI_M_GLF(GLuint, CreateShader,            (GLenum type))                                   \
    GLI_M_GLF(void,   ShaderSource,            (GLuint s, GLsizei n, const GLchar *const *src, const GLint *len)) \
    GLI_M_GLF(void,   CompileShader,           (GLuint s))                                      \
    GLI_M_GLF(void,   GetShaderiv,             (GLuint s, GLenum name, GLint *param))           \
    GLI_M_GLF(void,   GetShaderInfoLog,        (GLuint s, GLsizei max, GLsizei *len, GLchar *log)) \
    GLI_M_GLF(void,   DeleteShader,            (GLuint s))                                      \
    GLI_M_GLF(GLuint, CreateProgram,           (void))                                          \
    GLI_M_GLF(void,   AttachShader,            (GLuint p, GLuint s))                            \
    GLI_M_GLF(void,   BindAttribLocation,      (GLuint p, GLuint index, const GLchar *name))    \
    GLI_M_GLF(void,   LinkProgram,             (GLuint p))                                      \
    GLI_M_GLF(void,   GetProgramiv,            (GLuint p, GLenum name, GLint *param))           \
    GLI_M_GLF(void,   GetProgramInfoLog,       (GLuint p, GLsizei max, GLsizei *len, GLchar *log)) \
    GLI_M_GLF(void,   DeleteProgram,           (GLuint p))                                      \
    GLI_M_GLF(void,   UseProgram,              (GLuint p))                                      \
    GLI_M_GLF(GLint,  GetUniformLocation,      (GLuint p, const GLchar *name))                  \
    GLI_M_GLF(void,   Uniform1i,               (GLint loc, GLint v))                            \
    GLI_M_GLF(void,   Uniform1f,               (GLint loc, GLfloat v))                          \
    GLI_M_GLF(void,   Uniform2f,               (GLint loc, GLfloat v0, GLfloat v1))             \
    GLI_M_GLF(void,   Uniform4f,               (GLint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)) \
    GLI_M_GLF(void,   GenVertexArrays,         (GLsizei n, GLuint *arrays))                     \
    GLI_M_GLF(void,   DeleteVertexArrays,      (GLsizei n, const GLuint *arrays))               \
    GLI_M_GLF(void,   BindVertexArray,         (GLuint array))                                  \
    GLI_M_GLF(void,   GenBuffers,              (GLsizei n, GLuint *buffers))                    \
    GLI_M_GLF(void,   DeleteBuffers,           (GLsizei n, const GLuint *buffers))              \
    GLI_M_GLF(void,   BindBuffer,              (GLenum target, GLuint buffer))                  \
    GLI_M_GLF(void,   BufferData,              (GLenum target, GLsizeiptr size, const void *data, GLenum usage)) \
    GLI_M_GLF(void,   VertexAttribPointer,     (GLuint index, GLint size, GLenum type, GLboolean norm, GLsizei stride, const void *ptr)) \
    GLI_M_GLF(void,   EnableVertexAttribArray, (GLuint index))                                  \
    GLI_M_GLF(void,   DrawArrays,              (GLenum mode, GLint first, GLsizei count))

#define GLI_M_GLF(Ret, Name, Args) Ret (GLI_GL_CALL *p_fn_##Name) Args;
typedef struct GLI_tdstGLFunctions_
{
    GLI_M_GLFUNCTIONS
} GLI_tdstGLFunctions;
#undef GLI_M_GLF

extern GLI_tdstGLFunctions GLI_g_stGL;

/* ---- A texture as the driver keeps it (the engine's texture holds a pointer to it where
   the Direct3D 6 driver keeps its texture interface, offset 0x10) ------------------------ */
typedef struct GLI_tdstGLTexture_
{
    GLuint  uiName;             /* GL texture object */
    long    bAlphaFormat;       /* the pixel format the engine chose has alpha bits */
    long    bColorKey;          /* key pixels carry alpha 0 (colour key, format without alpha) */
    long    bMipMaps;           /* mip levels uploaded */
    long    lAddressMode;       /* wrap mode last set on it (-1: none yet) */
} GLI_tdstGLTexture;

/* ---- Device (ogl_dev.c) ------------------------------------------------------------- */
extern void *GLI_g_p_stSDLWindow;       /* the SDL window the context renders to */
extern void *GLI_g_hGLContext;          /* the GL context */
extern long  GLI_g_lDrawableWidth;      /* size of the drawable, in pixels */
extern long  GLI_g_lDrawableHeight;

long __cdecl GLI_fn_bCreateGLContext(HWND hWnd);
void __cdecl GLI_fn_vDestroyGLContext(void);
void __cdecl GLI_fn_vSwapBuffers(long bSynchro);

/* ---- Rendering (ogl_draw.c) ---------------------------------------------------------- */
long __cdecl GLI_fn_bInitRenderer(void);
void __cdecl GLI_fn_vCloseRenderer(void);
void __cdecl GLI_fn_vResetRenderStates(void);
void __cdecl GLI_fn_vSetViewport(long lScreenWidth, long lScreenHeight, long lDrawableWidth, long lDrawableHeight);
void __cdecl GLI_fn_vClearBuffers(long bZBuffer, long bColor, unsigned long ulColor);
void __cdecl GLI_fn_vDrawBitmap(const unsigned long *p_ulRGBA, long lWidth, long lHeight,
                                long lLeft, long lTop, long lRight, long lBottom);

/* ---- Textures (ogl_tex.c) ------------------------------------------------------------ */
void __cdecl GLI_fn_vInitTextureFormats(void);

#ifdef __cplusplus
}
#endif

#endif /* GLIGL3VF_H */
