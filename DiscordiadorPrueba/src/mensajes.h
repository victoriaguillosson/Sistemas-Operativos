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



typedef enum {
	NUEVOS_TRIPULANTES,
	INICIAR_PATOTA,
	LISTAR_TRIPULANTES,
	EXPULSAR_TRIPULANTE,
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	OBTENER_BITACORA,
	SOLICITAR_TAREA,
	INFORMAR_MOVIMIENTO,
	SOLICITAR_PROX_TAREA,
	HACER_TAREA

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
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
} idTareas;



typedef struct {
  ID_MENSAJE id;
  size_t size;
  void* content;
}t_message;

typedef struct {
	idTareas idMensaje;
	char *nombreTarea;
	uint32_t parametros; //para descartar basura NULL
	uint32_t posicionX;
	uint32_t posicionY;
	uint32_t tiempo; //en ciclos CPU
} tarea;

typedef struct{
	ID_MENSAJE idMensaje;
	idTareas tarea;
	uint32_t parametros;
}tareaAMandar;//ES LO QUE SE LE MANDA AL iMONGOSTORE


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
	ID_MENSAJE idMensaje; //LISTAR_TRIPULANTES
} listarTripulantes;


typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;

} expulsarTripulante;

typedef struct {
	ID_MENSAJE idMensaje;

} iniciarPlanificacion;

typedef struct {
	ID_MENSAJE idMensaje;
} pausarPlanificacion;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;

} obtenerBitacora;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;

} solicitarTarea;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;

} informarMovimiento;

typedef struct {
	ID_MENSAJE idMensaje;
	uint32_t idTripulante;

} solicitarProxTarea;




void armar_paquete(int conexion, char* leido);
int contarCantidadParamentros(char** lista);

////PAQUETES A MANDAR
//void sendPaquetePatota(iniciarPatota patota, int conexion);
//void sendPaqueteExpulsarTripulante(expulsarTripulante tripulante, int conexion);
//void sendPaqueteSolicitarTarea(solicitarTarea solicitarTarea, int conexion);
//void sendPaqueteInformarMov(informarMovimiento informarMovimiento, int conexion);
//void sendPaqueteProxTarea(solicitarProxTarea solicitarProxTarea, int conexion);
//void sendPaqueteObtenerBitacora(obtenerBitacora bitacora, int conexion);
//void sendPaqueteTarea(tareaAMandar tarea, int conexion);
//
////SERIALIZAZION DE PAQUETES
//void* serializarPatota(iniciarPatota patota);
//void* serializarExpulsarTripulante(expulsarTripulante tripulante);
//void* serializarSolicitarTarea(solicitarTarea solicitarTarea);
//void* serializarInformarMovimiento(informarMovimiento informarMovimiento);
//void* serializarSolicitarProxTarea(solicitarProxTarea solicitarProxTarea);
//void* serializarObtenerBitacora(obtenerBitacora bitacora);
//void* serializarTarea(tareaAMandar tarea);
//
//void recibirPaqueteRAM(int conexion);
//void recibirPaqueteiMongo(int conexion);



#endif /* MENSAJES_H_ */
