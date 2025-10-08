#include <xc.h>

#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"

// --- Objetos de Sincronização e Comunicação ---

// Pipes para comunicação entre tarefas
pipe_t pipe_sensores;
pipe_t pipe_bateria;

// Variável compartilhada para velocidade dos motores e seu mutex de proteção
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
        // Espera por novos dados dos sensores
        read_pipe(&pipe_sensores, (char*)&dados_sensores);
        // Lê o nível da bateria
        read_pipe(&pipe_bateria, (char*)&nivel_bateria);

        // Lógica de controle (SIMULAÇÃO)
        // Se a bateria estiver baixa, planeja o pouso (ex: reduz a velocidade)
        if (nivel_bateria < 200) { // Valor de exemplo para bateria baixa
            velocidade_motores.motor1_speed = 50;
            velocidade_motores.motor2_speed = 50;
            velocidade_motores.motor3_speed = 50;
            velocidade_motores.motor4_speed = 50;
        } else {
            // "Calcula" a velocidade dos motores com base nos sensores
            // Ex: usa o acelerômetro para definir a velocidade base
            uint16_t base_speed = dados_sensores.acelerometro / 4; // Converte de 10-bit para 8-bit
            velocidade_motores.motor1_speed = base_speed;
            velocidade_motores.motor2_speed = base_speed;
            velocidade_motores.motor3_speed = base_speed;
            velocidade_motores.motor4_speed = base_speed;
        }

        // Bloqueia o mutex para escrever na variável compartilhada de forma segura
        mutex_lock(&mutex_motores);
        // A lógica acima já alterou a variável, aqui apenas simularia a escrita
        // (Em um caso real, a cópia para a variável global seria aqui dentro)
        mutex_unlock(&mutex_motores);
        
        os_delay(100); // Executa a lógica de controle a cada 100 ticks
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
        // Simula a leitura do giroscópio e acelerômetro lendo canais ADC
        set_channel(CHANNEL_0); // AN0 para giroscópio
        dados_sensores.giroscopio = adc_read();
        
        set_channel(CHANNEL_1); // AN1 para acelerômetro
        dados_sensores.acelerometro = adc_read();

        // Envia os dados lidos para o pipe de sensores
        write_pipe(&pipe_sensores, (char*)&dados_sensores);
        
        os_delay(150); // Lê os sensores a cada 150 ticks
    }
}

/**
 * @brief Tarefa de baixa prioridade que monitora o nível da bateria
 * (simulado por um potenciômetro).
 */
TASK tarefa_monitoramento_bateria(void)
{
    int nivel_bateria;

    while (1) {
        // Simula a leitura da bateria lendo um canal ADC
        set_channel(CHANNEL_2); // AN2 para bateria
        nivel_bateria = adc_read();

        // Envia o dado para o pipe da bateria
        write_pipe(&pipe_bateria, (char*)&nivel_bateria);
        
        os_delay(500); // Verifica a bateria a cada 500 ticks
    }
}