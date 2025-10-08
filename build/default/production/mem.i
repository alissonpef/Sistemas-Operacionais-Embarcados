# 1 "mem.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 285 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include/language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "mem.c" 2
# 106 "mem.c"
# 1 "./mem.h" 1



# 1 "./types.h" 1



# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 1 3



# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/musl_xc8.h" 1 3
# 5 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 2 3
# 26 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 1 3
# 133 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef unsigned __int24 uintptr_t;
# 148 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef __int24 intptr_t;
# 164 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 194 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 235 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 27 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 2 3

typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;
typedef int24_t int_fast24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;
typedef uint24_t uint_fast24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 148 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/bits/stdint.h" 1 3
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 149 "C:\\Program Files\\Microchip\\xc8\\v3.00\\pic\\include\\c99/stdint.h" 2 3
# 5 "./types.h" 2
# 1 "./os_config.h" 1
# 6 "./types.h" 2





typedef void TASK;


typedef void (*f_ptr)(void);


typedef enum {READY = 0, RUNNING, WAITING, WAITING_SEM} state_t;


typedef struct tcb {
    uint8_t task_id;
    f_ptr task_func;
    state_t task_state;
    uint8_t task_priority;
    uint8_t task_time_to_waiting;

    uint8_t BSR_reg;
    uint8_t WORK_reg;
    uint8_t STATUS_reg;
    uint24_t STACK[32];
    uint8_t task_sp;
} tcb_t;


typedef struct f_aptos {
    tcb_t readyQueue[5];
    uint8_t readyQueueSize;
    tcb_t *taskRunning;
} f_aptos_t;

typedef struct semaphore {
    int contador;
    tcb_t *sem_queue[5];
    uint8_t sem_queue_in;
    uint8_t sem_queue_out;
} sem_t;

typedef struct pipe {
    uint8_t pipe_pos_read;
    uint8_t pipe_pos_write;
    char pipe_data[3];

    sem_t pipe_sem_read;
    sem_t pipe_sem_write;
} pipe_t;




typedef union _SALLOC
{
 unsigned char byte;
 struct _BITS
 {
  unsigned count:7;
  unsigned alloc:1;
 } bits;
}SALLOC;
# 5 "./mem.h" 2



unsigned char * SRAMalloc(unsigned char nBytes);
void SRAMfree(unsigned char *pSRAM);
void SRAMInitHeap(void);
     unsigned char _SRAMmerge(SALLOC * pSegA);
# 107 "mem.c" 2
# 119 "mem.c"
#pragma udata _SRAM_ALLOC_HEAP
unsigned char _uDynamicHeap[0x200];





#pragma udata access _SRAM_ALLOC
# 154 "mem.c"
unsigned char * SRAMalloc( unsigned char nBytes)
{
 SALLOC * pHeap;
 SALLOC * temp;
      SALLOC segHeader;
      unsigned char segLen;


 if (nBytes > (0x7F - 1)) return (0);


 pHeap = (SALLOC *)_uDynamicHeap;

 while (1)
 {

  segHeader = *pHeap;


  segLen = segHeader.bits.count - 1;


  if (segHeader.byte == 0) return (0);


  if (!(segHeader.bits.alloc))
  {

   if (nBytes > segLen)
   {

    if (!(_SRAMmerge(pHeap))) pHeap += segHeader.bits.count;
   }
   else



   if (nBytes == segLen)
   {

    (*pHeap).bits.alloc = 1;


    return ((unsigned char *)(pHeap + 1));
   }


   else
   {

    (*pHeap).byte = nBytes + 0x81;


    temp = pHeap + 1;


    pHeap += (nBytes + 1);


    (*pHeap).byte = segLen - nBytes;


    return ((unsigned char *) temp);
   }
  }


  else
  {
   pHeap += segHeader.bits.count;
  }
 }
}
# 250 "mem.c"
void SRAMfree(unsigned char * pSRAM)
{

 (*(SALLOC *)(pSRAM - 1)).bits.alloc = 0;
}
# 276 "mem.c"
void SRAMInitHeap(void)
{
 unsigned char * pHeap;
      unsigned int count;

 pHeap = _uDynamicHeap;
 count = 0x200 -1;

 while (1)
 {
  if (count > 0x7F)
  {
   *pHeap = 0x7F;
   pHeap += 0x7F;
   count = count - 0x7F;
  }
  else
  {
   *pHeap = count;
   *(pHeap + count) = 0;
   return;
  }
 }
}
# 322 "mem.c"
     unsigned char _SRAMmerge(SALLOC * pSegA)
{
 SALLOC * pSegB;
      SALLOC uSegA, uSegB, uSum;



 pSegB = pSegA + (*pSegA).byte;


 uSegA = *pSegA;
 uSegB = *pSegB;


 if (uSegB.byte == 0) return (0);


 if (uSegA.bits.alloc || uSegB.bits.alloc) return (0);


 if (uSegA.bits.count == 0x7F) return (0);


 uSum.byte = uSegA.byte + uSegB.byte;





 if ((uSum.byte) > 0x7F)
 {
  (*pSegA).byte = 0x7F;
  pSegA += 0x7F;
  pSegB += uSegB.byte;
  (*pSegA).byte = pSegB - pSegA;

  return (0x7F);
 }


 else
 {
  return ((*pSegA).byte = uSum.byte);
 }
}
