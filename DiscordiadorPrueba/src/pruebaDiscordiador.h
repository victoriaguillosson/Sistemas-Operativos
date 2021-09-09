/*
 * Modulo_Discordiador.h
 *
 *  Created on: 25 abr. 2021
 *      Author: utnso
 */

#ifndef PRUEBADISCORDIADOR_H_
#define PRUEBADISCORDIADOR_H_

#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/node.h>
#include <commons/config.h>
#include <commons/memory.h>
#include <commons/temporal.h>
#include <commons/string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include "mensajes.h"

/*t_config* configDiscordiador;

typedef struct {
	char* IP_MI_RAM_HQ;
	char* PUERTO_MI_RAM_HQ;
	char* IP_I_MONGO_STORE;
	char* PUERTO_I_MONGO_STORE;
	char* ALGORITMO;
	int GRADO_MULTITAREA;
	int QUANTUM;
	int DURACION_SABOTAJE;
	int RETARDO_CICLO_CPU;
} infoConfigPrueba;

infoConfigPrueba infoConfig;


typedef struct {
	ID_MENSAJE id;
	int cantTripulantes;
	int largoTareas;
	char* tareas;
	t_list* posiciones;
} iniciarPatota;

infoConfigPrueba obtenerInfoConfigMiRAMHQ();
int crearConexionDiscordiador(char* ip, char* puerto, t_log* logDiscordiador);
void mandarMensaje(int conexion);*/



typedef struct {
	uint32_t patotaID;
	int cantidadTripulantes;
	t_list* tareas;
	t_list* tripulantes;
} Patota;

typedef struct{
	char * tarea;
	int parametro;
	int posX;
	int posY;
	int tiempo;
	char* tareaEscrita;
	idTareas id;
} Task;

typedef struct{
	uint32_t tripulanteID;
	uint32_t patotaID;
//	ESTADO_TRIPULANTE estadoTripulante;
	uint32_t posX;
	uint32_t posY;
//	sem_t semExec;
//	sem_t semIO;
//	sem_t semEM;
//	sem_t semNew;
	int quantumUsado;
	int tengoTarea;
	int tiempoRestante;
	int comienzoAHacerTarea;
	int estoyFrenadoExec;
	int estoyFrenadoIO;
//	ACTIVIDAD actividad;
	Task tarea;
	int durmiendo;

} Tripulante;


int iniciarPatotaMemoria (Patota* patota);

#endif /* PRUEBADISCORDIADOR_H_ */
