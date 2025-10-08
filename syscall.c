#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "scheduler.h"

// Declaração da fila de aptos
extern f_aptos_t readyQueue;

void os_create_task(uint8_t id, f_ptr task_f, uint8_t prior)
{
    tcb_t new_task;
    
    new_task.task_id            = id;
    new_task.task_func          = task_f;
    new_task.task_priority      = prior;
    new_task.task_state         = READY;
    
    new_task.BSR_reg            = 0;
    new_task.STATUS_reg         = 0;
    new_task.WORK_reg           = 0;
    new_task.task_sp            = 0;
    
    // Inserir tarefa na fila de aptos
    readyQueue.readyQueue[readyQueue.readyQueueSize++] = new_task;    
}

void os_delay(uint8_t time)
{
    di();

    readyQueue.taskRunning->task_time_to_waiting = time;
    SAVE_CONTEXT(WAITING);
    // Escalonador
    scheduler();
    RESTORE_CONTEXT(); 
    
    ei();    
}

void os_yield()
{
    di();
    
    SAVE_CONTEXT(READY);
    // Escalonador
    scheduler();
    RESTORE_CONTEXT();    
    
    ei();
}

void os_change_state(state_t new_state)
{
    di();

    SAVE_CONTEXT(new_state);
    // Escalonador
    scheduler();
    RESTORE_CONTEXT();    
    
    ei();
}
