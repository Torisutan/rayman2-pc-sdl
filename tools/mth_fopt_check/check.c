/* Runs every 32-bit pattern through the MTH_fopt.h table functions (MTH_fn_fSquareRootOpt,
 * MTH_fn_fInverseOpt, MTH_fn_fInvSquareRootOpt) and prints a checksum of the results per function.
 * With MTH_FOPT_CHECK_ASM (the retail compiler build) it also compares the C branch with the
 * x87 branch input by input. The expected checksums are those of the x87 branch built with the
 * retail compiler; a C-branch build on any target must give the same. */
#include <stdio.h>
#include <time.h>

#define D_MTH_VariableDefine
#include "MTH/MTH_globals.h"

extern void MTH_fn_vInit(void);

/* Checksums of the x87 branch, retail compiler (tools/mth_fopt_check.py) */
#ifndef MTH_FOPT_CHECK_SQRT
#define MTH_FOPT_CHECK_SQRT     0x984A5520UL
#define MTH_FOPT_CHECK_INV      0x11D158B3UL
#define MTH_FOPT_CHECK_INVSQRT  0xEF969F8CUL
#endif /* MTH_FOPT_CHECK_SQRT */

typedef unsigned long (*tdfn_ulForm)(unsigned long);

extern unsigned long C_ulSquareRoot(unsigned long);
extern unsigned long C_ulInverse(unsigned long);
extern unsigned long C_ulInvSquareRoot(unsigned long);
#ifdef MTH_FOPT_CHECK_ASM
extern unsigned long A_ulSquareRoot(unsigned long);
extern unsigned long A_ulInverse(unsigned long);
extern unsigned long A_ulInvSquareRoot(unsigned long);
#endif /* MTH_FOPT_CHECK_ASM */

typedef struct tdstCase_
{
    const char *szName;
    tdfn_ulForm fn_ulC;
    tdfn_ulForm fn_ulAsm;
    unsigned long ulExpected;
} tdstCase;

/* multiply and rotate over the results, every input from 0 to 0xFFFFFFFF in order */
static unsigned long fn_ulHash(unsigned long ulH, unsigned long ulV)
{
    ulH = ((ulH ^ ulV) * 2654435761UL) & 0xFFFFFFFFUL;
    return ((ulH << 13) | (ulH >> 19)) & 0xFFFFFFFFUL;
}

int main(void)
{
    tdstCase a_stCase[3];
    int i, iFailed = 0;
    clock_t xStart = clock();

    a_stCase[0].szName = "MTH_fn_fSquareRootOpt";
    a_stCase[0].fn_ulC = C_ulSquareRoot;
    a_stCase[0].ulExpected = MTH_FOPT_CHECK_SQRT;
    a_stCase[1].szName = "MTH_fn_fInverseOpt";
    a_stCase[1].fn_ulC = C_ulInverse;
    a_stCase[1].ulExpected = MTH_FOPT_CHECK_INV;
    a_stCase[2].szName = "MTH_fn_fInvSquareRootOpt";
    a_stCase[2].fn_ulC = C_ulInvSquareRoot;
    a_stCase[2].ulExpected = MTH_FOPT_CHECK_INVSQRT;
#ifdef MTH_FOPT_CHECK_ASM
    a_stCase[0].fn_ulAsm = A_ulSquareRoot;
    a_stCase[1].fn_ulAsm = A_ulInverse;
    a_stCase[2].fn_ulAsm = A_ulInvSquareRoot;
#endif /* MTH_FOPT_CHECK_ASM */

    MTH_fn_vInit();

    for (i = 0; i < 3; i++) {
        unsigned long ulHi, ulLo, ulHashC = 2166136261UL;
        double dCount = 0.0;
#ifdef MTH_FOPT_CHECK_ASM
        unsigned long ulHashAsm = 2166136261UL, ulMismatch = 0, ulFirst = 0;
#endif /* MTH_FOPT_CHECK_ASM */
        /* two 16-bit loops: MSVC 5.0 /O2 ends a single loop that wraps to 0 after one pass */
        for (ulHi = 0; ulHi < 0x10000; ulHi++)
        for (ulLo = 0; ulLo < 0x10000; ulLo++) {
            unsigned long ulX = (ulHi << 16) | ulLo;
            unsigned long ulC = a_stCase[i].fn_ulC(ulX);
            ulHashC = fn_ulHash(ulHashC, ulC);
#ifdef MTH_FOPT_CHECK_ASM
            {
                unsigned long ulAsm = a_stCase[i].fn_ulAsm(ulX);
                ulHashAsm = fn_ulHash(ulHashAsm, ulAsm);
                if (ulAsm != ulC && ulMismatch++ == 0)
                    ulFirst = ulX;
            }
#endif /* MTH_FOPT_CHECK_ASM */
            dCount += 1.0;
        }

#ifdef MTH_FOPT_CHECK_ASM
        printf("%-26s asm %08lx  C %08lx  mismatches %lu", a_stCase[i].szName, ulHashAsm, ulHashC, ulMismatch);
        if (ulMismatch)
            printf(" (first input %08lx)", ulFirst);
        if (ulMismatch || ulHashAsm != a_stCase[i].ulExpected)
            iFailed = 1;
#else /* MTH_FOPT_CHECK_ASM */
        printf("%-26s C %08lx", a_stCase[i].szName, ulHashC);
#endif /* MTH_FOPT_CHECK_ASM */
        if (ulHashC != a_stCase[i].ulExpected)
            iFailed = 1;
        printf("  expected %08lx  inputs %.0f\n", a_stCase[i].ulExpected, dCount);
    }
    printf("%s (%.1f s)\n", iFailed ? "FAILED" : "OK", (double)(clock() - xStart) / CLOCKS_PER_SEC);
    return iFailed;
}
