/*
 * mensajes.c
 *
 *  Created on: 7 may. 2021
 *      Author: utnso
 */

#include "mensajes.h"
//LOGEAR TODO!!!!!!!!!!

//t_log* logger;
//void armar_paquete(int conexion, char* leido) {
//
//    //char* leido = readline(">");
//
//    log_info(logger, "Iniciando el armado de paquete..");
//
//    // Lo podemos cambiar por string_starts_with()
//    if(strstr(leido, "INICIAR_PATOTA") != NULL ){ //PARA MIRAMHQ
//
//    	int totalParametrosSupuestos;
//    	iniciarPatota patota;
//    	char** lista = string_n_split(leido, 3, " ");
//    	patota.idMensaje = atoi(lista[0]);
//    	patota.tripulantes = atoi(lista[1]);
//    	patota.pathTareas = lista[2];
//    	patota.sizeCharPath = string_length(patota.pathTareas);
//
//    	lista = string_split(leido, " ");
//    	int parametros = contarCantidadParamentros(lista);
//
//    	for(int i=3; i < parametros; i++){
//    		list_add(patota.posiciones, lista[i]);
//    	}
//
//    	int diferenciaRecibido = patota.tripulantes - (parametros - 3);
//
//    	if(diferenciaRecibido > 0){
//    		for(int i = 0; i < diferenciaRecibido; i++)
//    			list_add(patota.posiciones, "0|0");
//    	}
//
//
//    	sendPaquetePatota(patota,conexion);
//
//
//    }else if(strstr(leido, "EXPULSAR_TRIPULANTE") != NULL ){ //PARA MIRAMHQ
//
//    	expulsarTripulante tripulante;
//
//    	char** lista = string_split(leido, " ");
//
//    	tripulante.idMensaje = atoi(lista[0]);
//    	tripulante.idTripulante = atoi(lista[1]);
//
//    	sendPaqueteExpulsarTripulante(tripulante,conexion);
//
//    }else if (strstr(leido, "SOLICITAR_TAREA") != NULL){ //PARA MIRAMHQ
//
//		solicitarTarea solicitarTarea;
//
//		char** lista = string_split(leido, " ");
//
//		solicitarTarea.idMensaje = atoi(lista[0]);
//		solicitarTarea.idTripulante = atoi(lista[1]);
//
//		sendPaqueteSolicitarTarea(solicitarTarea,conexion);
//
//
//	} else if (strstr(leido, "INFORMAR_MOVIMIENTO") != NULL){ //PARA MIRAMHQ
//
//		informarMovimiento informarMovimiento;
//
//		char** lista = string_split(leido, " ");
//
//		informarMovimiento.idMensaje = atoi(lista[0]);
//		informarMovimiento.idTripulante = atoi(lista[1]);
//
//		sendPaqueteInformarMov(informarMovimiento,conexion);
//
//
//	} else if (strstr(leido, "SOLICITAR_PROX_TAREA") != NULL){ //PARA MIRAMHQ
//
//		solicitarProxTarea solicitarProxTarea;
//
//		char** lista = string_split(leido, " ");
//
//		solicitarProxTarea.idMensaje = atoi(lista[0]);
//		solicitarProxTarea.idTripulante = atoi(lista[1]);
//
//		sendPaqueteProxTarea(solicitarProxTarea,conexion);
//
//	}else if(strstr(leido, "OBTENER_BITACORA") != NULL ){ //PARA iMONGOSTORE
//
//    	obtenerBitacora bitacora;
//
//		char** lista = string_split(leido, " ");
//
//		bitacora.idMensaje = atoi(lista[0]);
//		bitacora.idTripulante = atoi(lista[1]);
//
//		sendPaqueteObtenerBitacora(bitacora,conexion);
//
//    }
//
//}
//
//
//
//int contarCantidadParamentros(char** lista){
//
//	int cantidadParametros = 0;
//	while (lista[cantidadParametros]!= NULL){
//		cantidadParametros++;
//	}
//	return cantidadParametros;
//}
//
//


