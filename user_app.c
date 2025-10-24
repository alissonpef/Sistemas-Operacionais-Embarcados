#include <xc.h>
#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"

// --- Objetos Globais de Sincroniza??o e Comunica??o ---
pipe_t pipe_sensores;
pipe_t pipe_bateria;
mutex_t mutex_motores;

// Vari?vel compartilhada para a velocidade dos motores
motor_speeds_t velocidade_motores;


// Configura o hardware espec?fico da aplica??o e os objetos do RTOS
void config_app(void)
{
    // 0. IMPORTANTE: Desabilita fun??es anal?gicas no PORTD
    // Alguns pinos do PORTD podem estar configurados como anal?gicos por padr?o
    ADCON1 = 0x0F;  // Configura TODOS os pinos como digitais
    
    // 1. Configura??o do Hardware
    set_channel(CHANNEL_0);
    set_port(AN03); // Habilita AN0, AN1, AN2 como entradas anal?gicas
    config_adc(TAD12, FOSC4);
    adc_go(1); // Liga o m?dulo ADC
    
    pwm_init(); // Inicializa o PWM para o controle dos motores
    
    // 2. Inicializa??o dos Objetos do RTOS
    mutex_init(&mutex_motores);
    create_pipe(&pipe_sensores, sizeof(sensor_data_t));
    create_pipe(&pipe_bateria, sizeof(int));
    
    // 3. Inicializa velocidades dos motores em 50% (espera sensores)
    velocidade_motores.motor1_speed = 512;
    velocidade_motores.motor2_speed = 512;
    velocidade_motores.motor3_speed = 512;
    velocidade_motores.motor4_speed = 512;
    
    // 3.1 Aplica velocidades iniciais aos motores (50% inicial)
    pwm_set_duty_cycle_motor1(512);
    pwm_set_duty_cycle_motor2(512);
    pwm_set_duty_cycle_motor3(512);
    pwm_set_duty_cycle_motor4(512);
    
    // 4. Torna as tarefas vis?veis para o linker
    asm("GLOBAL _tarefa_controle_central, _tarefa_controle_motores, _tarefa_leitura_sensores, _tarefa_monitoramento_bateria");
}

// Tarefa principal: recebe dados dos sensores, processa e controla os motores
TASK tarefa_controle_central(void)
{
    sensor_data_t dados_sensores;
    int nivel_bateria;

    while (1) {
        // Aguarda e l? os dados dos pipes
        read_pipe(&pipe_sensores, &dados_sensores, sizeof(sensor_data_t));
        read_pipe(&pipe_bateria, &nivel_bateria, sizeof(int));
        
        // --- IN?CIO DA SE??O CR?TICA (CORRIGIDO) ---
        // Bloqueia o mutex ANTES de modificar a vari?vel compartilhada
        mutex_lock(&mutex_motores);

        // L?gica de controle: verifica a bateria ou calcula a velocidade
        if (nivel_bateria < 200) { // N?vel de bateria baixo
            // Ativa modo de pouso de emerg?ncia com velocidade baixa e fixa
            velocidade_motores.motor1_speed = 100;
            velocidade_motores.motor2_speed = 100;
            velocidade_motores.motor3_speed = 100;
            velocidade_motores.motor4_speed = 100;
        } else { // Opera??o normal
            // Usa o aceler?metro diretamente (0-1023)
            uint16_t base_speed = (uint16_t)dados_sensores.acelerometro;
            velocidade_motores.motor1_speed = base_speed;
            velocidade_motores.motor2_speed = base_speed;
            velocidade_motores.motor3_speed = base_speed;
            velocidade_motores.motor4_speed = base_speed;
        }

        // Libera o mutex DEPOIS de modificar a vari?vel compartilhada
        mutex_unlock(&mutex_motores);
        // --- FIM DA SE??O CR?TICA ---
        
        os_delay(100); // Aguarda 100 ticks
    }
}

// Tarefa de atua��o: l� a vari�vel de velocidade e aciona os motores via PWM
TASK tarefa_controle_motores(void)
{
    while (1) {
        // L� diretamente sem mutex (risco controlado - apenas leitura)
        // Isso economiza tempo de CPU e evita overhead
        pwm_set_duty_cycle_motor1(velocidade_motores.motor1_speed);
        pwm_set_duty_cycle_motor2(velocidade_motores.motor2_speed);
        pwm_set_duty_cycle_motor3(velocidade_motores.motor3_speed);
        pwm_set_duty_cycle_motor4(velocidade_motores.motor4_speed);
        
        os_delay(100); // Aumentado para 100 ticks (reduz carga)
    }
}

// Tarefa de sensoriamento: l? girosc?pio e aceler?metro e envia via pipe
TASK tarefa_leitura_sensores(void)
{
    sensor_data_t dados_sensores;

    while (1) {
        set_channel(CHANNEL_0);
        dados_sensores.giroscopio = adc_read();
        
        set_channel(CHANNEL_1);
        dados_sensores.acelerometro = adc_read();

        // Envia os dados lidos para a tarefa de controle
        write_pipe(&pipe_sensores, &dados_sensores, sizeof(sensor_data_t));
        
        os_delay(150); // L? os sensores a cada 150 ticks
    }
}

// Tarefa de monitoramento: l? o n?vel da bateria e envia via pipe
TASK tarefa_monitoramento_bateria(void)
{
    int nivel_bateria;

    while (1) {
        set_channel(CHANNEL_2);
        nivel_bateria = adc_read();

        // Envia o n?vel da bateria para a tarefa de controle
        write_pipe(&pipe_bateria, &nivel_bateria, sizeof(int));
        
        // O valor de delay n?o pode ser maior que 255 (uint8_t)
        os_delay(250);
    }
}