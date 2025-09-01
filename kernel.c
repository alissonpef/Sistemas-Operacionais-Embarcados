#include "types.h"
#include "syscall.h"
#include "kernel.h"
#include <xc.h>

// Declara��o da fila de aptos
f_aptos_t readyQueue;

void os_config(void)
{
    readyQueue.readyQueueSize   = 0;
    readyQueue.taskRunning      = &readyQueue.readyQueue[0];
    
    // Criar a tarefa idle
    os_create_task(0, os_idle_task, 1);
}

void os_start(void)
{
    // Configurar o timer
    
    // Configura��es de usu�rio
}


void os_idle_task(void)
{
    while (1) {
        Nop();
    }
}
