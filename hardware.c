#include "hardware.h"
#include "kernel.h"
#include "scheduler.h"
#include <xc.h>


void conf_timer_0(void)
{
    // Configuração do periférico do TIMER (timer zero)
    T0CONbits.T0CS      = 0;        // Clock interno (Fosc/4)
    T0CONbits.PSA       = 0;        // Habilita o prescaler
    T0CONbits.T0PS      = 0b110;    // Prescaler de 1:128
    TMR0                = 128;      // Carrega valor inicial do timer
    INTCONbits.TMR0IE   = 1;        // Habilita a interrupção do Timer0
    INTCONbits.TMR0IF   = 0;        // Limpa a flag de interrupção do Timer0
    T0CONbits.TMR0ON    = 1;        // Liga o Timer0
}

void conf_interrupts(void)
{
    INTCONbits.PEIE     = 1;        // Habilita interrupções de periféricos
}

// Tratador de interrupção
void __interrupt() ISR_TIMER_0(void)
{
    di();
    
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0;
        
        // Diminui o tempo das tarefas em espera (delay)
        os_task_time_decrease();
        
        SAVE_CONTEXT(READY);
        scheduler();
        RESTORE_CONTEXT();
    }
    
    ei();
}