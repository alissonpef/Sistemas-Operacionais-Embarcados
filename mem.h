#ifndef MEM_H
#define	MEM_H

#include "types.h"

// Aloca um bloco de memória de 'nBytes'
unsigned char * SRAMalloc(unsigned char nBytes);

// Libera um bloco de memória previamente alocado
void SRAMfree(unsigned char *pSRAM);

// Inicializa o heap de memória para uso
void SRAMInitHeap(void);

// (Função interna) Tenta unir dois blocos de memória livres
unsigned char _SRAMmerge(SALLOC * pSegA);


#endif	/* MEM_H */