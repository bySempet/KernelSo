#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    // srand, rand,...
#include <time.h>      // time 
#include "ProcessQueue.h"

typedef struct Hilo {
    int id;
    PCB *pcb;
    int ocupado;
} Hilo;

typedef struct Core {
    int id;
    int numHilos;
    Hilo *hilos;
} Core;

typedef struct CPU {
    int id;
    int numCores;
    Core *cores;
} CPU;


Hilo * createHilo(int id) {
    Hilo *hilo;
    hilo = (Hilo *)malloc(sizeof(Hilo));
    hilo->id = id;
    hilo->pcb = (PCB *)malloc(sizeof(PCB));
    hilo->ocupado = 0;
    return hilo;

}

Core * createCore(int id, int numHilos) {
    Core *core;
    core = (Core *)malloc(sizeof(Core));
    core->id = id;
    core->numHilos = numHilos;
    core->hilos = (Hilo *)malloc(sizeof(Hilo)*numHilos);
    return core;
}

CPU * createCPU(int id, int cores, int hilos) {
    CPU *cpu;
    cpu = (CPU *)malloc(sizeof(CPU));
    cpu->id = id;
    cpu->numCores = cores;
    cpu->cores = (Core *)malloc(sizeof(Core)*cores);

    int i,j;
    for(i=0; i<cores; i++) {
        Core *c = createCore(i, hilos);
        for (j=0; j<hilos; j++) {
            Hilo *h = createHilo(j + i*hilos);
            c->hilos[j] = *h;
        }
        cpu->cores[i] = *c;
    }
    return cpu;
}


void printHilo(Hilo *hilo){
    if (hilo->ocupado==1) {
        printPCB(hilo->pcb);
    } else {
        printf("vacio \n");
    }
    
}

void printCore(Core *core){
    int i;
    for(i=0; i<core->numHilos; i++) {
        printf("Hilo %d: ", core->hilos[i].id);
        printHilo(&core->hilos[i]);
     }
}

void printCPU(CPU *cpu){
    int i;
    printf("CPU: \n");
    for(i=0; i<cpu->numCores; i++) {
        printf("Core %d: \n", cpu->cores[i].id);
        printCore(&cpu->cores[i]);
     }
}


int insertPCBinHilo(Hilo *hilo, PCB *pcb){
    if (hilo->ocupado == 0) {
        hilo->pcb = pcb;
        hilo->ocupado =1;
        return 0;
    } else {
        return 1;
    }
}

int insertPCBinCore(Core *core, PCB *pcb){
    int i;
    for(i=0; i<core->numHilos; i++) {
        if (insertPCBinHilo(&core->hilos[i], pcb)==0) {
            return 0;
        } 
    }
    return 1;
}

int insertPCBinCPU(CPU *cpu, PCB *pcb) {
    int i;
    if (pcb->ID != 0) {
        for(i=0; i<cpu->numCores; i++) {
            if (insertPCBinCore(&cpu->cores[i], pcb)==0) {
                return 0;
            }
        }
    }
    return 1;
}


void restarTiempoinHilo(Hilo *hilo){
    if (hilo->ocupado == 1) {
        if(restarTiempoinPCB(hilo->pcb)==1) {
            hilo->pcb = createPCB(0,0,0);
            hilo->ocupado = 0;
        }
    }
}

void restarTiempoinCore(Core *core){
    int i;
    for(i=0; i<core->numHilos; i++) {
        restarTiempoinHilo(&core->hilos[i]) ;
    }
}

void restarTiempoinCPU(CPU *cpu) {
    int i;
    for(i=0; i<cpu->numCores; i++) {
        restarTiempoinCore(&cpu->cores[i]);
    }
}



void quitarProcesosHilo(Priority_queues *PQ, Hilo *hilo) {
    int i;
    if (hilo->ocupado == 1) {
        DisminuirPrioridadPCB(hilo->pcb);
        EnqueueInQueues(PQ, hilo->pcb);
        hilo->pcb = createPCB(0,0,0);
        hilo->ocupado = 0;
    }
}

void quitarProcesosCore(Priority_queues *PQ, Core *core) {
    int i;
    for(i=0; i<core->numHilos; i++) {
        quitarProcesosHilo(PQ, &core->hilos[i]);
    }
}

void quitarProcesosCPU(Priority_queues *PQ, CPU *cpu) {
    int i;
    for(i=0; i<cpu->numCores; i++) {
        quitarProcesosCore(PQ, &cpu->cores[i]);
    }
}


void liberarMemoriaHilo(Hilo *hilo) {
    free(hilo);
}

void liberarMemoriaCore(Core *core) {
    int i;
    for (i=0;i<core->numHilos;i++) {
        liberarMemoriaHilo(&core->hilos[i]);
    }
    free(core);
}

void liberarMemoriaCPU(CPU *cpu){
    int i;
    for(i=0; i<cpu->numCores; i++) {
        liberarMemoriaCore(&cpu->cores[i]);
     }
     free(cpu);
}
