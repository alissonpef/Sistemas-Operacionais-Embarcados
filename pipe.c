#include "pipe.h"
#include "sync.h"
#include "mem.h"
#include <xc.h>
#include <stdint.h>

void create_pipe(pipe_t *p, uint8_t size)
{
    p->pipe_pos_read    = 0;
    p->pipe_pos_write   = 0;
    p->pipe_size        = size;
    p->pipe_data        = (char*)SRAMalloc(size);
    
    // Inicializa os semáforos de controle do pipe
    sem_init(&p->pipe_sem_read, 0);
    sem_init(&p->pipe_sem_write, size);
}

// Implementação corrigida para ler múltiplos bytes
void read_pipe(pipe_t *p, void *buffer, uint8_t size)
{
    char *data_buffer = (char*)buffer;
    for (uint8_t i = 0; i < size; i++) {
        di();
        sem_wait(&p->pipe_sem_read);
        data_buffer[i] = p->pipe_data[p->pipe_pos_read];
        p->pipe_pos_read = (p->pipe_pos_read + 1) % p->pipe_size;
        sem_post(&p->pipe_sem_write);
        ei();
    }
}

// Implementação corrigida para escrever múltiplos bytes
void write_pipe(pipe_t *p, void *buffer, uint8_t size)
{
    char *data_buffer = (char*)buffer;
    for (uint8_t i = 0; i < size; i++) {
        di();
        sem_wait(&p->pipe_sem_write);
        p->pipe_data[p->pipe_pos_write] = data_buffer[i];
        p->pipe_pos_write = (p->pipe_pos_write + 1) % p->pipe_size;
        sem_post(&p->pipe_sem_read);
        ei();
    }
}