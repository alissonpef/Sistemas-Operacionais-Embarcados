#ifndef KERNEL_H
#define	KERNEL_H

#include <xc.h>
#include <stdint.h>
#include "types.h"

// Declaração da fila de aptos
extern f_aptos_t readyQueue;

void os_config(void);
void os_start(void);
void os_idle_task(void);
uint8_t os_task_pos(f_ptr task);
void os_task_time_decrease(void);

// Salvar e restaurar o contexto


#define SAVE_CONTEXT(new_state) \
do { \
    if (readyQueue.taskRunning->task_state == RUNNING) { \
        readyQueue.taskRunning->BSR_reg     = BSR; \
        readyQueue.taskRunning->STATUS_reg  = STATUS; \
        readyQueue.taskRunning->WORK_reg    = WREG; \
        /* Piha */ \
        readyQueue.taskRunning->task_sp     = 0; \
        while (STKPTR) { \
            readyQueue.taskRunning->STACK[readyQueue.taskRunning->task_sp] = TOS; \
            readyQueue.taskRunning->task_sp++; \
            asm("POP"); \
        } \
        readyQueue.taskRunning->task_state  = new_state; \
    } \
} while (0);\


#define RESTORE_CONTEXT() \
do { \
    if (readyQueue.taskRunning->task_state == READY) { \
        BSR     = readyQueue.taskRunning->BSR_reg; \
        STATUS  = readyQueue.taskRunning->STATUS_reg; \
        WREG    = readyQueue.taskRunning->WORK_reg; \
        /* Piha */ \
        STKPTR = 0; \
        /* Primeira execução */ \
        if (readyQueue.taskRunning->task_sp == 0) { \
            asm("PUSH"); \
            TOS = (uint24_t)readyQueue.taskRunning->task_func; \
        } \
        else { /* Já executou alguma vez */ \
            do { \
                asm("PUSH"); \
                readyQueue.taskRunning->task_sp--; \
                TOS = readyQueue.taskRunning->STACK[readyQueue.taskRunning->task_sp]; \
            } while (readyQueue.taskRunning->task_sp != 0); \
        } \
        readyQueue.taskRunning->task_state  = RUNNING; \
    } \
} while (0);\

#endif	/* KERNEL_H */

