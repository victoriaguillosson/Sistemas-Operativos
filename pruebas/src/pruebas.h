/*
 * pruebas.h
 *
 *  Created on: 27 jul. 2021
 *      Author: utnso
 */

#ifndef PRUEBAS_H_
#define PRUEBAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <commons/collections/list.h>
#include <string.h>
#include <fcntl.h>
#include <readline/readline.h>

#define IPDEMISERVER "127.0.0.1"
#define PUERTODEMISERVER "5010"


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
	ID_MENSAJE idMensaje; //NUEVOS_TRIPULANTES
	uint32_t cantNuevosTripulantes;
	t_list* nuevosTripulantes;
}nuevosTripulantes;

typedef struct{
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
	uint32_t largoDeInfo;
	char* info;
}infoParaBitacora;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;
}bitacoraArecibir;

typedef struct{
	ID_MENSAJE idMensaje;
	idTareas tarea;
	uint32_t parametros;
}tareaAHacer;

typedef struct{
	int conexion;
	int tripu;
}arg_struct;

t_log* logDiscordiador;

void iniciarServidor();
void alertaDeSabotaje (void* buffer);
int crearConexionDiscordiador(char* ip, char* puerto, t_log* logDiscordiador);
void nuevosTripus(int conexion, int a);
void mandarInfoBitacora(int conexion, int a, char* mnsj);
void pedirBitacora(int conexion, int a);
void hacerTarea(int conexion, int parmetros, idTareas tar);
void mainHilo(void* argumentos);
void nuevoHilo(int conexionCliente, int tripu);



#endif /* PRUEBAS_H_ */
