#ifndef PIPE_H
#define	PIPE_H

#include "types.h"

void create_pipe(pipe_t *p, uint8_t size);
// Modificadas para trabalhar com blocos de dados
void read_pipe(pipe_t *p, void *buffer, uint8_t size);
void write_pipe(pipe_t *p, void *buffer, uint8_t size);

#endif	/* PIPE_H */