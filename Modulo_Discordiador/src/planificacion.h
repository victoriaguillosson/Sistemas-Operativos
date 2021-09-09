#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/node.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <commons/log.h>
#include <assert.h>
#include <time.h>
#include "estructuras.h"

int planificando;
int haySabotaje;
int errorNew;
int primeraVez;
int planisBloqueadas;
sem_t hayLugarEnExec;
sem_t hayAlguienEnReady;
sem_t alertaQuantum;
sem_t siguienteIO;
sem_t hayAlguienEnIO;
sem_t semaforoNew;
sem_t semPlanificando;
sem_t turnosQuantum;
sem_t semLogCambioCola;

int iniciarPlanificacion();
int pausarPlanificacion();
void readyToExec();
void execToIO();
void desalojoQuantum();

#endif
