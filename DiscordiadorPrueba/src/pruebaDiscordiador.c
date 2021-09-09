/*
 ============================================================================
 Name        : Modulo_Discordiador.c
 Author      : DreamTeam
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "pruebaDiscordiador.h"

t_log * logDiscordiador;

/*int main(void){
	int conexion;
	char* ip = "127.0.0.1";
	char* puerto = "5001";

	logDiscordiador= log_create("discordiador.log", "Discordiador", 1, LOG_LEVEL_INFO);
	log_info(logDiscordiador, "INICIANDO LOG...");

	configDiscordiador = config_create("/home/utnso/tp-2021-1c-DreamTeam/Modulo_Discordiador/discordiador.config");
	log_info(logDiscordiador, "CREANDO CONFIG...");

	if (configDiscordiador == NULL) {
		log_error(logDiscordiador, "NO SE PUDO CREAR EL CONFIG...");
		exit (-1);
	}

	infoConfig = obtenerInfoConfigMiRAMHQ();

	conexion = crearConexionDiscordiador(ip, puerto, logDiscordiador);

	mandarMensaje(conexion);

	log_destroy(logDiscordiador);
	config_destroy(configDiscordiador);
	return EXIT_SUCCESS;
}

infoConfigPrueba obtenerInfoConfigMiRAMHQ() {
	infoConfigPrueba info;

	info.ALGORITMO = config_get_string_value(configDiscordiador, "ALGORITMO");
	info.DURACION_SABOTAJE = atoi(config_get_string_value(configDiscordiador, "DURACION_SABOTAJE"));
	info.GRADO_MULTITAREA = atoi(config_get_string_value(configDiscordiador, "GRADO_MULTITAREA"));
	info.IP_I_MONGO_STORE = config_get_string_value(configDiscordiador, "IP_I_MONGO_STORE");
	info.IP_MI_RAM_HQ = config_get_string_value(configDiscordiador, "IP_MI_RAM_HQ");
	info.PUERTO_I_MONGO_STORE = config_get_string_value(configDiscordiador, "PUERTO_I_MONGO_STORE");
	info.PUERTO_MI_RAM_HQ = config_get_string_value(configDiscordiador, "PUERTO_MI_RAM_HQ");
	info.QUANTUM = atoi(config_get_string_value(configDiscordiador, "QUANTUM"));
	info.RETARDO_CICLO_CPU = atoi(config_get_string_value(configDiscordiador, "RETARDO_CICLO_CPU"));

	return info;
}

int crearConexionDiscordiador(char* ip, char* puerto, t_log* logDiscordiador)
	{
	  	struct addrinfo hints;
	    struct addrinfo *server_info;
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
	        log_error(logDiscordiador, "FALLÓ CONEXIÓN ENTRE DISCORDIADOR y MI RAM HQ...");
	        exit(-1);
	    }

	    log_info(logDiscordiador, "CONEXIÓN EXITOSA...");

	    freeaddrinfo(server_info);
	    return clientSocket;
}


void mandarMensaje(int conexion){

	iniciarPatota patota;
	int a=1, b=2;

	patota.id= INICIAR_PATOTA;
	patota.cantTripulantes = 1;
	patota.largoTareas = string_length(patota.tareas);
	patota.tareas = "GENERAR OXIGENO";
	patota.posiciones = list_create();
	list_add(patota.posiciones, &a);
	list_add(patota.posiciones, &b);

	void* buffer;
	int desplazamiento = 0;

	buffer = malloc(sizeof(patota));

	memcpy(buffer + desplazamiento, &(patota.id), sizeof(ID_MENSAJE));
	desplazamiento += sizeof(ID_MENSAJE);

	memcpy(buffer + desplazamiento, &(patota.cantTripulantes), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(buffer + desplazamiento, &(patota.largoTareas), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(buffer + desplazamiento, patota.tareas, sizeof(char) * patota.largoTareas);
	desplazamiento += sizeof(char) * patota.largoTareas;

	for(int i=0; i<patota.cantTripulantes; i++){
		void* valor = list_get(patota.posiciones, i);
		memcpy(buffer + desplazamiento, valor, sizeof(int));//TODO CAMBIAR ESE INT POR EL VALOR QUE REALMENTE SEA
		desplazamiento += sizeof(int);						//NOSE CUAL ES EL VALOR DE LAS POSICIONES SI INT O UN CHAR*
	}

	int bytesAMandar = sizeof(patota);
	send(conexion, &bytesAMandar, sizeof(int), 0);
	send(conexion, buffer, sizeof(patota), 0);

}*/

