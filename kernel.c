#include "types.h"
#include "syscall.h"
#include "kernel.h"
#include "hardware.h"
#include "user_app.h"
#include "mem.h"
#include <xc.h>

// Declaração da fila de aptos
f_aptos_t readyQueue;

void os_config(void)
{
    readyQueue.readyQueueSize   = 0;
    readyQueue.taskRunning      = 0;
    
    // Criar a tarefa idle
    os_create_task(0, os_idle_task, 1);
    
    asm("GLOBAL _os_idle_task");
}

void os_start(void)
{
    // Configurar o timer
    conf_interrupts();
    
    // Configuração de memória
    #if DYNAMIC_MEM_ALLOC == YES
    SRAMInitHeap();
    #endif    

    // Configurações de usuário
    config_app();
    
    conf_timer_0();
   
    // Habilita interrupções globais
    ei();
}


void os_idle_task(void)
{
    TRISDbits.RD3 = 0;
    
    while (1) {
        // Nop();
        LATDbits.LD3 = ~PORTDbits.RD3;
    }
}

uint8_t os_task_pos(f_ptr task)
{
    for (uint8_t i = 0; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_func == task) return i;
    }
    
    return 0;
}

void os_task_time_decrease()
{
    for (uint8_t i = 1; i < readyQueue.readyQueueSize; i++) {
        if (readyQueue.readyQueue[i].task_state == WAITING) {
            readyQueue.readyQueue[i].task_time_to_waiting--;
            if (readyQueue.readyQueue[i].task_time_to_waiting == 0) {
                // Restaura o estado de pronto (ready)
                readyQueue.readyQueue[i].task_state = READY;
            }
        }
    }
}
