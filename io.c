#include <xc.h>
#include "io.h"


// --- Funções do ADC ---

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
    ADCON2bits.ADFM = 1; // Justificação à direita
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

// --- Funções do PWM ---

// Inicializa o PWM no pino RC2 (CCP1)
void pwm_init(void)
{
    TRISCbits.RC2 = 0;      // Pino RC2 como saída
    
    PR2 = 124;              // Define o período do PWM
    
    CCPR1L = 0;             // Inicia duty cycle em 0
    CCP1CONbits.DC1B = 0;
    
    T2CONbits.T2CKPS = 0b10; // Prescaler do Timer2 para 1:16
    T2CONbits.TMR2ON = 1;    // Liga o Timer2
    
    CCP1CONbits.CCP1M = 0b1100; // Configura modo PWM
}

// Define o ciclo de trabalho (duty cycle) do PWM
void pwm_set_duty_cycle(uint16_t duty_cycle)
{
    // Separa o valor de 10 bits do duty cycle
    CCPR1L = (duty_cycle >> 2);
    CCP1CONbits.DC1B = (duty_cycle & 0x03);
}


// --- Funções de Interrupção Externa ---

// Inicializa a interrupção externa INT0 no pino RB0
void external_interrupt_init(void)
{
    TRISBbits.RB0 = 1;          // Pino RB0 como entrada
    
    INTCONbits.INT0IE = 1;      // Habilita a interrupção INT0
    INTCONbits.INT0IF = 0;      // Limpa a flag da INT0
    INTCON2bits.INTEDG0 = 1;    // Interrupção na borda de subida
    
    RCONbits.IPEN = 1;          // Habilita níveis de prioridade
    INTCONbits.GIEH = 1;        // Habilita interrupções globais
    INTCONbits.GIEL = 1;
}