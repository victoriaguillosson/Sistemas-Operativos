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
#include "estructuras.h"

#define IPDEMISERVER "127.0.0.1"
#define PUERTODEMISERVER "5010"

typedef enum {
	NUEVOS_TRIPULANTES,
	INFO_BITACORA,
	ENVIAR_BITACORA,
	HACER_TAREA,
	INICIAR_PATOTA,
	EXPULSAR_TRIPULANTE,
	SOLICITAR_TAREA,
	INFORMAR_MOVIMIENTO
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

typedef struct {
  ID_MENSAJE id;
  size_t size;
  void* content;
}t_message;

typedef struct {
	IdTareas idMensaje;
	char *nombreTarea;
	uint32_t parametros; //para descartar basura NULL
	uint32_t posicionX;
	uint32_t posicionY;
	uint32_t tiempo; //en ciclos CPU
} Tarea;

typedef struct{
	ID_MENSAJE idMensaje;
	IdTareas tarea;
	uint32_t parametros;
} TareaAMandar;//ES LO QUE SE LE MANDA AL iMONGOSTORE

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
} ExpulsarTripulante;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
} ObtenerBitacora;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
} SolicitarTarea;

typedef struct {
	ID_MENSAJE idMensaje; //INFORMAR_MOVIMIENTO
	uint32_t idTripulante;
	uint32_t largoDeInfo;
	char* info;
} InformarMovimiento;

typedef struct {
	ID_MENSAJE id;
	int cantTripulantes;
	int largoTareas;
	char* tareas;
	t_list* posiciones;
} IniciarPatota;

typedef struct{
	ID_MENSAJE idMensaje; //NUEVOS_TRIPULANTES
	uint32_t cantNuevoTripualnates;
	t_list* nuevosTripulantes;
} NuevosTripulantes;

void armar_paquete(int conexion, char* leido);
int contarCantidadParamentros(char** lista);

void recibirPaqueteRAM(int conexion);
void recibirPaqueteiMongo(int conexion);
void iniciarServidor();
void alertaDeSabotaje(void* buffer);
int crearConexionDiscordiador(char* ip, char* puerto, t_log* logDiscordiador);
char* pedirBitacora(int tripulante);
int iniciarPatotaMemoria (Patota* patota);
Task solicitarTarea(int tripulante);
Task tareaToTask(char* tarea);
void mensajeExpulsarTripulante(int tripulante);
void nuevosTripulantes (Patota* patota);
void informarMovimiento(int tripulante, int x, int y, int x2, int y2);
void infoParaBitacora(int tripulante, char* bitacora);
void avisarComienzoFSCK (int flag);
void hacerTarea(Task tarea);
Task asignarIdTarea(Task tarea);
Task* asignarIdTask(Task* tarea);

t_log* logMensajes;
int* clienteConMemoria;
int* clienteConFileSystem;
int servidorDiscordiador;
int clienteDeMiServidor;
int fsck;
int primerConexionMemoria;
int primerConexionFS;
void* bufferServer;

#endif /* MENSAJES_H_ */
