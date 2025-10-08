#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"
#include "kernel.h"

#include <stdio.h>

// Declaração da fila de aptos
extern f_aptos_t readyQueue;


tcb_t *rr_scheduler()
{
    uint8_t pos_task_running = os_task_pos(readyQueue.taskRunning->task_func);
    uint8_t idle_selected = 0;
    
    do {
        pos_task_running = (pos_task_running+1) % readyQueue.readyQueueSize;
        if (readyQueue.readyQueue[pos_task_running].task_func == os_idle_task) {
            idle_selected++;
            if (idle_selected > 1) return &readyQueue.readyQueue[0];            
        }
        
    } while (readyQueue.readyQueue[pos_task_running].task_state != READY ||
             readyQueue.readyQueue[pos_task_running].task_func == os_idle_task);
    
    return &readyQueue.readyQueue[pos_task_running];
}

tcb_t *priority_scheduler()
{
    tcb_t *next_task = &readyQueue.readyQueue[0]; // Começa com a idle task como padrão
    uint8_t highest_priority = 0; // A idle task tem a menor prioridade
    
    // Varre a fila de aptos para encontrar a tarefa de maior prioridade
    for (uint8_t i = 1; i < readyQueue.readyQueueSize; i++) {
        // Verifica se a tarefa está pronta para executar
        if (readyQueue.readyQueue[i].task_state == READY) {
            // Se a prioridade da tarefa atual for maior que a maior encontrada até agora
            if (readyQueue.readyQueue[i].task_priority > highest_priority) {
                highest_priority = readyQueue.readyQueue[i].task_priority;
                next_task = &readyQueue.readyQueue[i];
            }
        }
    }
    
    return next_task;
}

void scheduler()
{
#if DEFAULT_SCHEDULER == RR_SCHEDULER
    readyQueue.taskRunning = rr_scheduler();
#elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
    readyQueue.taskRunning = priority_scheduler();
#endif    
}