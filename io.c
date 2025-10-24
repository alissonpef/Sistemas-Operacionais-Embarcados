#include <xc.h>
#include "io.h"


// --- Fun??es do ADC ---

void set_channel(channel_t channel)
{
    ADCON0bits.CHS = channel;
}

void set_port(port_conf_t port)
{
    ADCON1bits.PCFG = port;
}

void config_adc(tad_t tad, conversion_clock_t cclk)
{
    ADCON2bits.ACQT = tad;
    ADCON2bits.ADCS = cclk;
    ADCON2bits.ADFM = 1; // Justifica??o ? direita
}

void adc_go(int go_done)
{
    ADCON0bits.ADON = go_done;
}

int adc_read()
{
    ADCON0bits.GO = 1;
    while (ADCON0bits.GODONE);
    return ADRES;
}

// --- Fun??es do PWM ---

// Vari?veis globais para controle de PWM por software (N?O static - usadas em hardware.c)
uint16_t motor_a_duty = 0;  // RD0
uint16_t motor_b_duty = 0;  // RD1
uint16_t motor_c_duty = 0;  // RD2
uint16_t motor_d_duty = 0;  // RD3
static uint16_t pwm_counter = 0;

// Inicializa os 4 canais PWM para controle dos motores (todos em PORTD)
void pwm_init(void)
{
    // --- Configura TODOS os 4 motores como sa?das digitais ---
    // Conforme esquem?tico: PORTD pinos 19-22 (RD0, RD1, RD2, RD3)
    TRISDbits.RD0 = 0;      // Motor A (pino 19)
    TRISDbits.RD1 = 0;      // Motor B (pino 20)
    TRISDbits.RD2 = 0;      // Motor C (pino 21)
    TRISDbits.RD3 = 0;      // Motor D (pino 22)
    
    // Inicializa LEDs em estado desligado (PWM controlará)
    LATDbits.LATD0 = 0;
    LATDbits.LATD1 = 0;
    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 0;
    
    // Inicializa duty cycles em 50% para teste
    motor_a_duty = 512;
    motor_b_duty = 512;
    motor_c_duty = 512;
    motor_d_duty = 512;
    
    // --- Configura Timer1 para gerar PWM por software ---
    T1CONbits.TMR1CS = 0;   // Clock interno (Fosc/4)
    T1CONbits.T1CKPS = 0b00; // Prescaler 1:1
    T1CONbits.RD16 = 1;     // Modo 16 bits
    TMR1 = 0;               // Zera o contador
    
    // Habilita interrup??o do Timer1
    PIE1bits.TMR1IE = 1;    // Habilita interrup??o Timer1
    PIR1bits.TMR1IF = 0;    // Limpa flag
    
    T1CONbits.TMR1ON = 1;   // Liga Timer1
}

// MOTOR A (RD0): Define o duty cycle
void pwm_set_duty_cycle_motor1(uint16_t duty_cycle)
{
    if (duty_cycle > 1023) duty_cycle = 1023;
    motor_a_duty = duty_cycle;
}

// MOTOR B (RD1): Define o duty cycle
void pwm_set_duty_cycle_motor2(uint16_t duty_cycle)
{
    if (duty_cycle > 1023) duty_cycle = 1023;
    motor_b_duty = duty_cycle;
}

// MOTOR C (RD2): Define o duty cycle
void pwm_set_duty_cycle_motor3(uint16_t duty_cycle)
{
    if (duty_cycle > 1023) duty_cycle = 1023;
    motor_c_duty = duty_cycle;
}

// MOTOR D (RD3): Define o duty cycle
void pwm_set_duty_cycle_motor4(uint16_t duty_cycle)
{
    if (duty_cycle > 1023) duty_cycle = 1023;
    motor_d_duty = duty_cycle;
}

// FUNÇÃO NÃO UTILIZADA - PWM controlado por ISR em hardware.c
// void pwm_software_isr(void)
// {
//     // Código movido para hardware.c ISR_TIMER_0()
// }


// --- Fun??es de Interrup??o Externa ---

// Inicializa a interrup??o externa INT0 no pino RB0
void external_interrupt_init(void)
{
    TRISBbits.RB0 = 1;          // Pino RB0 como entrada
    
    INTCONbits.INT0IE = 1;      // Habilita a interrup??o INT0
    INTCONbits.INT0IF = 0;      // Limpa a flag da INT0
    INTCON2bits.INTEDG0 = 1;    // Interrup??o na borda de subida
    
    RCONbits.IPEN = 1;          // Habilita n?veis de prioridade
    INTCONbits.GIEH = 1;        // Habilita interrup??es globais
    INTCONbits.GIEL = 1;
}
