#include <pic18f4550.h>

#include "io.h"


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
    ADCON2bits.ADFM = 1;
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

// --- Implementação do PWM (usando CCP1 no pino RC2) ---

/**
 * @brief Inicializa o PWM no pino RC2 (CCP1).
 * * Configura o Timer2 para ser a base de tempo do PWM e 
 * habilita o módulo CCP1 em modo PWM.
 * Frequência do PWM = Fosc / (4 * (PR2 + 1) * TMR2_Prescale)
 * Ex: Fosc=8MHz, Prescale=16, PR2=124 => F_pwm = 1kHz
 */
void pwm_init(void)
{
    TRISCbits.RC2 = 0;      // Configura o pino RC2 (CCP1) como saída
    
    // 1. Configurar o período do PWM (frequência) com o registrador PR2
    PR2 = 124;              // Define a frequência do PWM (ex: 1kHz com Fosc=8MHz)
    
    // 2. Configurar o ciclo de trabalho (duty cycle) com CCPR1L e CCP1CON
    CCPR1L = 0;             // Inicia com duty cycle 0
    CCP1CONbits.DC1B = 0;   // Inicia com duty cycle 0
    
    // 3. Configurar o Timer2
    T2CONbits.T2CKPS = 0b10; // Prescaler do Timer2 para 1:16
    T2CONbits.TMR2ON = 1;    // Liga o Timer2
    
    // 4. Configurar o módulo CCP1 para o modo PWM
    CCP1CONbits.CCP1M = 0b1100; // Modo PWM com saídas P1A e P1C ativas
}

/**
 * @brief Define o ciclo de trabalho (duty cycle) do PWM.
 * * @param duty_cycle Um valor de 10 bits (0 a 1023) que representa o duty cycle.
 * Para este exemplo com PR2=124, o valor máximo é (PR2+1)*4 = 500.
 * Vamos simplificar e usar um valor de 0 a 255.
 */
void pwm_set_duty_cycle(uint16_t duty_cycle)
{
    // O valor do duty cycle é de 10 bits. 
    // Os 8 bits mais significativos vão em CCPR1L
    // Os 2 bits menos significativos vão em CCP1CON<5:4>
    CCPR1L = (duty_cycle >> 2);
    CCP1CONbits.DC1B = (duty_cycle & 0x03);
}


// --- Implementação da Interrupção Externa (usando INT0 no pino RB0) ---

/**
 * @brief Inicializa a interrupção externa INT0 no pino RB0.
 * * Configura a interrupção para ocorrer na borda de subida.
 */
void external_interrupt_init(void)
{
    TRISBbits.RB0 = 1;          // Configura o pino RB0 (INT0) como entrada
    
    // Configura os registradores de interrupção
    INTCONbits.INT0IE = 1;      // Habilita a interrupção externa INT0
    INTCONbits.INT0IF = 0;      // Limpa a flag de interrupção INT0
    INTCON2bits.INTEDG0 = 1;    // Configura a interrupção para a borda de subida
    
    // Habilita interrupções de alta prioridade (se estiver usando prioridades)
    RCONbits.IPEN = 1;          // Habilita níveis de prioridade de interrupção
    INTCONbits.GIEH = 1;        // Habilita interrupções globais de alta prioridade
    INTCONbits.GIEL = 1;        // Habilita interrupções globais de baixa prioridade
}