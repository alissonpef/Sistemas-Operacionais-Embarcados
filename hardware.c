#include "hardware.h"
#include "kernel.h"
#include "scheduler.h"
#include <xc.h>


void conf_timer_0(void)
{
    // Configuração do periférico do TIMER (timer zero)
    T0CONbits.T0CS      = 0;        // Mudança de instrução interna
    T0CONbits.PSA       = 0;        // Ativa preescale
    T0CONbits.T0PS      = 0b110;    // 1:128
    TMR0                = 128;      // Carrega offset do timer
    INTCONbits.TMR0IE   = 1;        // Ativa interrupção por timer
    INTCONbits.TMR0IF   = 0;        // Seta flag do timer em zero
    T0CONbits.TMR0ON    = 1;        // Liga o timer
}

void conf_interrupts(void)
{
    INTCONbits.PEIE     = 1;        // Interrupções de periférico ativida    
}

// Tratador de interrupção
void __interrupt() ISR_TIMER_0(void)
{
    di();
    
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0;
        
        // Diminui tempo das tarefas em espera
        os_task_time_decrease();
        
        SAVE_CONTEXT(READY);
        scheduler();
        RESTORE_CONTEXT();
    }
    
    ei();
}
