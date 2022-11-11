#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    // srand, rand,...
#include <time.h>      // time 
#include "PCB.h"

typedef struct Process_queue {
    int max;
    int size;
    int first;
    int last;
    PCB *elements;
    int priority;
} Process_queue;

typedef struct Priority_queues {
    Process_queue *Queues;
    int numQueues;
} Priority_queues;



Process_queue * createQueue(int maxProcess, int priority) {
    Process_queue *Q;
    Q = (Process_queue *)malloc(sizeof(Process_queue));
    Q->elements = (PCB *)malloc(sizeof(PCB)*maxProcess*5);
    Q->size = 0;
    Q->max = maxProcess;
    Q->first = 0;
    Q->last = -1;
    Q->priority = priority;
    return Q;
}

Priority_queues * createPriorityQueues(int maxProcess, int Priorities) {
    Priority_queues *PQ;
    PQ = (Priority_queues *)malloc(sizeof(Priority_queues));
    PQ->Queues = (Process_queue *)malloc(sizeof(Process_queue)*Priorities);
    PQ->numQueues = Priorities;
    int i;
    for (i=0; i<Priorities; i++) {
        Process_queue *Q = createQueue(maxProcess, (Priorities - i));
        PQ->Queues[i] = *Q;
    }
    return PQ;
}

/*----------------------------------------------------------------- 
 *   Comprobar si una cola esta vacia
 *----------------------------------------------------------------*/

int colaVacia(Process_queue *Q) {
    if (Q->size == 0) {
        return 0;
    } else {
        return 1;
    }
}

/*----------------------------------------------------------------- 
 *   Obtener la prioridad de la cola
 *----------------------------------------------------------------*/

int getPriorityQueue(Process_queue *Q) {
    return Q->priority;
}

/*----------------------------------------------------------------- 
 *   Quitar primer proceso de mayor prioridad de las colas
 *----------------------------------------------------------------*/

int Dequeue(Process_queue *Q) {
    if(Q->size==0) {
        return 1;
    } else {
        Q->size--;
        Q->first++;
        return 0;
    }

}

void DequeueFromQueues(Priority_queues *PQ) {
    int i;
    for (i=0; i<PQ->numQueues; i++) {
        if (Dequeue(&PQ->Queues[i]) == 0) {
            return;
        }
    }
}

/*----------------------------------------------------------------- 
 *   Devolver el primer proceso de mayor prioridad de las colas
 *----------------------------------------------------------------*/

PCB * first(Process_queue *Q) {
    if(Q->size==0) {
        printf("Cola vacia \n");
        return createPCB(0,0,0);
    }
    PCB *pcb = &Q->elements[Q->first];
    return pcb;

}

PCB * firstInQueues(Priority_queues *PQ) {
    int i;
    for(i=0; i<PQ->numQueues; i++) {
        if (colaVacia(&PQ->Queues[i]) == 1) {
            return first(&PQ->Queues[i]);
        }
    }

    return createPCB(0,0,0);
}
/*----------------------------------------------------------------- 
 *   Añadir proceso a la cola asignada a su prioridad
 *----------------------------------------------------------------*/

void Enqueue(Process_queue *Q, PCB *process) {
    if(Q->size == Q->max) {
    } else {
        Q->size = Q->size + 1;
        Q->last = Q->last + 1;
        Q->elements[Q->last] = *process;
    }
    return;
    
}

void EnqueueInQueues(Priority_queues *PQ, PCB *process) {
    int i;
    for(i=0;i<PQ->numQueues;i++) {
        if (process->priority == getPriorityQueue(&PQ->Queues[i])) {
            Enqueue(&PQ->Queues[i], process);
            return;
        }
    }
}

/*----------------------------------------------------------------- 
 *   Liberar memoria de las colas
 *----------------------------------------------------------------*/

void liberarMemoriaQueue(Process_queue *Q) {
    free(Q);
}

void liberarMemoriaQueues(Priority_queues *PQ) {
    int i;
    for(i=0;i<PQ->numQueues;i++) {
            liberarMemoriaQueue(&PQ->Queues[i]);
        
    }
    free(PQ);
}
