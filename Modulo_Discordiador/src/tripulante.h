#ifndef TRIPULANTE_H_
#define TRIPULANTE_H_

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
#include <commons/log.h>
#include <assert.h>
#include <time.h>
#include "mensajes.h"
#include "estructuras.h"
#include "planificacion.h"

extern int planificando;
extern int tripulanteElegido;
extern int sabotajePosX;
extern int sabotajePosY;

void* startThread(void* args);
int tripulanteExec (Tripulante* parametro);
int tripulanteIO (Tripulante* parametro);
int tripulanteEM (Tripulante* parametro);


#endif
