#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct PCB {
    int ID;
    int totalTime; 
    int timeLeft;
    int priority;
    mm  mm;
} PCB;
typedef struct mm 
{
    int * code;
    int * data;
    int * pagTable;
}mm;
/*----------------------------------------------------------------- 
 *   Crear PCB
 *----------------------------------------------------------------*/

PCB * createPCB(int id,int tT, int priority) {
    PCB *pcb;
    pcb = (PCB *)malloc(sizeof(PCB));
    pcb->ID = id;
    pcb->totalTime = tT;
    pcb->timeLeft = tT;
    pcb->priority = priority;
    return pcb;
}

/*----------------------------------------------------------------- 
 *   Disminuir tiempo restante al PCB
 *----------------------------------------------------------------*/

int restarTiempoinPCB(PCB *pcb) {
    pcb->timeLeft = pcb->timeLeft - 1;
    if (pcb->timeLeft == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*----------------------------------------------------------------- 
 *   Disminuir prioridad del PCB
 *----------------------------------------------------------------*/

int DisminuirPrioridadPCB(PCB *pcb) {
    
    if (pcb->priority == 0 || pcb->priority == 1) {
        return 1;
    } else {
        pcb->priority = pcb->priority - 1;
        return 0;
    }
}

/*----------------------------------------------------------------- 
 *   Print PCB
 *----------------------------------------------------------------*/

int getID(PCB *pcb) {
    return pcb->ID;
}

int getTotalTime(PCB *pcb) {
    return pcb->totalTime;
}

int getTimeLeft(PCB *pcb) {
    return pcb->timeLeft;
}

int getPriorityPCB(PCB *pcb) {
    return pcb->priority;
}

void printPCB (PCB *pcb) {
    printf("PCB = %d, Total time = %d, Time Left = %d, Priority = %d \n", getID(pcb), getTotalTime(pcb),
    getTimeLeft(pcb), getPriorityPCB(pcb));
    return;
}
