#include "scheduler.h"
#include "os_config.h"
#include "syscall.h"
#include "kernel.h"

// Escalonador Round-Robin: seleciona a próxima tarefa pronta na fila
tcb_t *rr_scheduler()
{
    uint8_t pos_task_running = os_task_pos(readyQueue.taskRunning->task_func);
    uint8_t idle_selected = 0;
    
    do {
        pos_task_running = (pos_task_running + 1) % readyQueue.readyQueueSize;
        
        // Garante que a tarefa idle só seja escolhida se não houver outra opção
        if (readyQueue.readyQueue[pos_task_running].task_func == os_idle_task) {
            idle_selected++;
            if (idle_selected > 1) return &readyQueue.readyQueue[0];            
        }
        
    } while (readyQueue.readyQueue[pos_task_running].task_state != READY ||
             readyQueue.readyQueue[pos_task_running].task_func == os_idle_task);
    
    return &readyQueue.readyQueue[pos_task_running];
}

// Escalonador por Prioridade: seleciona a tarefa pronta de maior prioridade
tcb_t *priority_scheduler()
{
    // Inicia com a idle task como candidata padrão
    tcb_t *next_task = &readyQueue.readyQueue[0];
    uint8_t highest_priority = 0; // Prioridade da idle task
    
    // Procura na fila de aptos pela tarefa de maior prioridade que está pronta
    for (uint8_t i = 1; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_state == READY) {
            if (readyQueue.readyQueue[i].task_priority > highest_priority) {
                highest_priority = readyQueue.readyQueue[i].task_priority;
                next_task = &readyQueue.readyQueue[i];
            }
        }
    }
    
    return next_task;
}

// Função principal do escalonador, chamada pelo tick do sistema
void scheduler()
{
    // Seleciona o algoritmo de escalonamento definido em os_config.h
    #if DEFAULT_SCHEDULER == RR_SCHEDULER
        readyQueue.taskRunning = rr_scheduler();
    #elif DEFAULT_SCHEDULER == PRIORITY_SCHEDULER
        readyQueue.taskRunning = priority_scheduler();
    #endif    
}