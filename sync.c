#include "sync.h"
#include "kernel.h"
#include "scheduler.h"
#include <xc.h>

// Declaração da fila de aptos
extern f_aptos_t readyQueue;


void sem_init(sem_t *s, uint8_t init_value)
{
    s->contador         = init_value;
    s->sem_queue_in     = 0;
    s->sem_queue_out    = 0;    
}

void sem_wait(sem_t *s)
{
    di();
    
    // Decrementa contador
    s->contador--;
    
    if (s->contador < 0) {
        // Bloqueia a tarefa
        s->sem_queue[s->sem_queue_in] = readyQueue.taskRunning;
        s->sem_queue_in = (s->sem_queue_in + 1) % MAX_TASKS_ON_READY_QUEUE;
        // Troca de contexto
        SAVE_CONTEXT(WAITING_SEM);
        scheduler();
        RESTORE_CONTEXT();
    }
    
    ei();    
}

void sem_post(sem_t *s)
{
    di();
    
    s->contador++;
    
    if (s->contador <= 0) {
        s->sem_queue[s->sem_queue_out]->task_state = READY;
        s->sem_queue_out = (s->sem_queue_out + 1) % MAX_TASKS_ON_READY_QUEUE;
    }
    
    ei();
}

// --- Implementação do Mutex ---

void mutex_init(mutex_t *m)
{
    // Mutex é um semáforo binário inicializado com 1 (recurso livre)
    sem_init(m, 1);
}

void mutex_lock(mutex_t *m)
{
    // Tenta "pegar" o recurso
    sem_wait(m);
}

void mutex_unlock(mutex_t *m)
{
    // Libera o recurso
    sem_post(m);
}