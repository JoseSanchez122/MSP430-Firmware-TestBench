#ifndef TIMERS_H_
#define TIMERS_H_

extern volatile unsigned int Ticks_TB0;

extern void CFG_TB0_1ms();
extern void SMCLK_INIT();
extern void SMCLK_INIT_1MHZ();
extern void Start_TimerT_A0_ACLK_1seg(int time);
extern void Stop_TimerA(void);

#endif /* TIMERS_H_ */