typedef struct {
	uint32_t tid;
	uint32_t posicionX;
	uint32_t posicionY;
	uint32_t proximaInstruccion;
	uint32_t* punteroPCB;
} tripulanteControlBlock;

//fisica = logica + offset
typedef struct {
	uint32_t inicio;//dir logica // - Revisar que este correctamente reservada la memoria para este direccion
	uint32_t tamanio;//offset
	char tipo; //P, I(tareas), T
	uint32_t idEstructura;
} segmentacion;



int iniciarPatotaMemoria (Patota* patota) {
//	SEND
	int conexion ;//= conexionConMemoria();
	ID_MENSAJE mensaje=INICIAR_PATOTA;
	int size, i, largoTarea, x, y;
	int desplazamiento=0;
	char* tarea;
	Task* task;
	Tripulante* tripulante;
	int cantTripulantes=patota->cantidadTripulantes;
	int cantidadTareas = list_size(patota->tareas);
	size=sizeof(ID_MENSAJE)+sizeof(int)*2+sizeof(int)*cantTripulantes*2;
	for(i=0;i<cantidadTareas;i++) {
		task=list_get(patota->tareas,i);
		tarea=task->tareaEscrita;
		largoTarea = strlen(tarea)+1;
		size+=sizeof(int);
		size+=largoTarea;
	}
	void* buffer=malloc(size);
	memcpy(buffer+desplazamiento,&mensaje,sizeof(ID_MENSAJE));
	desplazamiento+=sizeof(ID_MENSAJE);
	memcpy(buffer+desplazamiento,&cantTripulantes,sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(buffer+desplazamiento,&cantidadTareas,sizeof(int));
	desplazamiento+=sizeof(int);
	for (i=0; i<cantidadTareas; i++) {
		task=list_get(patota->tareas,i);
		tarea=task->tareaEscrita;
		largoTarea = strlen(tarea)+1;
		memcpy(buffer+desplazamiento,&largoTarea,sizeof(int));
		desplazamiento+=sizeof(int);
		memcpy(buffer+desplazamiento,tarea,largoTarea);
		desplazamiento+=largoTarea;
	}
	for (i=0; i<cantTripulantes; i++) {
		tripulante = list_get(patota->tripulantes,i);
		x=tripulante->posX;
		y=tripulante->posY;
		memcpy(buffer+desplazamiento,&x,sizeof(int));
		desplazamiento+=sizeof(int);
		memcpy(buffer+desplazamiento,&y,sizeof(int));
		desplazamiento+=sizeof(int);
	}
//	pthread_mutex_lock(&mutexMemoria);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
	free(buffer);

//	RECV (UN INT NOMAS, 0 SI SALIO BIEN, 1 SI SALIO MAL)
	int huboProblema=6;
	int aux;
	buffer = malloc(sizeof(int)*cantTripulantes);
	recv(conexion, &huboProblema, sizeof(int), 0);
	if (huboProblema==0) {
		recv(conexion, buffer, sizeof(int)*cantTripulantes,0);
		for (i=0;i<cantTripulantes;i++) {
			memcpy(&aux,buffer+sizeof(int)*i,sizeof(int));
			tripulante = list_get(patota->tripulantes,i);
			tripulante->tripulanteID=aux;
		}
	}
	free(buffer);
//	pthread_mutex_unlock(&mutexMemoria);
	close(conexion);
	return huboProblema;
}














int main() {

	Patota patota;
	patota.patotaID = 5;
	patota.cantidadTripulantes = 4;
	patota.tareas;
	patota.tripulantes;



	/*
	 * Segmentación
			Id del Proceso (patota)
			# de segmento
			Dirección de inicio
			Tamaño del segmento
	 */
	void* inicio = malloc(sizeof(64));

	segmentacion* segmentacion = malloc(sizeof(segmentacion));
	segmentacion->idEstructura = 1;
	segmentacion->inicio = 2;

	memcpy(inicio, &segmentacion, sizeof(segmentacion));

	char* pathDump = string_new();

	char* horaActual = temporal_get_string_time("%H:%M:%S:%MS");
	char* fechaActual = temporal_get_string_time("%d/%m/%y %H:%M:%S");

	string_append(&pathDump, "Dump_");
	string_append(&pathDump, horaActual);
	string_append(&pathDump, ".dmp");

	FILE* dumpFile = fopen(pathDump , "w");

	char* formato = string_new();
	string_append(&formato, "Dump: ");
	string_append(&formato, fechaActual);

	int tamanioFormato = string_length(formato);
	fwrite(&formato, tamanioFormato, 1, dumpFile);

	//mem_hexdump(inicio, sizeof(segmentacion));

	free(inicio);
	free(segmentacion);
//	free(formato);
//	free(pathDump);

	return 0;
}









