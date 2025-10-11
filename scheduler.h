#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "types.h"

// Algoritmo de escalonamento Round-Robin
tcb_t *rr_scheduler(void);

// Algoritmo de escalonamento por Prioridade
tcb_t *priority_scheduler(void);

// Função principal do escalonador
void scheduler(void);


#endif	/* SCHEDULER_H */