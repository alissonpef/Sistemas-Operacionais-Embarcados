#ifndef MEM_H
#define	MEM_H

#include "types.h"

#define NEAR

unsigned char * SRAMalloc(unsigned char nBytes);
void SRAMfree(unsigned char *pSRAM);
void SRAMInitHeap(void);
NEAR unsigned char _SRAMmerge(SALLOC * NEAR pSegA);


#endif	/* MEM_H */

