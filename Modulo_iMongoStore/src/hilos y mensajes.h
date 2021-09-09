/*
 * hilos.h
 *
 *  Created on: 15 may. 2021
 *      Author: utnso
 */

#ifndef HILOS_Y_MENSAJES_H_
#define HILOS_Y_MENSAJES_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

#include "createFS.h"


sem_t* mutexTripulantes;
sem_t* nuevaBitaco;
sem_t* agregarADiccionario;
sem_t* semOxigeno;
sem_t* semComida;
sem_t* semBasura;


//HILOS

typedef struct{
	pthread_t hilo;
	uint32_t indexDelHilo;
	int conexion;
}hilo;


t_dictionary* diccionarioDeSemBitaco;

int bloqueoDeServidor;

void nuevoHilo(int conexionCliente);
void mainHilo(void* socket);



//MENSAJES

typedef enum {
	NUEVOS_TRIPULANTES,
	INFO_BITACORA,
	ENVIAR_BITACORA,
	HACER_TAREA
} ID_MENSAJE;

typedef enum {
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
} idTareas;


typedef struct{
	int conexion;
}arg_struct;

typedef struct{
	ID_MENSAJE idMensaje;
	uint32_t cantNuevosTripulantes;
	t_list* nuevosTripulantes;//Lista con los nuevos tripulantes
}nuevosTripualntes;

typedef struct{
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
	uint32_t largoDeInfo;
	char* info;
}infoParaBitacora;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
}bitacoraAEnviar;

typedef struct{
	ID_MENSAJE idMensaje;
	idTareas tarea;
	uint32_t parametros;
}tareaAHacer;




void recibirPaqueteiMongo(int conexion);

//NUEVOS_TRIPULANTES
void crearBitacoras(nuevosTripualntes tripulantes);

//INFO_BITACORA
void agregarABitacora(infoParaBitacora info);

//ENVIAR_BITACORA
void enviarBitacora(bitacoraAEnviar bitacora, int conexion);


//HACER_TAREA
void hacerTarea(tareaAHacer tarea);

void agregarEnArchivoBlocks(tareaAHacer tarea, t_config* configFile);
void sacarDeArchivoBlocks(tareaAHacer tarea, t_config* configFile);

file valoresGenerales(t_config* config, char** bloks);
//HACER_TAREA



//Comunes
char* llenar(char* caracter, uint32_t veces);
char* borrarUltimoBloque(char* array);
char* arrayAString(char** array, int largo);
char* tlistAString(t_list* array, int cantDeBloques);

int obtenerProxBloqueVacio();
void limpiarBitEnBlock(int bloque);
int ultimoBloque(char** array);


t_list* arrayATlist(char** array);

#endif /* HILOS_Y_MENSAJES_H_ */





