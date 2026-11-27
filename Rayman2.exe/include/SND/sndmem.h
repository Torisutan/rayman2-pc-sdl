#ifndef SND_SNDMEM_H
#define SND_SNDMEM_H

void* SND_CALL SND_fn_pvMallocSnd(unsigned long nb_octets);

void  SND_CALL SND_fn_vFreeSnd(void* ptr);

#endif
