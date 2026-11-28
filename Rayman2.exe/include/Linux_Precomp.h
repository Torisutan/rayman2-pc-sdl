/****************************************
 * Linux_Precomp.h
 * Prefix of the Linux target: the build includes it first in every file
 * (gcc/clang -include), as the console projects of the original engine
 * structure include their prefix. The target's configuration comes from the
 * build (LINUX and the feature macros).
 *
 * Besides the target's system headers it includes cpa_std.h: the decomp
 * spells the calling conventions in files that reach cpa_std.h through no
 * include of theirs.
 ****************************************/

#include "cpa_std.h"
