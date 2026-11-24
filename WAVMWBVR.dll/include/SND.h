#ifndef _SND_H_
#define _SND_H_

#include "SND/snddef.h"

#include "SND/sndtype.h"
#include "SND/sound3D.h"

#ifndef DREAMCAST
#include "SND/sndfile.h"
#endif

#include "SND/snderr.h"
#include "SND/sndmem.h"

#include "SND/sndreal.h"

#include "SND/sndload.h"

#include "SND/sndldbin.h"

#ifdef PSX
#include "SNDext/PSX/sndpsx.h"
#endif

#ifdef DREAMCAST
#include "sndext/sndmem.h"
#include "sndext/sndfile.h"
#include "sndext/sndaddon.h"
#endif

#ifndef abss
#define abss( a )	( ( ( a ) < 0 ) ? -( a ) : ( a ) )
#endif

#endif
