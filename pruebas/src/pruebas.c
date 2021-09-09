/*
 ============================================================================
 Name        : pruebas.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "pruebas.h"



int crearConexionDiscordiador(char* ip, char* puerto, t_log* logDiscordiador){
	  	struct addrinfo hints;
	    struct addrinfo *server_info;
	    //int setAddr;
	    int clientSocket;

	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE;

	    if(getaddrinfo(ip, puerto, &hints, &server_info)){
	    	log_error(logDiscordiador, "ERROR EN GETADDRINFO");
	    	exit(-1);
	    }

	    clientSocket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	    if(connect(clientSocket, server_info->ai_addr, server_info->ai_addrlen) == -1){
	        log_error(logDiscordiador, "FALLÓ CONEXIÓN ENTRE DISCORDIADOR E iMONGOSOTRE...");
	        exit(-1);
	    }

	    log_info(logDiscordiador, "CONEXIÓN EXITOSA...");

	    freeaddrinfo(server_info);
	    return clientSocket;
}


void nuevosTripus(int conexion, int a){

	nuevosTripulantes tripulantes;

	tripulantes.idMensaje=NUEVOS_TRIPULANTES;
	tripulantes.cantNuevosTripulantes = 1;
	tripulantes.nuevosTripulantes = list_create();
	list_add(tripulantes.nuevosTripulantes, &a);


	void* buffer;
	size_t desplazamiento = 0;

	size_t espacioNecesario = sizeof(ID_MENSAJE)+sizeof(uint32_t)+sizeof(int)*tripulantes.cantNuevosTripulantes;
	buffer = malloc(espacioNecesario);

	memcpy(buffer + desplazamiento, &(tripulantes.idMensaje), sizeof(ID_MENSAJE));
	desplazamiento += sizeof(ID_MENSAJE);
	memcpy(buffer + desplazamiento, &(tripulantes.cantNuevosTripulantes), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	int* valor;
	for(int i=0; i<tripulantes.cantNuevosTripulantes; i++){
		valor = list_get(tripulantes.nuevosTripulantes, i);
		memcpy(buffer + desplazamiento, valor, sizeof(int));
		desplazamiento += sizeof(int);
	}


	send(conexion, &desplazamiento, sizeof(int), 0);

	send(conexion, buffer, desplazamiento, 0);

	list_destroy(tripulantes.nuevosTripulantes);
	free(buffer);
}

void mandarInfoBitacora(int conexion, int a, char* mnsj){

	infoParaBitacora info;
	info.idMensaje = INFO_BITACORA;
	info.info = mnsj;
	info.largoDeInfo = string_length(info.info);
	info.idTripulante = a;

	char* informacion = info.info;

	size_t bytesInfo = sizeof(char)*info.largoDeInfo;
	size_t espacioNecesario = sizeof(ID_MENSAJE)+sizeof(uint32_t)*2+bytesInfo;
	void* buffer = malloc(espacioNecesario);
	size_t desplazamiento = 0;

	memcpy(buffer + desplazamiento, &(info.idMensaje), sizeof(ID_MENSAJE));
	desplazamiento += sizeof(ID_MENSAJE);
	memcpy(buffer + desplazamiento, &(info.idTripulante), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(info.largoDeInfo), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, informacion, bytesInfo);
	desplazamiento += sizeof(char)*info.largoDeInfo;

	int bytesAMandar = desplazamiento;
	send(conexion, &bytesAMandar, sizeof(int), 0);

	send(conexion, buffer, bytesAMandar, 0);

	free(buffer);
}

void pedirBitacora(int conexion, int a){
	bitacoraArecibir bitacora;
	bitacora.idMensaje = ENVIAR_BITACORA;
	bitacora.idTripulante = a;

	//size_t espacioNecesario = sizeof(ID_MENSAJE)+sizeof(int);
	void* buffer = malloc(sizeof(bitacoraArecibir));
	size_t desplazamiento = 0;

	memcpy(buffer + desplazamiento, &(bitacora.idMensaje), sizeof(ID_MENSAJE));
	desplazamiento += sizeof(ID_MENSAJE);
	memcpy(buffer + desplazamiento, &(bitacora.idTripulante), sizeof(int));
	desplazamiento += sizeof(int);

	int bytesAMandar = desplazamiento;
	send(conexion, &bytesAMandar, sizeof(int), 0);

	send(conexion, buffer, desplazamiento, 0);

	free(buffer);

	size_t bytesRecv = 0;
	recv(conexion, &bytesRecv, sizeof(int), MSG_WAITALL);
	void* bufferRecv = malloc(bytesRecv);
	recv(conexion, bufferRecv, bytesRecv, 0);


	//printf("BITACORA: %s\n", bufferRecv);

	free(bufferRecv);

}

void hacerTarea(int conexion, int parmetros, idTareas tar){

	tareaAHacer tarea;
	tarea.idMensaje = HACER_TAREA;
	tarea.parametros = parmetros;
	tarea.tarea = tar;

	void* buffer = malloc(sizeof(tareaAHacer));
	size_t desplazamiento = 0;

	memcpy(buffer + desplazamiento, &(tarea.idMensaje), sizeof(ID_MENSAJE));
	desplazamiento += sizeof(ID_MENSAJE);
	memcpy(buffer + desplazamiento, &(tarea.tarea), sizeof(idTareas));
	desplazamiento += sizeof(idTareas);
	memcpy(buffer + desplazamiento, &(tarea.parametros), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	send(conexion, &desplazamiento, sizeof(int), 0);
	send(conexion, buffer, desplazamiento, 0);

	free(buffer);
}





void mainHilo(void* argumentos){


	arg_struct* args = (arg_struct* ) argumentos;

	//RECIBIR PAQUETE
	pedirBitacora(args->conexion, args->tripu);

	close(args->conexion);
	free(args);
}

void nuevoHilo(int conexionCliente, int tripu){


	pthread_t newHilo;

	arg_struct* args = malloc(sizeof(arg_struct));
	args->conexion = conexionCliente;
	args->tripu = tripu;

	pthread_create(&newHilo, NULL, (void*)mainHilo, (void*)args);
	pthread_detach(newHilo);

}



int main(void) {


	void* pp = malloc(100);
	printf("%p", pp);


	logDiscordiador = log_create("discordiador.log", "Discordiador", 1, LOG_LEVEL_INFO);

	char* ip = "127.0.0.1";
	char* puerto = "5002";
	int conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);

//NUEVOS TRIPUS
	nuevosTripus(conexion, 0);

//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	nuevosTripus(conexion, 2);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	nuevosTripus(conexion, 8);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	nuevosTripus(conexion, 14);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	nuevosTripus(conexion, 25);

//ESCRIBIR EN BITACOS
sleep(5);
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	char* s = string_new();
	string_append(&s, "Me muevo a la posicion 1|2");
	mandarInfoBitacora(conexion, 0, s);

	char* s1 = string_new();
	string_append(&s1, "Me muevo a la posicion 2|2");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s1);

	char* s2 = string_new();
	string_append(&s2, "Me muevo a la posicion 2|3");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s2);

	char* s3 = string_new();
	string_append(&s3, "Me muevo a la posicion 3|3");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s3);

	char* s4 = string_new();
	string_append(&s4, "Me muevo a la posicion 4|3");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s4);

	char* s5 = string_new();
	string_append(&s5, "Me muevo a la posicion 4|4");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s5);

	char* s6 = string_new();
	string_append(&s6, "Me muevo a la posicion 1|4");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s6);

	char* s7 = string_new();
	string_append(&s7, "Me muevo a la posicion 2|3");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s7);

	char* s8 = string_new();
	string_append(&s8, "Me muevo a la posicion 5|3");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s8);

	char* s9 = string_new();
	string_append(&s9, "Me muevo a la posicion 2|1");
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	mandarInfoBitacora(conexion, 0, s9);

//PEDIR BITACORAS
sleep(5);
	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
	pedirBitacora(conexion, 0);

//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	pedirBitacora(conexion, 2);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	pedirBitacora(conexion, 8);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	pedirBitacora(conexion, 14);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	pedirBitacora(conexion, 25);
//
//
//HACER TAREAS
//sleep(2);
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	hacerTarea(conexion, 123, GENERAR_OXIGENO);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	hacerTarea(conexion, 84, GENERAR_COMIDA);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	hacerTarea(conexion, 12, GENERAR_BASURA);
//
//sleep(5);
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	hacerTarea(conexion, 55, CONSUMIR_OXIGENO);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	hacerTarea(conexion, 93, CONSUMIR_COMIDA);
//
//	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);
//	hacerTarea(conexion, 0, DESCARTAR_BASURA);


	log_destroy(logDiscordiador);
	return EXIT_SUCCESS;

}




