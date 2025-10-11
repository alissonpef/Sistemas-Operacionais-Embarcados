#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "scheduler.h"

// Cria uma nova tarefa e a adiciona na fila de aptos
void os_create_task(uint8_t id, f_ptr task_f, uint8_t prior)
{
    tcb_t new_task;
    
    // Preenche a estrutura TCB da nova tarefa
    new_task.task_id            = id;
    new_task.task_func          = task_f;
    new_task.task_priority      = prior;
    new_task.task_state         = READY;
    
    // Zera o contexto de hardware inicial
    new_task.BSR_reg            = 0;
    new_task.STATUS_reg         = 0;
    new_task.WORK_reg           = 0;
    new_task.task_sp            = 0;
    
    // Insere a tarefa na fila de aptos
    readyQueue.readyQueue[readyQueue.readyQueueSize++] = new_task;    
}

// Bloqueia a tarefa atual por um número de ticks do sistema
void os_delay(uint8_t time)
{
    di();

    readyQueue.taskRunning->task_time_to_waiting = time;
    SAVE_CONTEXT(WAITING); // Muda estado para WAITING e salva o contexto
    
    scheduler(); // Chama o escalonador
    
    RESTORE_CONTEXT(); 
    
    ei();    
}

// Força a tarefa atual a ceder o processador voluntariamente
void os_yield()
{
    di();
    
    SAVE_CONTEXT(READY); // Mantém o estado como READY e salva o contexto
    
    scheduler(); // Chama o escalonador
    
    RESTORE_CONTEXT();    
    
    ei();
}

// (Função interna) Muda o estado da tarefa atual e força a troca de contexto
void os_change_state(state_t new_state)
{
    di();

    SAVE_CONTEXT(new_state); // Muda para o novo estado e salva o contexto
    
    scheduler(); // Chama o escalonador
    
    RESTORE_CONTEXT();    
    
    ei();
}