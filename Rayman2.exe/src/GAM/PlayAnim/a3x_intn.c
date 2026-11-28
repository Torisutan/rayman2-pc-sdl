#include "GAM/A3d.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_flt.h"

void fn_v_MatrixByVector(float *p_fDst, const float *p_fMat, const float *p_fVect);
void fn_v_MatrixByDia(float *p_fDst, const float *p_fMat, const float *p_fVect);
void fn_v_RotDiaInvRot(float *p_fDst, const float *p_fMat, const float *p_fVect);
void fn_v_InterpolVect(float *p_fDst, const float *p_fA, const float *p_fB, float fT);
void fn_v_MultMatrix(float _a3a3_xMtxR[3][3], float _a3a3_xMtx1[3][3], float _a3a3_xMtx2[3][3]);
void fn_v_InterpolQuatWithOmega(float *_a4_xQuatD, float *_a4_xQuatS1, const unsigned long *_a4_xQuatS2, float _xT, int _swOmega);
void __cdecl fn_v_ExpandQuat(float *_a4_xQuatD, short *_a4_xQuatS);
void __cdecl fn_v_QuatToMatrix(float (*_a3a3_xMtx)[3], float *_a4_xQuat);

extern short GAM_pc_a_wSinTable[8192];

static const unsigned long A3X_xInterpParamScale_bits = 0x39000000u;
#define A3X_xInterpParamScale (*(const float *)&A3X_xInterpParamScale_bits)

static const unsigned long xEps_bits = 0x33D6BF95u;   /* 0x0049FAC8 */
#define A3X_xEps                (*(const float *)&xEps_bits)

#define A3X_xScaleEqualEps      0.01
#define A3X_FX_SinIdx(x)        (GAM_pc_a_wSinTable[(unsigned long)(x) & 0x1fffUL])
#define A3X_P_VECT(idx)         ((float *)&p_stSpeedVectors[(idx)])
#define A3X_P_QUAT(idx)         (&p_stAngularSpeedQuats[(idx) * 4])

