#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    // srand, rand,...
#include <time.h>      // time 


typedef struct Physical {
    int size;
    int first;
    int kernelIni;
    int kernelFi;
    int *elements;
} Physical;


Physical * createPhysicalMemory(int size_bits, int kernelIni, int kernelFi)
{
    Physical *P;
    P = (Physical*) malloc(sizeof(Physical));
    P->size = size_bits;
    
}