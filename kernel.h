#ifndef KERNEL_H
#define	KERNEL_H

#include <xc.h>
#include <stdint.h>
#include "types.h"

// Declaração da fila de aptos (visível para outros arquivos)
extern f_aptos_t readyQueue;

void os_config(void);
void os_start(void);
void os_idle_task(void);
uint8_t os_task_pos(f_ptr task);
void os_task_time_decrease(void);

// Macro para salvar o contexto da CPU na TCB da tarefa atual
#define SAVE_CONTEXT(new_state) \
do { \
    if (readyQueue.taskRunning->task_state == RUNNING) { \
        /* Salva os registradores principais */ \
        readyQueue.taskRunning->BSR_reg     = BSR; \
        readyQueue.taskRunning->STATUS_reg  = STATUS; \
        readyQueue.taskRunning->WORK_reg    = WREG; \
        /* Salva a pilha de hardware (stack) */ \
        readyQueue.taskRunning->task_sp     = 0; \
        while (STKPTR) { \
            readyQueue.taskRunning->STACK[readyQueue.taskRunning->task_sp] = TOS; \
            readyQueue.taskRunning->task_sp++; \
            asm("POP"); \
        } \
        readyQueue.taskRunning->task_state  = new_state; \
    } \
} while (0);


// Macro para restaurar o contexto da CPU a partir da TCB da próxima tarefa
#define RESTORE_CONTEXT() \
do { \
    if (readyQueue.taskRunning->task_state == READY) { \
        /* Restaura os registradores principais */ \
        BSR     = readyQueue.taskRunning->BSR_reg; \
        STATUS  = readyQueue.taskRunning->STATUS_reg; \
        WREG    = readyQueue.taskRunning->WORK_reg; \
        /* Restaura a pilha de hardware (stack) */ \
        STKPTR = 0; \
        /* Se for a primeira execução da tarefa, empilha o endereço da função */ \
        if (readyQueue.taskRunning->task_sp == 0) { \
            asm("PUSH"); \
            TOS = (uint24_t)readyQueue.taskRunning->task_func; \
        } \
        /* Se a tarefa já executou antes, restaura a pilha salva */ \
        else { \
            do { \
                asm("PUSH"); \
                readyQueue.taskRunning->task_sp--; \
                TOS = readyQueue.taskRunning->STACK[readyQueue.taskRunning->task_sp]; \
            } while (readyQueue.taskRunning->task_sp != 0); \
        } \
        readyQueue.taskRunning->task_state  = RUNNING; \
    } \
} while (0);

#endif	/* KERNEL_H */