#include <string.h>

#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

/* 0x10008940 */
__declspec(dllexport) void __cdecl
GLI_DRV_vAddBlackPolygon(long lLeft, long lTop, long lRight, long lBottom)
{
    GLI_tdstTLVertex *p_stVertex;

    if (GLI_g_BlackPolygonVertexCount + 6 > GLI_BLACK_POLYGON_MAX_VERTICES)
        return;

    p_stVertex = &GLI_g_BlackPolygonVertices[GLI_g_BlackPolygonVertexCount];
    memset(p_stVertex, 0, 6 * sizeof(GLI_tdstTLVertex));
    GLI_g_BlackPolygonVertexCount += 6;

    p_stVertex->sx = (float)lLeft;
    p_stVertex->sy = (float)lBottom;
    p_stVertex->rhw = GLI_g_fBlackPolygonRhw;
    p_stVertex++;
    p_stVertex->sx = (float)lLeft;
    p_stVertex->sy = (float)lTop;
    p_stVertex->rhw = GLI_g_fBlackPolygonRhw;
    p_stVertex++;
    p_stVertex->sx = (float)lRight;
    p_stVertex->sy = (float)lTop;
    p_stVertex->rhw = GLI_g_fBlackPolygonRhw;
    p_stVertex++;
    p_stVertex->sx = (float)lRight;
    p_stVertex->sy = (float)lTop;
    p_stVertex->rhw = GLI_g_fBlackPolygonRhw;
    p_stVertex++;
    p_stVertex->sx = (float)lRight;
    p_stVertex->sy = (float)lBottom;
    p_stVertex->rhw = GLI_g_fBlackPolygonRhw;
    p_stVertex++;
    p_stVertex->sx = (float)lLeft;
    p_stVertex->sy = (float)lBottom;
    p_stVertex->rhw = GLI_g_fBlackPolygonRhw;
}
