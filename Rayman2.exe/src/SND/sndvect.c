#include "SND/SND.h"

/* 0x0045B290 */
SndReal SND_CALL SND_fn_rNormeVectorSnd(SndVector *v)
{
    return (v->x >> 8) * (v->x >> 8)
         + (v->y >> 8) * (v->y >> 8)
         + (v->z >> 8) * (v->z >> 8);
}

/* 0x0045B2C0 */
static SndReal calcul_pseudo_d(SndReal a, SndReal b)
{
    SndReal aa, bb;
    aa = abss(a); bb = abss(b);

    if (aa < bb)
        return (bb + (aa >> 1));
    else
        return (aa + (bb >> 1));
}

/* 0x0045B2F0 */
SndReal SND_CALL SND_fn_rPseudoNormeVectorSnd(SndVector *v)
{
    SndReal dx, dy, dz;

    dx = abss(v->x);
    dy = abss(v->y);
    dz = abss(v->z);

    return calcul_pseudo_d(calcul_pseudo_d(dx, dy), dz);
}

/* 0x0045B330 */
SndReal SND_CALL SND_fn_rScalaireVectorSnd(SndVector *v1, SndVector *v2)
{
    return (SND_fn_rMulRealRealSnd(v1->x, v2->x)
          + SND_fn_rMulRealRealSnd(v1->y, v2->y)
          + SND_fn_rMulRealRealSnd(v1->z, v2->z));
}

/* 0x0045B370 */
SndReal SND_CALL SND_fn_rPseudoScalaireNormeVectorSnd(SndVector *v1, SndVector *v2)
{
    return (SND_fn_rMulRealRealQuickSnd(v1->x, v2->x)
          + SND_fn_rMulRealRealQuickSnd(v1->y, v2->y)
          + SND_fn_rMulRealRealQuickSnd(v1->z, v2->z));
}

/* 0x0045B3B0 */
void SND_CALL SND_fn_vProduitVectorSnd(SndVector *v1, SndVector *v2, SndVector *v)
{
    SndReal lA;

    lA = SND_fn_rMulRealRealSnd(v1->z, v2->y);
    v->x = SND_fn_rMulRealRealSnd(v1->y, v2->z) - lA;
    lA = SND_fn_rMulRealRealSnd(v1->x, v2->z);
    v->y = SND_fn_rMulRealRealSnd(v1->z, v2->x) - lA;
    v->z = SND_fn_rMulRealRealSnd(v1->x, v2->y) - SND_fn_rMulRealRealSnd(v1->y, v2->x);
}
