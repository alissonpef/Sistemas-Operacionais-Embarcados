#ifndef OS_CONFIG_H
#define	OS_CONFIG_H

// --- Definições Gerais ---
#define YES                         1
#define NO                          0

// --- Configurações do Kernel ---

// Número máximo de tarefas que o sistema pode criar (incluindo a idle)
#define MAX_TASKS_ON_READY_QUEUE    5

// Define o algoritmo de escalonamento padrão (PRIORITY_SCHEDULER ou RR_SCHEDULER)
#define DEFAULT_SCHEDULER           PRIORITY_SCHEDULER

// --- Configurações de Módulos ---

// Tamanho padrão para pipes (pode ser obsoleto com alocação dinâmica)
#define PIPE_MAX_SIZE               3

// Habilita (YES) ou desabilita (NO) a alocação dinâmica de memória
#define DYNAMIC_MEM_ALLOC           YES


#endif	/* OS_CONFIG_H */