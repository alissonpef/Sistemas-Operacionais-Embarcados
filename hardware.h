#ifndef HARDWARE_H
#define	HARDWARE_H

#include <xc.h>

void conf_timer_0(void);
void conf_interrupts(void);

// Tratador de interrupção
void __interrupt() ISR_TIMER_0(void);


#endif	/* HARDWARE_H */

