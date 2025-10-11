#include "sync.h"
#include "kernel.h"
#include "scheduler.h"
#include <xc.h>

// --- Funções de Semáforo ---

// Inicializa um semáforo com um valor inicial
void sem_init(sem_t *s, uint8_t init_value)
{
    s->contador         = init_value;
    s->sem_queue_in     = 0;
    s->sem_queue_out    = 0;    
}

// Tenta decrementar o semáforo (operação P); bloqueia se o contador for negativo
void sem_wait(sem_t *s)
{
    di(); // Inicia seção crítica
    
    s->contador--;
    
    if (s->contador < 0) {
        // Recurso indisponível, bloqueia a tarefa atual
        s->sem_queue[s->sem_queue_in] = readyQueue.taskRunning;
        s->sem_queue_in = (s->sem_queue_in + 1) % MAX_TASKS_ON_READY_QUEUE;
        
        // Salva o contexto e chama o escalonador
        SAVE_CONTEXT(WAITING_SEM);
        scheduler();
        RESTORE_CONTEXT();
    }
    
    ei(); // Finaliza seção crítica
}

// Incrementa o semáforo (operação V); libera uma tarefa bloqueada se houver
void sem_post(sem_t *s)
{
    di(); // Inicia seção crítica
    
    s->contador++;
    
    // Se havia tarefas esperando, acorda a primeira da fila
    if (s->contador <= 0) {
        s->sem_queue[s->sem_queue_out]->task_state = READY;
        s->sem_queue_out = (s->sem_queue_out + 1) % MAX_TASKS_ON_READY_QUEUE;
    }
    
    ei(); // Finaliza seção crítica
}

// --- Funções de Mutex ---

// Inicializa um mutex (semáforo binário com valor 1)
void mutex_init(mutex_t *m)
{
    sem_init(m, 1); // 1 = recurso está livre
}

// Bloqueia (trava) o mutex; aguarda se já estiver bloqueado
void mutex_lock(mutex_t *m)
{
    sem_wait(m);
}

// Desbloqueia (libera) o mutex
void mutex_unlock(mutex_t *m)
{
    sem_post(m);
}