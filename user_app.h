#ifndef USER_APP_H
#define	USER_APP_H

#include "types.h"
#include "syscall.h"
#include "kernel.h"
#include "hardware.h"
#include "user_app.h" // Adicionado
#include "mem.h"
#include <xc.h>

// Estrutura para os dados dos sensores
typedef struct {
    int giroscopio;
    int acelerometro;
} sensor_data_t;

// Estrutura para a velocidade dos motores (variável compartilhada)
typedef struct {
    uint16_t motor1_speed;
    uint16_t motor2_speed;
    uint16_t motor3_speed;
    uint16_t motor4_speed;
} motor_speeds_t;


// Função de configuração da aplicação
void config_app(void);

// Declaração das tarefas do drone
TASK tarefa_controle_central(void);
TASK tarefa_controle_motores(void);
TASK tarefa_leitura_sensores(void);
TASK tarefa_monitoramento_bateria(void);


#endif	/* USER_APP_H */