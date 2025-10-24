#ifndef IO_H
#define	IO_H

#include <xc.h>
#include <stdint.h>

// Tipos de dados para configura??o do ADC

typedef enum {CHANNEL_0     = 0b0000, 
              CHANNEL_1     = 0b0001,
              CHANNEL_2     = 0b0010,
              CHANNEL_3     = 0b0011,
              CHANNEL_4     = 0b0100,
              CHANNEL_5     = 0b0101,
              CHANNEL_6     = 0b0110,
              CHANNEL_7     = 0b0111,
              CHANNEL_8     = 0b1000,
              CHANNEL_9     = 0b1001,
              CHANNEL_10    = 0b1010,
              CHANNEL_11    = 0b1011,
              CHANNEL_12    = 0b1100 } channel_t;
              
typedef enum {AN12  = 0b0000,
              AN11  = 0b0011,
              AN10  = 0b0100,
              AN09   = 0b0101,
              AN08   = 0b0110,
              AN07   = 0b0111,
              AN06   = 0b1000,
              AN05   = 0b1001,
              AN04   = 0b1010,
              AN03   = 0b1011,
              AN02   = 0b1100,
              AN01   = 0b1101,
              AN00   = 0b1110,
              DISABLE  = 0b1111} port_conf_t;

typedef enum {TAD20 = 0b111,
              TAD16 = 0b110,
              TAD12 = 0b101,
              TAD8  = 0b100,
              TAD6  = 0b011,
              TAD4  = 0b010,
              TAD2  = 0b001,
              TAD0  = 0b000} tad_t;

typedef enum {FRC1      = 0b111,
              FOSC64    = 0b110,
              FOSC16    = 0b101,
              FOSC4     = 0b100,
              FRC2      = 0b011,
              FOSC32    = 0b010,
              FOSC8     = 0b001,
              FOSC2     = 0b000} conversion_clock_t;              

// --- Fun??es do ADC ---
void set_channel(channel_t channel);
void set_port(port_conf_t port);
void config_adc(tad_t tad, conversion_clock_t cclk);
void adc_go(int go_done);
int adc_read();

// --- Fun??es do PWM ---
void pwm_init(void);
void pwm_set_duty_cycle_motor1(uint16_t duty_cycle);
void pwm_set_duty_cycle_motor2(uint16_t duty_cycle);
void pwm_set_duty_cycle_motor3(uint16_t duty_cycle);
void pwm_set_duty_cycle_motor4(uint16_t duty_cycle);
void pwm_software_isr(void);  // ISR do PWM por software

// --- Fun??es de Interrup??o Externa (usando INT0 no pino RB0) ---
void external_interrupt_init(void);


#endif	/* IO_H */