/* 0x00419BD0 */
void __cdecl fn_v_InterpolBetweenKeys(tdstKeyFrame *_p_stKey1, tdstKeyFrame *_p_stKey2,
                                      tdstA3dChannel *_p_stChannel, void *_p_stCompletePosition,
                                      unsigned short _uwInterpolated, float _xT)
{
    float               a3a3_xMtxOri[3][3], a3a3_xMtxTra[3][3];
    float               a3_xPosition[3];

    static float        a4_xQuatI[4], a4_xQuatS1[4], a4_xQuatS2[4];
    static float        a3a3_xMtxOriSca[3][3], a3a3_xMtxSca[3][3];
    static float        a3_xVecSca[3];
    float               xSinOmega;
    float               xCoef1, xCoef2;
    float              *p_Vect1, *p_Vect2;
    short               swOmega, swCosOmega, swTbyOmega;

    POS_fn_vSetIdentityMatrix(_p_stCompletePosition);

    MTH3D_M_vSetIdentityMatrix((MTH3D_tdstMatrix *)a3a3_xMtxOri);
    MTH3D_M_vSetIdentityMatrix((MTH3D_tdstMatrix *)a3a3_xMtxTra);
    MTH3D_M_vNullVector((MTH3D_tdstVector *)a3_xPosition);

    if ((_xT == MTH_C_ZERO) || ((_p_stKey1->uwMask & SEB_xMaskQuatOri) != 0x0000))
    {
        if ((_p_stKey1->uwMask & SEB_xMaskIdQuatOri) == 0x0000)
        {
            fn_v_ExpandQuat(a4_xQuatI, A3X_P_QUAT(_p_stKey1->uwNumOfQuatOri));
            fn_v_QuatToMatrix((float (*)[3])a3a3_xMtxOri, a4_xQuatI);
            POS_fn_vSetRotationMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxOri[0],
                                      (MTH3D_tdstVector *)a3a3_xMtxOri[1], (MTH3D_tdstVector *)a3a3_xMtxOri[2]);
        }
    }
    else
    {
        fn_v_ExpandQuat(a4_xQuatS1, A3X_P_QUAT(_p_stKey1->uwNumOfQuatOri));
        fn_v_ExpandQuat(a4_xQuatS2, A3X_P_QUAT(_p_stKey2->uwNumOfQuatOri));
        fn_v_InterpolQuatWithOmega(a4_xQuatI, a4_xQuatS1, (const unsigned long *)a4_xQuatS2, _xT, _p_stKey1->uwAngQuatOri);
        fn_v_QuatToMatrix((float (*)[3])a3a3_xMtxOri, a4_xQuatI);
        POS_fn_vSetRotationMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxOri[0],
                                  (MTH3D_tdstVector *)a3a3_xMtxOri[1], (MTH3D_tdstVector *)a3a3_xMtxOri[2]);
    }

    if (((_p_stKey1->uwMask & (SEB_xMaskScaleValues | SEB_xMaskIdScaleValues)) == (SEB_xMaskScaleValues | SEB_xMaskIdScaleValues))
        || ((_xT == MTH_C_ZERO) && ((_p_stKey1->uwMask & SEB_xMaskIdScaleValues) != 0x0000)))
    {
        MTH3D_M_vCopyMatrix((MTH3D_tdstMatrix *)a3a3_xMtxTra, (MTH3D_tdstMatrix *)a3a3_xMtxOri);
    }

    else
    {
        fn_v_InterpolVect(a3_xVecSca, A3X_P_VECT(_p_stKey1->uwNumOfScaleValues),
                                     A3X_P_VECT(_p_stKey2->uwNumOfScaleValues), _xT);

        if ((_xT == MTH_C_ZERO) || ((_p_stKey1->uwMask & SEB_xMaskQuatSca) != 0x0000))
        {
            if ((_p_stKey1->uwMask & SEB_xMaskIdQuatSca) != 0x0000)
            {

                if (MTH_M_bEqualWithEpsilon(a3_xVecSca[0], a3_xVecSca[1], A3X_xScaleEqualEps) &&
                    MTH_M_bEqualWithEpsilon(a3_xVecSca[0], a3_xVecSca[2], A3X_xScaleEqualEps))
                {
                    POS_fn_vSetZoomMatrix(_p_stCompletePosition, a3_xVecSca[0]);
                    POS_fn_vGetTransformMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxTra[0],
                                               (MTH3D_tdstVector *)a3a3_xMtxTra[1], (MTH3D_tdstVector *)a3a3_xMtxTra[2]);
                }
                else
                {
                    fn_v_MatrixByDia((float *)a3a3_xMtxTra, (float *)a3a3_xMtxOri, a3_xVecSca);
                    POS_fn_vSetTransformMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxTra[0],
                                               (MTH3D_tdstVector *)a3a3_xMtxTra[1], (MTH3D_tdstVector *)a3a3_xMtxTra[2]);
                }
            }
            else
            {
                fn_v_ExpandQuat(a4_xQuatI, A3X_P_QUAT(_p_stKey1->uwNumOfQuatSca));
                fn_v_QuatToMatrix((float (*)[3])a3a3_xMtxOriSca, a4_xQuatI);

                if (MTH_M_bEqualWithEpsilon(a3_xVecSca[0], a3_xVecSca[1], A3X_xScaleEqualEps) &&
                    MTH_M_bEqualWithEpsilon(a3_xVecSca[0], a3_xVecSca[2], A3X_xScaleEqualEps))
                {
                    POS_fn_vSetZoomMatrix(_p_stCompletePosition, a3_xVecSca[0]);
                    POS_fn_vGetTransformMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxTra[0],
                                               (MTH3D_tdstVector *)a3a3_xMtxTra[1], (MTH3D_tdstVector *)a3a3_xMtxTra[2]);
                }
                else
                {
                    fn_v_RotDiaInvRot((float *)a3a3_xMtxSca, (float *)a3a3_xMtxOriSca, a3_xVecSca);

                    fn_v_MultMatrix( a3a3_xMtxTra, a3a3_xMtxOri, a3a3_xMtxSca );
                    POS_fn_vSetTransformMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxTra[0],
                                               (MTH3D_tdstVector *)a3a3_xMtxTra[1], (MTH3D_tdstVector *)a3a3_xMtxTra[2]);
                }
            }
        }
        else
        {
            fn_v_ExpandQuat(a4_xQuatS1, A3X_P_QUAT(_p_stKey1->uwNumOfQuatSca));
            fn_v_ExpandQuat(a4_xQuatS2, A3X_P_QUAT(_p_stKey2->uwNumOfQuatSca));
            fn_v_InterpolQuatWithOmega(a4_xQuatI, a4_xQuatS1, (const unsigned long *)a4_xQuatS2, _xT, _p_stKey1->uwAngQuatSca);
            fn_v_QuatToMatrix((float (*)[3])a3a3_xMtxOriSca, a4_xQuatI);

            if (MTH_M_bEqualWithEpsilon(a3_xVecSca[0], a3_xVecSca[1], A3X_xScaleEqualEps) &&
                MTH_M_bEqualWithEpsilon(a3_xVecSca[0], a3_xVecSca[2], A3X_xScaleEqualEps))
            {
                POS_fn_vSetZoomMatrix(_p_stCompletePosition, a3_xVecSca[0]);
                POS_fn_vGetTransformMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxTra[0],
                                           (MTH3D_tdstVector *)a3a3_xMtxTra[1], (MTH3D_tdstVector *)a3a3_xMtxTra[2]);
            }
            else
            {
                fn_v_RotDiaInvRot((float *)a3a3_xMtxSca, (float *)a3a3_xMtxOriSca, a3_xVecSca);

                fn_v_MultMatrix( a3a3_xMtxTra, a3a3_xMtxOri, a3a3_xMtxSca );
                POS_fn_vSetTransformMatrix(_p_stCompletePosition, (MTH3D_tdstVector *)a3a3_xMtxTra[0],
                                           (MTH3D_tdstVector *)a3a3_xMtxTra[1], (MTH3D_tdstVector *)a3a3_xMtxTra[2]);
            }
        }
    }

    if ((_xT == MTH_C_ZERO) || (_p_stKey1->uwMask & SEB_xMaskPosition) != 0x0000)
    {
        float r_xDistMaster;

        p_Vect1 = A3X_P_VECT(_p_stKey1->uwNumOfPosition);
        r_xDistMaster = _p_stKey1->xDistMaster;
        a3_xPosition[0] = MTH_M_xMul(p_Vect1[0], r_xDistMaster);
        a3_xPosition[1] = MTH_M_xMul(p_Vect1[1], r_xDistMaster);
        a3_xPosition[2] = MTH_M_xMul(p_Vect1[2], r_xDistMaster);
        POS_fn_vSetTranslationVector(_p_stCompletePosition, (MTH3D_tdstVector *)a3_xPosition);
    }
    else
    {
        if (_uwInterpolated == 0x0000)
        {

            fn_v_MatrixByVector(a3_xVecSca, (float *)a3a3_xMtxTra,
                                                 A3X_P_VECT(_p_stChannel->uwNumOfLocalPivotPos));

            fn_v_InterpolVect(a3_xPosition, _p_stKey1->a3_xWorldPivotPosition,
                                         _p_stKey2->a3_xWorldPivotPosition, _xT);

            a3_xPosition[0] = MTH_M_xSub(a3_xPosition[0], a3_xVecSca[0]);
            a3_xPosition[1] = MTH_M_xSub(a3_xPosition[1], a3_xVecSca[1]);
            a3_xPosition[2] = MTH_M_xSub(a3_xPosition[2], a3_xVecSca[2]);
            POS_fn_vSetTranslationVector(_p_stCompletePosition, (MTH3D_tdstVector *)a3_xPosition);
        }
        else
        {

            if (MTH_M_bGreater(_p_stKey1->xDistMaster, A3X_xEps) || MTH_M_bGreater(_p_stKey2->xDistMaster, A3X_xEps))
            {
                p_Vect1 = A3X_P_VECT(_p_stKey1->uwNumOfPosition);

                if ((_p_stKey1->uwMask & SEB_xMaskZeroAngCentre) != 0x0000)
                {

                    xCoef1 = MTH_M_xAdd(MTH_M_xMul(MTH_M_xSub(_p_stKey2->xDistMaster, _p_stKey1->xDistMaster), _xT), _p_stKey1->xDistMaster);

                    a3_xPosition[0] = MTH_M_xMul(xCoef1, p_Vect1[0]);
                    a3_xPosition[1] = MTH_M_xMul(xCoef1, p_Vect1[1]);
                    a3_xPosition[2] = MTH_M_xMul(xCoef1, p_Vect1[2]);
                    POS_fn_vSetTranslationVector(_p_stCompletePosition, (MTH3D_tdstVector *)a3_xPosition);
                }
                else
                {
                    p_Vect2 = A3X_P_VECT(_p_stKey2->uwNumOfPosition);
                    swOmega = _p_stKey1->uwAngObjCentre;

                    swCosOmega = A3X_FX_SinIdx(swOmega & 0x0000FFFF);
                    if (swCosOmega != 0)
                    {
                        xSinOmega = MTH_M_xInv(swCosOmega);
                        swTbyOmega = (short)MTH_M_xMul(_xT, swOmega);
                        xCoef1 = MTH_M_xMul(A3X_FX_SinIdx(MTH_M_xSub(swOmega, swTbyOmega) & 0x0000FFFF), xSinOmega);
                        xCoef2 = MTH_M_xMul(A3X_FX_SinIdx(swTbyOmega & 0x0000FFFF), xSinOmega);
                    }
                    else
                    {
                        xCoef1 = MTH_M_xSub(MTH_C_ONE, _xT);
                        xCoef2 = _xT;
                    }

                    xSinOmega = MTH_M_xAdd(MTH_M_xMul(MTH_M_xSub(_p_stKey2->xDistMaster, _p_stKey1->xDistMaster), _xT), _p_stKey1->xDistMaster);
                    xCoef1 = MTH_M_xMul(xCoef1, xSinOmega);
                    xCoef2 = MTH_M_xMul(xCoef2, xSinOmega);

                    a3_xPosition[0] = MTH_M_xMulAddMul(xCoef1, p_Vect1[0], xCoef2, p_Vect2[0]);
                    a3_xPosition[1] = MTH_M_xMulAddMul(xCoef1, p_Vect1[1], xCoef2, p_Vect2[1]);
                    a3_xPosition[2] = MTH_M_xMulAddMul(xCoef1, p_Vect1[2], xCoef2, p_Vect2[2]);
                    POS_fn_vSetTranslationVector(_p_stCompletePosition, (MTH3D_tdstVector *)a3_xPosition);
                }
            }
        }
    }
}
tdstA3dGENERAL *__cdecl fn_p_GetA3dGENERAL(unsigned short uwNumOfA3dGENERAL, unsigned long ulFrameNumber);
tdstA3dGENERAL *__cdecl fn_p_GetA3dGENERAL(unsigned short uwNumOfA3dGENERAL, unsigned long ulFrameNumber);
#include "GAM/PlayAnim.h"
tdstA3dGENERAL *__cdecl fn_p_GetA3dGENERAL(unsigned short uwNumOfA3dGENERAL, unsigned long ulFrameNumber);
#include "TMP/TmpMem.h"

