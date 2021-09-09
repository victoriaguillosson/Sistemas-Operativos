/*
 * mensajes.h
 *
 *  Created on: 7 may. 2021
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <commons/collections/list.h>
#include <string.h>
#include <readline/readline.h>
#include <commons/bitarray.h>
#include <semaphore.h>


typedef struct {
	int tamanioMemoria;
	char* esquemaMemoria;
	int tamanioPagina;
	int tamanioSwap;
	char* pathSwap;
	char* algoritmoReemplazo;
	char* criterioSeleccion;
	char* puerto;
} infoConfigMiRAMHQ;

typedef enum {
	LIBRE,
	OCUPADO
} estadoMemoria;

typedef enum {
	INICIAR_PATOTA = 4,
	EXPULSAR_TRIPULANTE = 5,
	SOLICITAR_TAREA = 6,
	INFORMAR_MOVIMIENTO = 7
} ID_MENSAJE;

typedef struct {
	int size;
	void* stream;
} t_buffer;

typedef struct {
	ID_MENSAJE codigo_operacion;
	void* stream;
	int sizePaquete;
} t_paquete;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK_IO,
	BLOCK2,
	FINISH
} estado;

typedef enum {
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
} idTareas;

typedef struct {
	uint32_t patotaID;
	int cantidadTripulantes;
	t_list* pathTareas;
	t_list* tripulantes;
} patotaTripulantes;

typedef struct {
  ID_MENSAJE id;
  size_t size;
  void* content;
}t_message;

typedef struct {
	ID_MENSAJE id;
	int cantTripulantes;
	int cantTareas;
	char* tareas;
	int tamTareas;
	t_list* posX;
	t_list* posY;
} iniciarPatota;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
} expulsarTripulante;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
} solicitarTarea;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
	uint32_t posX;
	uint32_t posY;
} informarMovimiento;

typedef struct {
	uint32_t pid;
	uint32_t direccionLogicaTareas;
} patotaControlBlock;

typedef struct {
	uint32_t tid;
	char estado;
	uint32_t posicionX;
	uint32_t posicionY;
	uint32_t proximaInstruccion;
	uint32_t dirLogicaPCB;
} tripulanteControlBlock;


typedef struct{
	int conexion;
	NIVEL* mapa;
} arg_struct;

typedef enum {
	PCB,
	TCB,
	TAREAS
}tipoDeEstructura;


void* punteroAMemoria;

sem_t* semMemoria;
sem_t* semMapa;

sem_t* semElementosEnDisco;
sem_t* semLogMiRAMHQ;

sem_t* semPaginasEnDisco;
sem_t* semAlgoritmo;

sem_t* semLugaresLibresEnDisco;

infoConfigMiRAMHQ infoConfig;
t_config * configMiRAMHQ;
t_log * logMiRAMHQ;
t_bitarray* bitmap;
int idTripulantes;
int idPatota;

int asignadorIdTareas;
t_list* elementosEnDisco; // Para el caso de Segmentacion es en Memoria


void recibirPaqueteRAM(arg_struct* argumentos);

void responderIniciarPatota(int cliente, int valorOK, t_list* tripulantes);
void responderSolicitarTarea(int cliente, uint32_t idTripulante, char* mensaje);


#endif /* MENSAJES_H_ */
