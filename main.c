#include <xc.h>
#include "syscall.h"
#include "kernel.h"
#include "user_app.h"


int main()
{
    // Inicializa o sistema operacional
    os_config();
    
    // --- Cria as tarefas do drone com suas prioridades ---
    
    // Prioridade 5 (Mais alta): Tarefa que toma as decisões críticas.
    os_create_task(100, tarefa_controle_central, 5);
    
    // Prioridade 4: Tarefa que controla os atuadores (motores).
    os_create_task(101, tarefa_controle_motores, 4);
    
    // Prioridade 3: Tarefa que lê os sensores para o controle.
    os_create_task(102, tarefa_leitura_sensores, 3);
    
    // Prioridade 2 (Mais baixa): Tarefa de monitoramento, menos crítica.
    os_create_task(103, tarefa_monitoramento_bateria, 2);
    
    // A tarefa ociosa (os_idle_task) tem prioridade 1 (a mais baixa de todas)
    
    // Inicia o escalonador do RTOS
    os_start();
    
    // O programa nunca deve chegar aqui
    while (1);
    
    return 0;
}