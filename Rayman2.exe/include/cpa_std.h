#ifndef __CPA_STD_H__
#define __CPA_STD_H__

#if defined(U64) || defined(NITRO)

#include "ultra.h"

#elif defined(LINUX)

#include "TMR/Specif/u_define.h"
#include "LDR/Specif/u_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#else

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#endif /* U64 */

#endif /* __CPA_STD_H__ */
