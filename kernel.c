#include "types.h"
#include "syscall.h"
#include "kernel.h"
#include "hardware.h"
#include "user_app.h"
#include "mem.h"
#include <xc.h>

// Declaração da fila de aptos
f_aptos_t readyQueue;

// Inicializa as variáveis do kernel e cria a tarefa idle
void os_config(void)
{
    readyQueue.readyQueueSize   = 0;
    readyQueue.taskRunning      = 0;
    
    // Cria a tarefa idle (ociosa) com a menor prioridade
    os_create_task(0, os_idle_task, 1);
    
    // Torna a idle task visível para o linker
    asm("GLOBAL _os_idle_task");
}

// Inicia a execução do sistema operacional
void os_start(void)
{
    // Habilita as interrupções de periféricos
    conf_interrupts();
    
    // Configuração de memória dinâmica
    #if DYNAMIC_MEM_ALLOC == YES
    SRAMInitHeap();
    #endif    

    // Chama as configurações da aplicação do usuário
    config_app();
    
    // Inicia o timer do sistema (tick)
    conf_timer_0();
   
    // Habilita interrupções globais para iniciar o escalonamento
    ei();
}

// Tarefa ociosa (idle): executada quando nenhuma outra tarefa está pronta
void os_idle_task(void)
{
    TRISDbits.RD3 = 0; // Configura RD3 como saída para o LED de status
    
    while (1) {
        // Pisca o LED para indicar que o sistema está funcionando
        LATDbits.LD3 = ~PORTDbits.RD3;
    }
}

// Retorna a posição de uma tarefa na fila de aptos
uint8_t os_task_pos(f_ptr task)
{
    for (uint8_t i = 0; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_func == task) return i;
    }
    
    return 0;
}

// Decrementa o tempo de espera das tarefas em estado WAITING (atraso)
void os_task_time_decrease()
{
    // Inicia em 1 para ignorar a tarefa idle
    for (uint8_t i = 1; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_state == WAITING) {
            readyQueue.readyQueue[i].task_time_to_waiting--;
            if (readyQueue.readyQueue[i].task_time_to_waiting == 0) {
                // Tempo de espera esgotado, a tarefa volta para o estado READY
                readyQueue.readyQueue[i].task_state = READY;
            }
        }
    }
}