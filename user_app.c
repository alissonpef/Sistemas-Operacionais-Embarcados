#include <xc.h>

#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"

// --- Objetos de Sincroniza��o e Comunica��o ---

pipe_t pipe_sensores;
pipe_t pipe_bateria;

motor_speeds_t velocidade_motores;
mutex_t mutex_motores;


void config_app(void)
{
    // 1. Configuração do Hardware
    // Configura os pinos do ADC para ler os sensores (potenciômetros)
    set_channel(CHANNEL_0);
    set_port(AN03); // Habilita AN0, AN1, AN2
    config_adc(TAD12, FOSC4);
    adc_go(1); // Liga o módulo ADC
    
    // Inicializa o PWM para os motores
    pwm_init();
    
    // 2. Inicialização dos Objetos do RTOS
    mutex_init(&mutex_motores);
    create_pipe(&pipe_sensores, sizeof(sensor_data_t));
    create_pipe(&pipe_bateria, sizeof(int));
    
    // 3. Torna as tarefas visíveis para o linker
    asm("GLOBAL _tarefa_controle_central, _tarefa_controle_motores, _tarefa_leitura_sensores, _tarefa_monitoramento_bateria");
}

/**
 * @brief Tarefa principal, com a maior prioridade. Recebe dados dos sensores,
 * processa e envia comandos para os motores.
 */
TASK tarefa_controle_central(void)
{
    sensor_data_t dados_sensores;
    int nivel_bateria;

    while (1) {
        // Chamada de read_pipe corrigida
        read_pipe(&pipe_sensores, &dados_sensores, sizeof(sensor_data_t));
        read_pipe(&pipe_bateria, &nivel_bateria, sizeof(int));

        if (nivel_bateria < 200) {
            velocidade_motores.motor1_speed = 50;
            velocidade_motores.motor2_speed = 50;
            velocidade_motores.motor3_speed = 50;
            velocidade_motores.motor4_speed = 50;
        } else {
            // Adicionado um type cast (uint16_t) para remover o aviso
            uint16_t base_speed = (uint16_t)dados_sensores.acelerometro / 4;
            velocidade_motores.motor1_speed = base_speed;
            velocidade_motores.motor2_speed = base_speed;
            velocidade_motores.motor3_speed = base_speed;
            velocidade_motores.motor4_speed = base_speed;
        }

        mutex_lock(&mutex_motores);
        mutex_unlock(&mutex_motores);
        
        os_delay(100);
    }
}

/**
 * @brief Tarefa responsável por ler a variável compartilhada e
 * acionar os motores via PWM.
 */
TASK tarefa_controle_motores(void)
{
    motor_speeds_t velocidades_locais;

    while (1) {
        // Bloqueia o mutex para ler a variável compartilhada de forma segura
        mutex_lock(&mutex_motores);
        velocidades_locais = velocidade_motores;
        mutex_unlock(&mutex_motores);

        // Aplica as velocidades nos motores (simulado com PWM e LEDs)
        // A função pwm_set_duty_cycle espera um valor de 10 bits
        pwm_set_duty_cycle(velocidades_locais.motor1_speed); // Usando apenas um PWM para simular
        // Aqui iriam os comandos para os outros 3 motores...
        
        os_delay(50); // Atualiza os motores a cada 50 ticks
    }
}

/**
 * @brief Tarefa que lê os sensores (simulados por potenciômetros) e envia
 * os dados para a tarefa de controle central via pipe.
 */
TASK tarefa_leitura_sensores(void)
{
    sensor_data_t dados_sensores;

    while (1) {
        set_channel(CHANNEL_0);
        dados_sensores.giroscopio = adc_read();
        
        set_channel(CHANNEL_1);
        dados_sensores.acelerometro = adc_read();

        // Chamada de write_pipe corrigida
        write_pipe(&pipe_sensores, &dados_sensores, sizeof(sensor_data_t));
        
        os_delay(150);
    }
}

TASK tarefa_monitoramento_bateria(void)
{
    int nivel_bateria;

    while (1) {
        set_channel(CHANNEL_2);
        nivel_bateria = adc_read();

        // Chamada de write_pipe corrigida
        write_pipe(&pipe_bateria, &nivel_bateria, sizeof(int));
        
        // CORRIGIDO: O valor de delay n�o pode ser maior que 255
        // O valor 500 estava causando um overflow (virando 244).
        os_delay(250); // Valor m�ximo � 255
    }
}