/* 0x0041A400 */
void __cdecl fn_vUpdateFrame(HIE_tdstSuperObject *_p_stSuperObject, tdstFrame3d *_p_stFrame,
                             tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame)
{
    tdstA3dGENERAL  *p_stCurrentA3dGENERAL;
    tdstOnlyFrame   *p_stCurrentOnlyFrame;
    tdstA3dChannel  *p_stCurrentChannel;
    tdstElement3d   *p_stCurrentElement3d;
    unsigned short   uwT_NTTO;
    unsigned short   uwCnt1;
    unsigned short   uwCorrectedFrame;

    unsigned short   uwNumOfA3dGENERAL;

    (void)_p_stSuperObject;
    uwNumOfA3dGENERAL = _p_stAnim3d->uwNumOfA3dGENERAL;

    if (_uwFrame >= _p_stAnim3d->uwNumberOfFrames)
        return;

    if (_p_stFrame->p_stAnim == _p_stAnim3d && _p_stFrame->uwFrameNumber == _uwFrame)
        return;

    _p_stFrame->p_stAnim = _p_stAnim3d;
    _p_stFrame->uwFrameNumber = _uwFrame;
    _p_stFrame->ucMatrixUpdated = FALSE;

    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(uwNumOfA3dGENERAL, _uwFrame);
    uwCorrectedFrame = (unsigned short)(_uwFrame - p_stCurrentA3dGENERAL->uwStartFrame);

    p_stCurrentChannel = &p_stChannels[p_stCurrentA3dGENERAL->uwNumOfFirstChannel];
    p_stCurrentOnlyFrame = &p_stOnlyFrames[p_stCurrentA3dGENERAL->uwNumOfFirstOnlyFrame + uwCorrectedFrame];
    uwT_NTTO = p_stCurrentOnlyFrame->uwNumOfSavedFrame;

    if (p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat == SEB_Invalid)
    {
        if (_p_stFrame->p_stAngularSpeedMatrix != NULL)
            TMP_fn_vFree(&_p_stFrame->p_stAngularSpeedMatrix);
        _p_stFrame->p_stAngularSpeedMatrix = NULL;
    }
    else
    {
        float a4_xQuat[4];
        if (_p_stFrame->p_stAngularSpeedMatrix == NULL)
            _p_stFrame->p_stAngularSpeedMatrix = TMP_M_p_Malloc(0x24);
        fn_v_ExpandQuat(a4_xQuat,
            &p_stAngularSpeedQuats[p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat * 4]);
        fn_v_QuatToMatrix((float (*)[3])_p_stFrame->p_stAngularSpeedMatrix, a4_xQuat);
    }

    if (p_stCurrentOnlyFrame->uwNumOfSpeedVector == SEB_Invalid)
        _p_stFrame->p_stLinearSpeedVector = NULL;
    else
        _p_stFrame->p_stLinearSpeedVector = &p_stSpeedVectors[p_stCurrentOnlyFrame->uwNumOfSpeedVector];

    if (p_stCurrentOnlyFrame->uwHierarchyNbCouples == 0)
        _p_stFrame->stHierarchy.d_stCouples = NULL;
    else
        _p_stFrame->stHierarchy.d_stCouples = &p_stHierarchies[p_stCurrentOnlyFrame->uwNumOfFirstHierarchyCouple];
    _p_stFrame->stHierarchy.ulNbOfCouples = p_stCurrentOnlyFrame->uwHierarchyNbCouples;

    p_stCurrentElement3d = _p_stFrame->p_stArrayOfElts3d;
    for (uwCnt1 = 0; uwCnt1 < p_stCurrentA3dGENERAL->uwNumberOfChannels; uwCnt1++)
    {
        tdstA3dFrame *p_stCurrentFrame = &p_stFrames[p_stCurrentChannel->uwNumOfFirstFrame + uwT_NTTO];

        if (p_stCurrentFrame->uwNumOfNTTO != (unsigned short)0xFFFF)
        {
            tdstNTTO *p_stTheNTTO = &p_stNTTO[p_stCurrentFrame->uwNumOfNTTO];
            p_stCurrentElement3d->wElement = p_stTheNTTO->ucNumberInTable;
            p_stCurrentElement3d->ucChannelNumber = (unsigned char)p_stCurrentChannel->uwChannelNumber;
            p_stCurrentElement3d->bf1ActiveStatus = TRUE;
            p_stCurrentElement3d->eTypeOfElement = (unsigned char)(p_stTheNTTO->uwTypeOfObject & 0x00FF);
            p_stCurrentElement3d->ucTransparency = p_stTheNTTO->ucTransparency;
        }
        else
        {

            p_stCurrentElement3d->wElement = 0x7FFF;
            p_stCurrentElement3d->ucChannelNumber = (unsigned char)p_stCurrentChannel->uwChannelNumber;
            p_stCurrentElement3d->bf1ActiveStatus = TRUE;
            p_stCurrentElement3d->eTypeOfElement = TE_EmptyObject;
            p_stCurrentElement3d->ucTransparency = 0;
        }

        p_stCurrentChannel++;
        p_stCurrentElement3d++;
    }
}