//
//
////PAQUETES A MANDAR
//
//void sendPaquetePatota(iniciarPatota patota, int conexion){
//	int bytesAMandar = sizeof(patota);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//
//	void* aEnviar = serializarPatota(patota);
//
//	log_info(logger, "Enviando mensaje");
//	send(conexion, aEnviar, sizeof(patota), 0);
//}
//
//void sendPaqueteExpulsarTripulante(expulsarTripulante tripulante, int conexion){
//	int bytesAMandar = sizeof(tripulante);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//	void* aEnviar = serializarExpulsarTripulante(tripulante);
//
//	log_info(logger, "Enviando mensaje");
//	send(conexion, aEnviar, sizeof(tripulante), 0);
//}
//
//void sendPaqueteSolicitarTarea(solicitarTarea solicitarTarea, int conexion){
//	int bytesAMandar = sizeof(solicitarTarea);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//	void* aEnviar = serializarSolicitarTarea(solicitarTarea);
//
//	log_info(logger, "Enviando mensaje para Solicitar Tarea");
//	send(conexion, aEnviar, sizeof(solicitarTarea), 0);
//}
//
//void sendPaqueteInformarMov(informarMovimiento informarMovimiento, int conexion){
//	int bytesAMandar = sizeof(informarMovimiento);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//	void* aEnviar = serializarInformarMovimiento(informarMovimiento);
//
//	log_info(logger, "Enviando mensaje para Informar Movimiento");
//	send(conexion, aEnviar, sizeof(informarMovimiento), 0);
//}
//
//void sendPaqueteProxTarea(solicitarProxTarea solicitarProxTarea, int conexion){
//	int bytesAMandar = sizeof(solicitarProxTarea);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//	void* aEnviar = serializarSolicitarProxTarea(solicitarProxTarea);
//
//	log_info(logger, "Enviando mensaje para Solicitar Proxima Tarea");
//	send(conexion, aEnviar, sizeof(solicitarProxTarea), 0);
//}
//
//void sendPaqueteObtenerBitacora(obtenerBitacora bitacora, int conexion){
//	int bytesAMandar = sizeof(bitacora);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//	void* aEnviar = serializarObtenerBitacora(bitacora);
//
//	log_info(logger, "Enviando mensaje");
//	send(conexion, aEnviar, sizeof(bitacora), 0);
//
//}
//
//void sendPaqueteTarea(tareaAMandar tarea, int conexion){
//	int bytesAMandar = sizeof(tarea);
//	send(conexion, &bytesAMandar, sizeof(int), 0);
//
//	void* aEnviar = serializarTarea(tarea);
//
//	log_info(logger, "Enviando mensaje");
//	send(conexion, aEnviar, sizeof(tarea), 0);
//}
//
//
//
//
//
//
//
//
////SERIALIZAMOS
//
//void* serializarPatota(iniciarPatota patota){
//
//	void* serializado = malloc(sizeof(patota));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(patota.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(patota.tripulantes), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//	memcpy(serializado + desplazamiento, &(patota.sizeCharPath), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//	memcpy(serializado + desplazamiento, &(patota.pathTareas), sizeof(char)*(patota.sizeCharPath));
//	desplazamiento += sizeof(char);
//	memcpy(serializado + desplazamiento, &(patota.posiciones), sizeof(t_list)*(patota.tripulantes));
//	desplazamiento += sizeof(t_list)*(patota.tripulantes);
//
//
//	return serializado;
//
//}
//
//
//void* serializarExpulsarTripulante(expulsarTripulante tripulante){
//
//	void* serializado = malloc(sizeof(tripulante));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(tripulante.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(tripulante.idTripulante), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//
//	return serializado;
//
//}
//
//void* serializarObtenerBitacora(obtenerBitacora bitacora){
//
//	void* serializado = malloc(sizeof(bitacora));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(bitacora.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(bitacora.idTripulante), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//
//	return serializado;
//
//}
//
//void* serializarSolicitarTarea(solicitarTarea solicitarTarea){
//
//	void* serializado = malloc(sizeof(solicitarTarea));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(solicitarTarea.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(solicitarTarea.idTripulante), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//
//	return serializado;
//
//}
//
//void* serializarInformarMovimiento(informarMovimiento informarMovimiento){
//
//	void* serializado = malloc(sizeof(informarMovimiento));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(informarMovimiento.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(informarMovimiento.idTripulante), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//
//	return serializado;
//
//}
//
//void* serializarSolicitarProxTarea(solicitarProxTarea solicitarProxTarea){
//
//	void* serializado = malloc(sizeof(solicitarProxTarea));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(solicitarProxTarea.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(solicitarProxTarea.idTripulante), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//
//	return serializado;
//
//}
//
//void* serializarTarea(tareaAMandar tarea){
//
//	void* serializado = malloc(sizeof(tarea));
//	int desplazamiento = 0;
//
//	memcpy(serializado + desplazamiento, &(tarea.idMensaje), sizeof(idMensajes));
//	desplazamiento += sizeof(idMensajes);
//	memcpy(serializado + desplazamiento, &(tarea.tarea), sizeof(idTareas));
//	desplazamiento += sizeof(idTareas);
//	memcpy(serializado + desplazamiento, &(tarea.parametros), sizeof(uint32_t));
//	desplazamiento += sizeof(uint32_t);
//
//	return serializado;
//}
//
//
//
//
//
//
////RECIBIMOS MSJs
//
//
//void recibirPaqueteRAM(int conexion){
//
//	idMensajes tipoDeMensaje;
//	int bytesARecibir;
//
//	recv(conexion, &bytesARecibir, sizeof(int), MSG_WAITALL);
//	void* buffer = malloc(bytesARecibir);
//	recv(conexion, buffer, bytesARecibir, 0);
//
//	int desplazamiento = sizeof(idMensajes);
//
//	memcpy(&tipoDeMensaje, buffer, sizeof(idMensajes));
//
//	if (tipoDeMensaje == INICIAR_PATOTA){
//		iniciarPatota patota;
//		patota.idMensaje = INICIAR_PATOTA;
//
//		memcpy(&(patota.tripulantes), buffer + desplazamiento, sizeof(uint32_t));
//		desplazamiento += sizeof(uint32_t);
//		memcpy(&(patota.sizeCharPath), buffer + desplazamiento, sizeof(uint32_t));
//		desplazamiento += sizeof(uint32_t);
//		memcpy(&(patota.pathTareas), buffer + desplazamiento, sizeof(char) * (patota.sizeCharPath));
//		desplazamiento += sizeof(char)* (patota.sizeCharPath);
//		memcpy(&patota.posiciones, buffer + desplazamiento, sizeof(t_list)*(patota.tripulantes));
//		desplazamiento += sizeof(t_list)*(patota.tripulantes);
//
//
//	}else if (tipoDeMensaje ==  EXPULSAR_TRIPULANTE){
//		expulsarTripulante expulsarTripulante;
//
//		expulsarTripulante.idMensaje = EXPULSAR_TRIPULANTE;
//		memcpy(&(expulsarTripulante.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
//
//	}else if(tipoDeMensaje == SOLICITAR_TAREA){
//		solicitarTarea tarea;
//		tarea.idMensaje = SOLICITAR_TAREA;
//		memcpy(&(tarea.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
//
//	}else if(tipoDeMensaje == INFORMAR_MOVIMIENTO){
//		informarMovimiento mov;
//		mov.idMensaje = INFORMAR_MOVIMIENTO;
//		memcpy(&(mov.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
//
//	}else if(tipoDeMensaje == SOLICITAR_PROX_TAREA){
//		solicitarProxTarea tarea;
//		tarea.idMensaje = SOLICITAR_PROX_TAREA;
//		memcpy(&(tarea.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
//
//	}else{
//		printf("ERROR EN MENSAJE");
//	}
//
//	if (tipoDeMensaje ==  OBTENER_BITACORA){
//		obtenerBitacora obtenerBitacora;
//
//		obtenerBitacora.idMensaje = OBTENER_BITACORA;
//
//		memcpy(&(obtenerBitacora.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
//
//	}
//
//	free(buffer);
//}
//
//void recibirPaqueteiMongo(int conexion){
//
//
//	idMensajes tipoDeMensaje;
//	int bytesARecibir;
//
//	recv(conexion, &bytesARecibir, sizeof(int), MSG_WAITALL);
//	void* buffer = malloc(bytesARecibir);
//	recv(conexion, buffer, bytesARecibir, 0);
//
//	int desplazamiento = sizeof(idMensajes);
//
//	memcpy(&tipoDeMensaje, buffer, sizeof(idMensajes));
//
//	if (tipoDeMensaje ==  OBTENER_BITACORA){
//		obtenerBitacora obtenerBitacora;
//		obtenerBitacora.idMensaje = OBTENER_BITACORA;
//		memcpy(&(obtenerBitacora.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
//
//	}else if(tipoDeMensaje == HACER_TAREA){
//		tareaAMandar tarea;
//		tarea.idMensaje = HACER_TAREA;
//		memcpy(&(tarea.tarea), buffer + desplazamiento, sizeof(idTareas));
//		desplazamiento += sizeof(idTareas);
//		memcpy(&(tarea.parametros), buffer + desplazamiento, sizeof(uint32_t));
//	}
//}


