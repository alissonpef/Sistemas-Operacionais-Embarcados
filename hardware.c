#include "hardware.h"
#include "kernel.h"
#include "scheduler.h"
#include "io.h"
#include <xc.h>

// Vari�veis externas do PWM (definidas em io.c)
extern uint16_t motor_a_duty, motor_b_duty, motor_c_duty, motor_d_duty;

void conf_timer_0(void)
{
    // Configura��o do perif�rico do TIMER (timer zero)
    T0CONbits.T0CS      = 0;        // Clock interno (Fosc/4)
    T0CONbits.PSA       = 0;        // Habilita o prescaler
    T0CONbits.T0PS      = 0b110;    // Prescaler de 1:128
    TMR0                = 128;      // Carrega valor inicial do timer
    INTCONbits.TMR0IE   = 1;        // Habilita a interrup��o do Timer0
    INTCONbits.TMR0IF   = 0;        // Limpa a flag de interrup��o do Timer0
    T0CONbits.TMR0ON    = 1;        // Liga o Timer0
}

void conf_interrupts(void)
{
    INTCONbits.PEIE     = 1;        // Habilita interrup��es de perif�ricos
}

// Tratador de interrup��o OTIMIZADO
void __interrupt() ISR_TIMER_0(void)
{
    // Interrup��o do Timer1 - PWM por software (PRIORIDADE)
    // INLINE para economizar stack
    if (PIR1bits.TMR1IF == 1) {
        PIR1bits.TMR1IF = 0;
        TMR1 = 65535 - 500;  // Reduz frequ�ncia (menos sobrecarga)
        
        static uint16_t pwm_cnt = 0;
        pwm_cnt++;
        if (pwm_cnt >= 1024) pwm_cnt = 0;
        
        // Atualiza LEDs diretamente (sem fun��o)
        LATDbits.LATD0 = (pwm_cnt < motor_a_duty) ? 1 : 0;
        LATDbits.LATD1 = (pwm_cnt < motor_b_duty) ? 1 : 0;
        LATDbits.LATD2 = (pwm_cnt < motor_c_duty) ? 1 : 0;
        LATDbits.LATD3 = (pwm_cnt < motor_d_duty) ? 1 : 0;
    }
    
    // Interrup��o do Timer0 - Tick do RTOS
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0;
        
        di();
        
        os_task_time_decrease();
        
        SAVE_CONTEXT(READY);
        scheduler();
        RESTORE_CONTEXT();
        
        ei();
    }
}