MTH3D_tdstVector *__cdecl fn_p_stGetLinearSpeedVector(tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame)
{
    unsigned short uwNumOfA3dGENERAL;
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    tdstOnlyFrame *p_stCurrentOnlyFrame;

    if (_uwFrame >= _p_stAnim3d->uwNumberOfFrames)
        return NULL;

    uwNumOfA3dGENERAL = _p_stAnim3d->uwNumOfA3dGENERAL;
    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(uwNumOfA3dGENERAL, _uwFrame);

    _uwFrame -= p_stCurrentA3dGENERAL->uwStartFrame;

    p_stCurrentOnlyFrame = &p_stOnlyFrames[p_stCurrentA3dGENERAL->uwNumOfFirstOnlyFrame + _uwFrame];

    if (p_stCurrentOnlyFrame->uwNumOfSpeedVector == SEB_Invalid)
        return NULL;
    return &p_stSpeedVectors[p_stCurrentOnlyFrame->uwNumOfSpeedVector];
}

MTH3D_tdstMatrix *__cdecl fn_p_stGetAngularSpeedMatrix(tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame,
                                                       MTH3D_tdstMatrix *p_stMatrix)
{
    unsigned short uwNumOfA3dGENERAL;
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    tdstOnlyFrame *p_stCurrentOnlyFrame;

    if (_uwFrame >= _p_stAnim3d->uwNumberOfFrames)
        return NULL;

    uwNumOfA3dGENERAL = _p_stAnim3d->uwNumOfA3dGENERAL;
    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(uwNumOfA3dGENERAL, _uwFrame);

    _uwFrame -= p_stCurrentA3dGENERAL->uwStartFrame;

    p_stCurrentOnlyFrame = &p_stOnlyFrames[p_stCurrentA3dGENERAL->uwNumOfFirstOnlyFrame + _uwFrame];

    if (p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat == SEB_Invalid)
    {
        return NULL;
    }
    else
    {
        float a4_xQuatOri[4];

        fn_v_ExpandQuat(a4_xQuatOri, &p_stAngularSpeedQuats[p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat * 4]);

        fn_v_QuatToMatrix((float (*)[3])(p_stMatrix), a4_xQuatOri);

        return p_stMatrix;
    }
}
