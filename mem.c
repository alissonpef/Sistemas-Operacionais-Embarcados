#include "mem.h"

#define MAX_HEAP_SIZE       0x200 // Tamanho total do heap em bytes (512)
#define _MAX_SEGMENT_SIZE   0x7F  // Tamanho máximo de um único bloco de memória (127)
#define _MAX_HEAP_SIZE      (MAX_HEAP_SIZE - 1)

// Array que reserva o espaço de memória para o heap
unsigned char _uDynamicHeap[MAX_HEAP_SIZE];


// Aloca um bloco de memória do heap
unsigned char * SRAMalloc(unsigned char nBytes)
{
	SALLOC * pHeap;
	SALLOC * temp;
	SALLOC segHeader;
	unsigned char segLen;
	
	// Verifica se o tamanho solicitado é válido
	if (nBytes > (_MAX_SEGMENT_SIZE - 1)) return (0);
	
	// Inicia o ponteiro no começo do heap
	pHeap = (SALLOC *)_uDynamicHeap;
	
	while (1)
	{
		segHeader = *pHeap;
		segLen = segHeader.bits.count - 1;

		// Se chegou ao fim do heap, não há memória
		if (segHeader.byte == 0) return (0);
				
		// Se o bloco atual está livre
		if (!(segHeader.bits.alloc))
		{
			// Se o bloco é pequeno demais, tenta unir com o próximo
			if (nBytes > segLen)
			{
				if (!(_SRAMmerge(pHeap))) pHeap += segHeader.bits.count;	
			}
			// Se o tamanho é exato, simplesmente aloca o bloco
			else if (nBytes == segLen)
			{
				(*pHeap).bits.alloc = 1;
				return ((unsigned char *)(pHeap + 1));
			}
			// Se o bloco é maior, divide-o em dois (um alocado, um livre)
			else 
			{
				(*pHeap).byte = nBytes + 0x81;
				temp = pHeap + 1;
				pHeap += (nBytes + 1);
				(*pHeap).byte = segLen - nBytes;
				return ((unsigned char *) temp);
			}			
		}
		// Se o bloco está alocado, avança para o próximo
		else 
		{
			pHeap += segHeader.bits.count;
		}
	}
}

// Libera um bloco de memória previamente alocado
void SRAMfree(unsigned char * pSRAM)
{	
	// Acessa o cabeçalho e marca o bloco como não alocado
	(*(SALLOC *)(pSRAM - 1)).bits.alloc = 0;
}

// Inicializa o heap para a alocação de memória
void SRAMInitHeap(void)
{
	unsigned char * pHeap;
	unsigned int count;
	
	pHeap = _uDynamicHeap;
	count = _MAX_HEAP_SIZE;

	while (1)
	{
		// Divide o heap em blocos de tamanho máximo
		if (count > _MAX_SEGMENT_SIZE)
		{
			*pHeap = _MAX_SEGMENT_SIZE;
			pHeap += _MAX_SEGMENT_SIZE;
			count = count - _MAX_SEGMENT_SIZE;
		}
		// Cria o último bloco com o tamanho restante
		else
		{
			*pHeap = count;
			*(pHeap + count) = 0; // Adiciona o marcador de fim do heap
			return;
		}
	}
}

// Tenta unir dois blocos de memória livres e adjacentes
unsigned char _SRAMmerge(SALLOC * pSegA)
{
	SALLOC * pSegB;
	SALLOC uSegA, uSegB, uSum;
	
	pSegB = pSegA + (*pSegA).byte;
		
	uSegA = *pSegA;
	uSegB = *pSegB;
	
	// Não une se chegou ao fim do heap ou se algum bloco está alocado
	if (uSegB.byte == 0) return (0);
	if (uSegA.bits.alloc || uSegB.bits.alloc) return (0);
	if (uSegA.bits.count == _MAX_SEGMENT_SIZE) return (0);
		
	uSum.byte = uSegA.byte + uSegB.byte;	
		
	// Se a soma for maior que o máximo, ajusta os blocos
	if ((uSum.byte) > _MAX_SEGMENT_SIZE)
	{
		(*pSegA).byte = _MAX_SEGMENT_SIZE;
		pSegA += _MAX_SEGMENT_SIZE;
		pSegB += uSegB.byte;
		(*pSegA).byte = pSegB - pSegA;
		
		return (_MAX_SEGMENT_SIZE);
	}
	// Senão, combina os dois blocos em um só
	else
	{
		return ((*pSegA).byte = uSum.byte);
	}
}