#ifndef SYSCALL_H
#define	SYSCALL_H

#include <stdint.h>
#include "types.h"

// Cria uma nova tarefa
void os_create_task(uint8_t id, f_ptr task_f, uint8_t prior);

// Pausa a tarefa atual por 'time' ticks
void os_delay(uint8_t time);

// Libera a CPU para a próxima tarefa
void os_yield();

// Muda o estado da tarefa atual e reescalona
void os_change_state(state_t new_state);


#endif	/* SYSCALL_H */```