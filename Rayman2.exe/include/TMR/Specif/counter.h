/***********************************************************************************/
/*    counter.h : target specific part of the timer library                        */
/*                                                                                 */
/*    The timer library's fast counter, as the original engine structure declares */
/*    it. Defined by TMR/sdl_timer.c (USE_SDL), else by GLI/timer_sub2.c for the   */
/*    targets without x86 inline assembly; the PC form reads the CPU's time stamp  */
/*    counter inline at its one caller (AI/FuncRay2.c).                            */
/***********************************************************************************/

#ifndef _TIMER_SPECIF_H
#define _TIMER_SPECIF_H

unsigned long TMR_fn_ulFastGetInternalCounter(void);

#endif /*_TIMER_SPECIF_H*/
