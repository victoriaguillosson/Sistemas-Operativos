/*
 * Modulo_Discordiador.h
 *
 *  Created on: 25 abr. 2021
 *      Author: utnso
 */

#ifndef MODULO_DISCORDIADOR_H_
#define MODULO_DISCORDIADOR_H_

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
#include <semaphore.h>
#include <time.h>
#include "mensajes.h"
#include "tripulante.h"
#include "estructuras.h"
#include "planificacion.h"


typedef enum {
	INVALIDO,
	LISTAR_TRIPULANTES,
	OBTENER_BITACORA,
	INICIAR_PATOTA_COMANDO,
	SALIR,
	EXPULSAR_TRIPULANTE_COMANDO,
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	CONECTAR_FS
} COMANDOS;

t_list* patotas;
int contadorPatotas= 0;
int contadorTripulantes=0;
int tripulanteElegido=-1;
pthread_t hiloServidor;
t_config * configDiscordiador;
t_dictionary * consoleCommandDictionary;

void fillDictionary(t_dictionary * dictionary);
void llenarConfig(t_config* configDiscordiador);
int iniciarPatota(char * parameters);
int listarTripulantes();
int obtenerBitacora(char * parameters);
int expulsarTripulante(char * parametro);
int procedimientoSabotaje(char* posiciones);
int finDeSabotaje();
void destruirPatota(Patota* patota);
void destruirTarea(Task* tarea);
void destruirTripulante(Tripulante* t);
int sabotajePosX;
int sabotajePosY;
int haySabotaje=0;
int distanciaMinimaAlSabotaje=100000;
void cargarCantidadTripulantes(Patota * patota, char * cantidadTripulantes);
void cargarTareas(Patota * patota, char * cantidadTripulantes);
void iniciarTripulantes(Patota * patota, char * posiciones);
void crearUtilidades();
void liberarTodo();
void desbloquearTripulantesNew(Patota* patota);
void destruirSemaforo(sem_t* semaforo);
int conexionConFileSystem();
int conexionConMemoria();
void matarHilosPlanificadores();

#endif /* MODULO_DISCORDIADOR_H_ */
