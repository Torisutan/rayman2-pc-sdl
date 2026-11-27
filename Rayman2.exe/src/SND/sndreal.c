#include <math.h>
#include <stdlib.h>
#include "SND/SND.h"

SndReal SND_CALL SND_fn_rMulRealRealSnd(SndReal r1, SndReal r2)
{
    unsigned long r1p, r2p, t1, t2, t3, t4, r3p;

    r1p = abss(r1);
    r2p = abss(r2);

    if ((r1p < 0x800) && (r2p < 0x800) && (r1p > 0xFF) && (r2p > 0xFF))
        return ((r1 >> 8) * (r2 >> 8));

    if (r1p < 0x7F)
        return ((r1 * (r2 >> 8)) >> 8);

    if (r2p < 0x7F)
        return ((r2 * (r1 >> 8)) >> 8);

    t1 = (r1p & 0xFFFF) * (r2p & 0xFFFF);
    t2 = (r1p & 0xFFFF) * (r2p >> 16);
    t3 = (r1p >> 16) * (r2p & 0xFFFF);
    t4 = (r1p >> 16) * (r2p >> 16);

    r3p = (t1 >> 16) + t2 + t3 + (t4 << 16);

    if ((r1 < 0) ^ (r2 < 0))
        return (SndReal)(0 - r3p);
    else
        return (SndReal)r3p;
}

SndReal SND_CALL SND_fn_rMulRealRealQuickSnd(SndReal r1, SndReal r2)
{
    return ((r1 >> 8) * (r2 >> 8));
}

/* 0x00458760 */
SndReal SND_CALL SND_fn_rSqrtRealSnd(SndReal r)
{
    static const double c_rFrom16_16 = 1.52587890625e-5;  /* 0x0049C980 */
    static const double c_rTo16_16 = 65536.0;             /* 0x0049C988 */

    return (SndReal)(sqrt((double)r * c_rFrom16_16) * c_rTo16_16);
}

SndReal SND_CALL SND_fn_rDivRealRealSnd(SndReal r1, SndReal r2)
{
    static const float c_rTo16_16 = 65536.0f;   /* 0x0049C990 */

    return (SndReal)((double)r1 / (double)r2 * c_rTo16_16);
}

/* 0x004587A0 */
SndReal SND_CALL SND_fn_rAbsRealSnd(SndReal r)
{
    return labs(r);
}

/* 0x004587B0 */
SndReal SND_CALL SND_fn_rPseudoNormeRealSnd(SndReal r1, SndReal r2, SndReal r3)
{
    SndVector v;

    v.x = r1;
    v.y = r2;
    v.z = r3;
    return SND_fn_rPseudoNormeVectorSnd(&v);
}

void SND_CALL SND_fn_vSndRealTwinToSndReal(SndRealTwin rtwin,
                                            SndReal *pr1, SndReal *pr2)
{
    if (pr1)
        *pr1 = (rtwin >> 16) << 8;

    if (pr2)
        *pr2 = (rtwin << 16) >> 8;
}

/* 0x00458810 */
SndRealTwin SND_CALL SND_fn_rtSndRealToSndRealTwin(SndReal r1, SndReal r2)
{
    return (((r1 & 0xFFFFFF00L) << 8) | ((r2 >> 8) & 0xFFFFL));
}
