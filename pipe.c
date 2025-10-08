#include "pipe.h"
#include "sync.h"
#include "mem.h" // Incluir a biblioteca de gerenciamento de memória
#include <xc.h>

void create_pipe(pipe_t *p, uint8_t size)
{
    p->pipe_pos_read    = 0;
    p->pipe_pos_write   = 0;
    p->pipe_size        = size;
    p->pipe_data        = (char*)SRAMalloc(size); // Aloca memória dinamicamente
    
    // Inicializa os semáforos de controle do pipe
    sem_init(&p->pipe_sem_read, 0);
    sem_init(&p->pipe_sem_write, size); // Usa o tamanho dinâmico
}

void read_pipe(pipe_t *p, char *buffer)
{
    di();
    
    // Testa o semáforo de leitura
    sem_wait(&p->pipe_sem_read);
    
    *buffer = p->pipe_data[p->pipe_pos_read];
    p->pipe_pos_read = (p->pipe_pos_read + 1) % p->pipe_size; // Usa o tamanho dinâmico
    
    // Libera o semáforo da escrita
    sem_post(&p->pipe_sem_write);
    
    ei();
}

void write_pipe(pipe_t *p, char buffer)
{
    di();
    
    // Testa o semáforo de escrita
    sem_wait(&p->pipe_sem_write);
    
    p->pipe_data[p->pipe_pos_write] = buffer;
    p->pipe_pos_write = (p->pipe_pos_write + 1) % p->pipe_size; // Usa o tamanho dinâmico
    
    // Libera o semáforo da leitura
    sem_post(&p->pipe_sem_read);
    
    ei();    
}