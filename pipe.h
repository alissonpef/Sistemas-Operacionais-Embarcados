#ifndef PIPE_H
#define	PIPE_H

#include "types.h"

// Cria um pipe com um buffer de tamanho 'size'
void create_pipe(pipe_t *p, uint8_t size);

// Lê 'size' bytes do pipe para o 'buffer'
void read_pipe(pipe_t *p, void *buffer, uint8_t size);

// Escreve 'size' bytes do 'buffer' para o pipe
void write_pipe(pipe_t *p, void *buffer, uint8_t size);

#endif	/* PIPE_H */