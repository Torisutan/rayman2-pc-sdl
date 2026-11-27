#include "SND/SND.h"
#include "SND/sndreal.h"

#ifndef snd_assert
#define snd_assert(expr) ((void)0)
#endif

#define CELERITE_SON 300

static long g_lSoundSpeed=CELERITE_SON;    /* 0x004A3F60 */
static SndReal g_rDistSatur=0x80000;   /* 0x004A3F64 */
static SndReal g_rDistFade=8*0x80000;   /* 0x004A3F68 */
static SndReal g_rDistBackGround=C_ROLLOFFPARAM_INFINITE;   /* 0x004A3F6C */

static unsigned char SND_ucVolumeBackGround;   /* 0x004B961C */

/* 0x00459C90 */
void SND_CALL SND_fn_vSetDistanceForVolume(SndReal dist_satur, SndReal dist_back)
{
    SndReal rFade;
    SndReal rBack;

    g_rDistSatur = dist_satur;
    rFade = dist_satur * 8;
    g_rDistFade = rFade;
    g_rDistBackGround = dist_back;
    rBack = dist_back;
    if (rBack < dist_satur)
    {
        rBack = 0x7fffffff;
        g_rDistBackGround = rBack;
    }
    if (rBack >= rFade)
    {
        SND_ucVolumeBackGround = 0;
        return;
    }
    SND_ucVolumeBackGround = (unsigned char)SND_fn_rMulRealRealSnd(0x7f,
        SND_fn_rDivRealRealSnd(rFade - rBack, rFade - dist_satur));
}

/* 0x00459CF0 */
void SND_CALL SND_fn_vSetDefaultRollOff(RollOffParam *rolloff)
{
    SND_fn_vSetDistanceForVolume(rolloff->rDistSatur, rolloff->rDistBackGround);
}

/* 0x00459D10 */
void SND_CALL SND_fn_vGetDefaultRollOff(RollOffParam *rolloff)
{
    rolloff->rDistSatur      = g_rDistSatur;       /* 0x4A3F64 */
    rolloff->rDistBackGround = g_rDistBackGround;  /* 0x4A3F6C */
}

/* 0x00459D30 */
unsigned char SND_CALL SND_fn_ucDistanceToVolume(SndReal dist)
{
    if (dist < g_rDistSatur)
        return C_SOUNDVOL_MAXI;

    /* 0x459D30 */
    if (dist > g_rDistBackGround)
        return SND_ucVolumeBackGround;

    if (dist > g_rDistFade)
        return C_SOUNDVOL_MINI;

    return (unsigned char)SND_fn_rMulRealRealSnd(C_SOUNDVOL_MAXI,
               SND_fn_rDivRealRealSnd(g_rDistFade - dist,
                                      g_rDistFade - g_rDistSatur));
}

/* 0x00459D80 */
unsigned char SND_CALL SND_fn_ucDistanceToVolumeEx(SndReal dist, RollOffParam *rolloff)
{
    if (rolloff)
    {
        if (rolloff->rDistBackGround < rolloff->rDistSatur)
            rolloff->rDistBackGround = C_ROLLOFFPARAM_INFINITE;

        if (dist < rolloff->rDistSatur)
            return C_SOUNDVOL_MAXI;
        if (dist > rolloff->rDistBackGround)
        {
            if (rolloff->rDistBackGround > 8 * rolloff->rDistSatur)
                return C_SOUNDVOL_MINI;
            else
                return (unsigned char)SND_fn_rMulRealRealSnd(C_SOUNDVOL_MAXI,
                           SND_fn_rDivRealRealSnd(8 * rolloff->rDistSatur - rolloff->rDistBackGround,
                                                  (8 - 1) * rolloff->rDistSatur));
        }
        else
        {
            if (dist > 8 * rolloff->rDistSatur)
                return C_SOUNDVOL_MINI;
            else
                return (unsigned char)SND_fn_rMulRealRealSnd(C_SOUNDVOL_MAXI,
                           SND_fn_rDivRealRealSnd(8 * rolloff->rDistSatur - dist,
                                                  (8 - 1) * rolloff->rDistSatur));
        }
    }
    else
        return SND_fn_ucDistanceToVolume(dist);
}

