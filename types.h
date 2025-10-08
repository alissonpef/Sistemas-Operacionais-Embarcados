#ifndef TYPES_H
#define	TYPES_H

#include <stdint.h>
#include "os_config.h"

#define RR_SCHEDULER            1
#define PRIORITY_SCHEDULER      2

// Define o tipo tarefa
typedef void TASK;

// Define o ponteiro para a função
typedef void (*f_ptr)(void);

// Define os estados possíveis
typedef enum {READY = 0, RUNNING, WAITING, WAITING_SEM} state_t;

// Define a TCB 
typedef struct tcb {
    uint8_t     task_id;
    f_ptr       task_func;
    state_t     task_state;
    uint8_t     task_priority;
    uint8_t     task_time_to_waiting;
    // Contexto de hardware
    uint8_t     BSR_reg;
    uint8_t     WORK_reg;
    uint8_t     STATUS_reg;
    uint24_t     STACK[32];
    uint8_t     task_sp;    
} tcb_t;

// Fila de aptos
typedef struct f_aptos {
    tcb_t readyQueue[MAX_TASKS_ON_READY_QUEUE];
    uint8_t readyQueueSize;
    tcb_t *taskRunning;
} f_aptos_t;

typedef struct semaphore {
    int contador;
    tcb_t *sem_queue[MAX_TASKS_ON_READY_QUEUE];
    uint8_t sem_queue_in;
    uint8_t sem_queue_out;
} sem_t;

// Define a estrutura do Mutex
typedef sem_t mutex_t;

typedef struct pipe {
    uint8_t pipe_pos_read;
    uint8_t pipe_pos_write;
    char* pipe_data; // Alterado de array para ponteiro
    uint8_t pipe_size; // Novo campo para armazenar o tamanho
    sem_t pipe_sem_read;
    sem_t pipe_sem_write;
} pipe_t;

/*********************************************************************
 * Segment header data type
 ********************************************************************/
typedef union _SALLOC
{
	unsigned char byte;
	struct _BITS
	{
		unsigned count:7;
		unsigned alloc:1;	
	} bits;
}SALLOC;


#endif	/* TYPES_H */