/* One form of the MTH_fopt.h table functions behind wrappers that take and give the float's bits:
 * the PC branch (x87 inline assembly, wrappers A_*), or with MTH_C_DEV the C branch (wrappers C_*).
 * Built by tools/mth_fopt_check.py with INLINE = static __inline, so each object keeps its own form. */
#include "MTH/MTH_fopt.h"

#ifdef MTH_C_DEV
#define FORM(Name) C_##Name
#else /* MTH_C_DEV */
#define FORM(Name) A_##Name
#endif /* MTH_C_DEV */

unsigned long FORM(ulSquareRoot)(unsigned long ulX)
{
    float xR = MTH_fn_fSquareRootOpt(*(float *)&ulX);
    return *(unsigned long *)&xR;
}

unsigned long FORM(ulInverse)(unsigned long ulX)
{
    float xR = MTH_fn_fInverseOpt(*(float *)&ulX);
    return *(unsigned long *)&xR;
}

unsigned long FORM(ulInvSquareRoot)(unsigned long ulX)
{
    float xR = MTH_fn_fInvSquareRootOpt(*(float *)&ulX);
    return *(unsigned long *)&xR;
}
