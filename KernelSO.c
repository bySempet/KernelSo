#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>      
#include "CPU.h"

#define MAXPROCESS 100
#define TIME_TO_EXIT 100
#define TIMER_TIME 5
#define PRIORITIES 120
#define TIEMPOVIDAMAXIMO 30
#define PROCESSGENERATORTIME 5

int cores, hilos;

typedef struct { 
  int pq; 
  pthread_t tid; 
} identif_processGenerator; 

typedef struct {
    int c;
    pthread_t tid;
} identif_clock;

typedef struct {
    int t;
    pthread_t tid;
} identif_timer;

typedef struct {
    int s;
    pthread_t tid;
} identif_scheduler;

typedef struct shared_Memory {
    int seg;
    int timer0;
    int process;
} shared_Memory;

sem_t seg;
pthread_mutex_t mutexSeg, mutexTimer, mutexProcess;

Priority_queues *priorityQueues;
CPU *cpu;
shared_Memory sharedMemory;

void inicializar();
void *processGenerator(void *pq);
void *pclock(void *c);
void *timer(void *t);
void *scheduler(void *s);

int main(int argc, char *argv[]){

    inicializar();

    identif_processGenerator idProcessGenerator;
    identif_clock idPclock;
    identif_timer idTimer;
    identif_scheduler idScheduler;
    idProcessGenerator.pq = 0;
    idPclock.c = 0;
    idTimer.t = 0;
    idScheduler.s = 0;

    pthread_create(&(idProcessGenerator.tid),NULL,processGenerator,&(idProcessGenerator.pq));
    pthread_create(&(idPclock.tid),NULL,pclock,&(idPclock.c));
    pthread_create(&(idTimer.tid),NULL,timer,&(idTimer.t));
    pthread_create(&(idScheduler.tid),NULL,scheduler,&(idScheduler.s));
    sleep(TIME_TO_EXIT);
    printf("Fin \n");
    return(0);
}

void inicializar() {
    sharedMemory.seg = 0;
    sharedMemory.timer0= 0;
    sharedMemory.process= 0;
    priorityQueues= createPriorityQueues(MAXPROCESS, PRIORITIES);
    
    printf("Introduce el número de cores de la CPU: \n");
    scanf("%d", &cores);
    printf("Introduce el número de hilos por cada core de la CPU: \n");
    scanf("%d", &hilos);
    cpu = createCPU(1,cores,hilos);
    pthread_mutex_init(&mutexSeg, NULL);
    pthread_mutex_init(&mutexTimer, NULL);
    pthread_mutex_init(&mutexProcess, NULL);
    sem_init(&seg, 0, 0);

}

void *processGenerator(void *pq) {
    int i;
    for (i=1; i<=MAXPROCESS; i++) {
        int prioridad = ((rand()%PRIORITIES));
        int sleepSeg = ((rand()%PROCESSGENERATORTIME) +1);
        int tVida = ((rand()%TIEMPOVIDAMAXIMO) +1);
        PCB *pcb = createPCB(i,(tVida),prioridad);
        pthread_mutex_lock(&mutexProcess);
        EnqueueInQueues(priorityQueues, pcb);
        sharedMemory.process = 1;
        pthread_mutex_unlock(&mutexProcess);
        sleep(sleepSeg);
    }
}


void *scheduler(void *s) {
    int i;
    while(1){
        pthread_mutex_lock(&mutexProcess);
        if (sharedMemory.process == 1) {
            for (i=0; i<(cores*hilos);i++) {
                if(insertPCBinCPU(cpu, firstInQueues(priorityQueues)) == 0) {
                    DequeueFromQueues(priorityQueues);
                }
            }
            sharedMemory.process = 0;
        }
        pthread_mutex_unlock(&mutexProcess);
    
        pthread_mutex_lock(&mutexTimer);
        if (sharedMemory.timer0 == 1) {
            quitarProcesosCPU(priorityQueues, cpu);
            for (i=0; i<(cores*hilos);i++) {
                if(insertPCBinCPU(cpu, firstInQueues(priorityQueues)) == 0) {
                    DequeueFromQueues(priorityQueues);
                }
            }
            sharedMemory.timer0 = 0;
        }
        pthread_mutex_unlock(&mutexTimer);
    }
}

void *pclock(void *c) {
    sharedMemory.seg=0;
    while(1) {
        sleep(1);
        pthread_mutex_lock(&mutexSeg);
        sharedMemory.seg++;
        printf("seg= %d \n", sharedMemory.seg);
        restarTiempoinCPU(cpu);

        pthread_mutex_lock(&mutexProcess);
        sharedMemory.process = 1;
        pthread_mutex_unlock(&mutexProcess);

        pthread_mutex_unlock(&mutexSeg);
        sem_post(&seg);
        printCPU(cpu);
    }
}

void *timer(void *t) {
    int seg_aux = 0;
    sharedMemory.timer0 = 0;
    while(1) {
        sem_wait(&seg);
        pthread_mutex_lock(&mutexSeg);
        if (sharedMemory.seg == seg_aux + TIMER_TIME) {
            pthread_mutex_lock(&mutexTimer);
            sharedMemory.timer0 = 1;
            pthread_mutex_unlock(&mutexTimer);
            seg_aux = sharedMemory.seg;
        }
        pthread_mutex_unlock(&mutexSeg);

    }
}


void liberarMemoria(Priority_queues *PQ, CPU *cpu) {
    liberarMemoriaCPU(cpu);
    liberarMemoriaQueues(PQ);
}
