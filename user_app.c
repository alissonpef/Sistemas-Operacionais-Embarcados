#include <xc.h>
#include "user_app.h"
#include "syscall.h"
#include "sync.h"
#include "pipe.h"
#include "mem.h"
#include "io.h"

// --- Objetos Globais de Sincronização e Comunicação ---
pipe_t pipe_sensores;
pipe_t pipe_bateria;
mutex_t mutex_motores;

// Variável compartilhada para a velocidade dos motores
motor_speeds_t velocidade_motores;


// Configura o hardware específico da aplicação e os objetos do RTOS
void config_app(void)
{
    // 1. Configuração do Hardware
    set_channel(CHANNEL_0);
    set_port(AN03); // Habilita AN0, AN1, AN2 como entradas analógicas
    config_adc(TAD12, FOSC4);
    adc_go(1); // Liga o módulo ADC
    
    pwm_init(); // Inicializa o PWM para o controle dos motores
    
    // 2. Inicialização dos Objetos do RTOS
    mutex_init(&mutex_motores);
    create_pipe(&pipe_sensores, sizeof(sensor_data_t));
    create_pipe(&pipe_bateria, sizeof(int));
    
    // 3. Torna as tarefas visíveis para o linker
    asm("GLOBAL _tarefa_controle_central, _tarefa_controle_motores, _tarefa_leitura_sensores, _tarefa_monitoramento_bateria");
}

// Tarefa principal: recebe dados dos sensores, processa e controla os motores
TASK tarefa_controle_central(void)
{
    sensor_data_t dados_sensores;
    int nivel_bateria;

    while (1) {
        // Aguarda e lê os dados dos pipes
        read_pipe(&pipe_sensores, &dados_sensores, sizeof(sensor_data_t));
        read_pipe(&pipe_bateria, &nivel_bateria, sizeof(int));
        
        // --- INÍCIO DA SEÇÃO CRÍTICA (CORRIGIDO) ---
        // Bloqueia o mutex ANTES de modificar a variável compartilhada
        mutex_lock(&mutex_motores);

        // Lógica de controle: verifica a bateria ou calcula a velocidade
        if (nivel_bateria < 200) { // Nível de bateria baixo
            // Ativa modo de pouso de emergência com velocidade baixa e fixa
            velocidade_motores.motor1_speed = 50;
            velocidade_motores.motor2_speed = 50;
            velocidade_motores.motor3_speed = 50;
            velocidade_motores.motor4_speed = 50;
        } else { // Operação normal
            // Calcula a velocidade base a partir do acelerômetro
            uint16_t base_speed = (uint16_t)dados_sensores.acelerometro / 4;
            velocidade_motores.motor1_speed = base_speed;
            velocidade_motores.motor2_speed = base_speed;
            velocidade_motores.motor3_speed = base_speed;
            velocidade_motores.motor4_speed = base_speed;
        }

        // Libera o mutex DEPOIS de modificar a variável compartilhada
        mutex_unlock(&mutex_motores);
        // --- FIM DA SEÇÃO CRÍTICA ---
        
        os_delay(100); // Aguarda 100 ticks
    }
}

// Tarefa de atuação: lê a variável de velocidade e aciona os motores via PWM
TASK tarefa_controle_motores(void)
{
    motor_speeds_t velocidades_locais;

    while (1) {
        // Bloqueia o mutex para ler a variável compartilhada de forma segura
        mutex_lock(&mutex_motores);
        velocidades_locais = velocidade_motores;
        mutex_unlock(&mutex_motores);

        // Aplica as velocidades nos motores
        pwm_set_duty_cycle(velocidades_locais.motor1_speed); // Simula o motor 1
        // Aqui iriam os comandos para os outros 3 motores...
        
        os_delay(50); // Atualiza os motores a cada 50 ticks
    }
}

// Tarefa de sensoriamento: lê giroscópio e acelerômetro e envia via pipe
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
        
        os_delay(150); // Lê os sensores a cada 150 ticks
    }
}

// Tarefa de monitoramento: lê o nível da bateria e envia via pipe
TASK tarefa_monitoramento_bateria(void)
{
    int nivel_bateria;

    while (1) {
        set_channel(CHANNEL_2);
        nivel_bateria = adc_read();

        // Envia o nível da bateria para a tarefa de controle
        write_pipe(&pipe_bateria, &nivel_bateria, sizeof(int));
        
        // O valor de delay não pode ser maior que 255 (uint8_t)
        os_delay(250);
    }
}