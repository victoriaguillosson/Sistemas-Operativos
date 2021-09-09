/*
 * hilos.h
 *
 *  Created on: 4 jun. 2021
 *      Author: utnso
 */

#ifndef HILOS_H_
#define HILOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <curses.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <semaphore.h>

#include "paginacion.h"
#include "segmentacion.h"

typedef struct{
	pthread_t hilo;
	uint32_t indexDelHilo;
	int conexion;
	NIVEL* mapa;
}hilo;

sem_t * semIdTripulante;
sem_t * semIdPatota;


void nuevoHilo(int conexionCliente, NIVEL* mapa);
void mainHilo(void* argumentos);


patotaControlBlock* crearPCB();
tripulanteControlBlock* crearTCB(patotaControlBlock* pcb, uint32_t posX, uint32_t posY, int i, int direLogPCB);
t_list* crearTCBs(iniciarPatota patota, patotaControlBlock* pcb, int direLogPCB, NIVEL* mapa);

void actualizarPCB(int dirLogicaTareas, patotaControlBlock pcb);

int calcularTamanioPCB(patotaControlBlock pcb);
int calcularTamanioTCB(int i, t_list* listaTCBs);

int calcularTamanioTareas(iniciarPatota patota);

int dirLogica(int tamanio);

void asignarTareaATripulante(tripulanteControlBlock* tripulante, char* tareas);
void asignarTareaSiguiente(tripulanteControlBlock* tripulante, char* sigTarea);



void frezee(char** array);

#endif /* HILOS_H_ */