/* 0x00459E20 */
SndReal SND_CALL SND_fn_rDopplerPitch(SndVector *src_pos, SndVector *src_vit,
                                       SndVector *mic_pos, SndVector *mic_vit)
{
    SndVector dir;
    SndReal   nor;
    SndReal   retour;

    if (g_lSoundSpeed)
    {
        dir.x = (src_pos->x - mic_pos->x) >> 8;
        dir.y = (src_pos->y - mic_pos->y) >> 8;
        dir.z = (src_pos->z - mic_pos->z) >> 8;
        nor = SND_fn_rPseudoNormeVectorSnd(&dir);

        if (nor == 0) return 0x10000;

        if (nor > 0x400000)
            return 0x10000;
        retour = nor * g_lSoundSpeed;

        snd_assert(retour);

        retour = SND_fn_rDivRealRealSnd(
                     retour - SND_fn_rScalaireVectorSnd(src_vit, &dir),
                     retour - SND_fn_rScalaireVectorSnd(mic_vit, &dir));
        return retour;
    }
    else
    {
        return C_SNDREAL_1;
    }
}

/* 0x00459EC0 */
void SND_CALL SND_fn_vSetDopplerFactor(SndReal factor)
{
    if (factor)
        g_lSoundSpeed = M_RealToIntSnd(
            SND_fn_rDivRealRealSnd(M_IntToRealSnd(CELERITE_SON), factor));
    else
        g_lSoundSpeed = 0;
}

/* 0x00459EF0 */
SndReal __stdcall SND_fn_rGetDopplerFactor(void)
{
    if (g_lSoundSpeed != 0)
        return SND_fn_rDivRealRealSnd(M_IntToRealSnd(CELERITE_SON),
                                      M_IntToRealSnd(g_lSoundSpeed));
    return 0;
}

/* 0x00459F10 */
unsigned char SND_CALL SND_fn_ucPositionToPan(SndVector *src, SndVector *mic,
                                               SndVector *mic_nor, SndVector *mic_tan)
{
    SndReal s, n;
    SndVector diro;
    long pp;

    (void)mic_nor;

    diro.x = src->x - mic->x;
    diro.y = src->y - mic->y;
    diro.z = src->z - mic->z;

    n = SND_fn_rPseudoNormeVectorSnd(&diro);
    s = SND_fn_rPseudoScalaireNormeVectorSnd(&diro, mic_tan);

    if (2 * n / 256 == 0)
        return C_SOUNDPAN_MEDIAN;
    if (s <= -n)
        return C_SOUNDPAN_LEFT;
    if (s >= n)
        return C_SOUNDPAN_RIGHT;
    pp = C_SOUNDPAN_MEDIAN
         + (s / 256) * (C_SOUNDPAN_RIGHT - C_SOUNDPAN_LEFT) / (2 * n / 256);

    if (pp < C_SOUNDPAN_LEFT) pp = C_SOUNDPAN_LEFT;
    if (pp > C_SOUNDPAN_RIGHT) pp = C_SOUNDPAN_RIGHT;

    return (unsigned char)pp;
}

/* 0x00459FD0 */
unsigned char SND_CALL SND_fn_ucPositionToDolby(SndVector *src, SndVector *mic,
                                                 SndVector *mic_nor)
{
    SndReal s, n;
    SndVector diro;
    long pp;

    diro.x = src->x - mic->x;
    diro.y = src->y - mic->y;
    diro.z = src->z - mic->z;

    n = SND_fn_rPseudoNormeVectorSnd(&diro);
    s = SND_fn_rPseudoScalaireNormeVectorSnd(&diro, mic_nor);

    if (2 * n / 256 == 0)
        return (C_SOUNDSPACE_FRONT + C_SOUNDSPACE_BACK) / 2;

    if (s <= -n)
        return C_SOUNDSPACE_BACK;
    if (s >= n)
        return C_SOUNDSPACE_FRONT;
    pp = (C_SOUNDSPACE_FRONT + C_SOUNDSPACE_BACK) / 2
         + (s / 256) * (C_SOUNDSPACE_FRONT - C_SOUNDSPACE_BACK) / (2 * n / 256);

    if (pp < C_SOUNDSPACE_FRONT) pp = C_SOUNDSPACE_FRONT;
    if (pp > C_SOUNDSPACE_BACK) pp = C_SOUNDSPACE_BACK;

    return (unsigned char)pp;
}
