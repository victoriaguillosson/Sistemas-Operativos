/*
 * mensajes.c
 *
 *  Created on: 7 may. 2021
 *      Author: utnso
 */

#include "mensajes.h"

void iniciarServidor() {  // SABOTAJE: INTINT
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(IPDEMISERVER, PUERTODEMISERVER, &hints, &serverInfo);

	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	listen(listenningSocket, 1);
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	clienteDeMiServidor = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	bufferServer = malloc(sizeof(int)*2);
	int status = 1;

	while (status != 0){
		status = recv(clienteDeMiServidor, (void*) bufferServer, sizeof(int)*2, 0);
		if (status != 0) alertaDeSabotaje(bufferServer);
	}

	close(listenningSocket);
}

void alertaDeSabotaje (void* buffer){
	int x,y;
	memcpy(&x,buffer,sizeof(int));
	memcpy(&y,buffer+sizeof(int),sizeof(int));
	printf("ALERTA, SABOTAJE EN %d|%d\n",x,y);
	pthread_mutex_lock(&mutexLog);
	log_info(logDiscordiador,"Sabotaje en %d|%d",x,y);
	pthread_mutex_unlock(&mutexLog);
	char* stringX = string_itoa(x);
	char* stringY = string_itoa(y);
	string_append(&stringX," ");
	string_append(&stringX,stringY);
	procedimientoSabotaje(stringX);
	free(stringY);
}

char* pedirBitacora(int tripulante) {
//	SEND (ID_MENSAJE ID_TRIPULANTE)
	int conexion = conexionConFileSystem();
	ID_MENSAJE mensaje=ENVIAR_BITACORA;
	int size = sizeof(ID_MENSAJE)+sizeof(int);
	void* buffer = malloc(sizeof(ID_MENSAJE)+sizeof(int));
	memcpy(buffer,&mensaje,sizeof(ID_MENSAJE));
	memcpy(buffer+sizeof(ID_MENSAJE),&tripulante,sizeof(int));
	pthread_mutex_lock(&mutexFileSystem);
	send(conexion, &size, sizeof(int), 0);
	send(conexion, buffer, size, 0);
	free(buffer);

//	RECV (LA BITACORA EN UN CHAR*)
	int tamanioBitacora;
	recv(conexion, &tamanioBitacora, sizeof(int), 0);
	char* bitacora=malloc(tamanioBitacora);
	recv(conexion, bitacora, tamanioBitacora, 0);
	pthread_mutex_unlock(&mutexFileSystem);
	close(conexion);
	return bitacora;
}

int iniciarPatotaMemoria (Patota* patota) {
//	SEND
	int conexion = conexionConMemoria();
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
	pthread_mutex_lock(&mutexMemoria);
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
	pthread_mutex_unlock(&mutexMemoria);
	close(conexion);
	return huboProblema;
}

Task solicitarTarea(int tripulante) {
//	SEND
	int conexion = conexionConMemoria();
	ID_MENSAJE mensaje = SOLICITAR_TAREA;
	int size = sizeof(ID_MENSAJE) + sizeof(int);
	void* buffer = malloc(size);
	memcpy(buffer,&mensaje,sizeof(ID_MENSAJE));
	memcpy(buffer+sizeof(ID_MENSAJE),&tripulante,sizeof(int));
	pthread_mutex_lock(&mutexMemoria);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
	free(buffer);

//	RECV (LA SIGUIENTE TAREA EN UN CHAR*) (SI NO HAY MAS TAREAS, MANDARME UN CHAR QUE DIGA "FIN")
	Task task;
	int tamanioTarea;
	recv(conexion, &tamanioTarea, sizeof(int), 0);
	char* tarea = malloc(tamanioTarea);
	recv(conexion, tarea, tamanioTarea, 0);
	pthread_mutex_unlock(&mutexMemoria);
	task=tareaToTask(tarea);
	close(conexion);
	return task;
}

Task tareaToTask (char* tarea) {
	Task task;
	if (!strcmp(tarea,"FIN")) {task.tiempo=0; free(tarea);} else {
		char** aux = string_n_split(tarea,4,";");  // "TAREA PARAM;X;Y;T" - "TAREA;X;Y;T"
		char** aux2 = string_n_split(aux[0],2," ");
		task.tarea=aux2[0];
		if (aux2[1]!=NULL) {task.parametro=atoi(aux2[1]); free(aux2[1]);}
		else task.parametro = 9999;
		task.posX=atoi(aux[1]);
		task.posY=atoi(aux[2]);
		task.tiempo=atoi(aux[3]);
		task.tareaEscrita=tarea;
		if (task.parametro!=9999) task = asignarIdTarea(task);
		free(aux[0]); free(aux[1]); free(aux[2]); free(aux[3]); free(aux); free(aux2);
	}
	return task;
}

Task asignarIdTarea (Task tarea) {
	if (!strcmp(tarea.tarea,"GENERAR_OXIGENO")) tarea.id=GENERAR_OXIGENO;
	else if (!strcmp(tarea.tarea,"CONSUMIR_OXIGENO")) tarea.id=CONSUMIR_OXIGENO;
	else if (!strcmp(tarea.tarea,"GENERAR_COMIDA")) tarea.id=GENERAR_COMIDA;
	else if (!strcmp(tarea.tarea,"CONSUMIR_COMIDA")) tarea.id=CONSUMIR_COMIDA;
	else if (!strcmp(tarea.tarea,"GENERAR_BASURA")) tarea.id=GENERAR_BASURA;
	else if (!strcmp(tarea.tarea,"DESCARTAR_BASURA")) tarea.id=DESCARTAR_BASURA;
	return tarea;
}

Task* asignarIdTask (Task* tarea) {
	if (!strcmp(tarea->tarea,"GENERAR_OXIGENO")) tarea->id=GENERAR_OXIGENO;
	else if (!strcmp(tarea->tarea,"CONSUMIR_OXIGENO")) tarea->id=CONSUMIR_OXIGENO;
	else if (!strcmp(tarea->tarea,"GENERAR_COMIDA")) tarea->id=GENERAR_COMIDA;
	else if (!strcmp(tarea->tarea,"CONSUMIR_COMIDA")) tarea->id=CONSUMIR_COMIDA;
	else if (!strcmp(tarea->tarea,"GENERAR_BASURA")) tarea->id=GENERAR_BASURA;
	else if (!strcmp(tarea->tarea,"DESCARTAR_BASURA")) tarea->id=DESCARTAR_BASURA;
	return tarea;
}

void mensajeExpulsarTripulante (int tripulante){
//	SEND
	int conexion = conexionConMemoria();
	ID_MENSAJE mensaje = EXPULSAR_TRIPULANTE;
	int size = sizeof(ID_MENSAJE) + sizeof(int);
	void* buffer = malloc(size);
	memcpy(buffer,&mensaje,sizeof(ID_MENSAJE));
	memcpy(buffer+sizeof(ID_MENSAJE),&tripulante,sizeof(int));
	pthread_mutex_lock(&mutexMemoria);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
	pthread_mutex_unlock(&mutexMemoria);
	free(buffer);
	close(conexion);
//	NO HAY RECV
}

void nuevosTripulantes (Patota* patota) {
//	SEND
	int conexion = conexionConFileSystem();
	ID_MENSAJE mensaje = NUEVOS_TRIPULANTES;
	int desplazamiento=0;
	int i, id;
	int cantidadTripulantes = patota->cantidadTripulantes;
	Tripulante* tripulante;
	int size = sizeof(ID_MENSAJE) + sizeof(int) + cantidadTripulantes * sizeof(int);
	void* buffer = malloc(size);
	memcpy(buffer+desplazamiento,&mensaje,sizeof(ID_MENSAJE));
	desplazamiento+=sizeof(ID_MENSAJE);
	memcpy(buffer+desplazamiento,&cantidadTripulantes,sizeof(int));
	desplazamiento+=sizeof(int);
	for (i=0; i<cantidadTripulantes; i++) {
		tripulante=list_get(patota->tripulantes,i);
		id=tripulante->tripulanteID;
		memcpy(buffer+desplazamiento,&id,sizeof(int));
		desplazamiento+=sizeof(int);
	}
	pthread_mutex_lock(&mutexFileSystem);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
	pthread_mutex_unlock(&mutexFileSystem);
	free(buffer);
	close(conexion);

//	NO HAY RECV
}

void infoParaBitacora (int tripulante, char* bitacora) {
//	SEND A FILESYSTEM
	int conexion = conexionConFileSystem();
	ID_MENSAJE mensaje = INFO_BITACORA;
	int desplazamiento=0;
	int largoBitacora = string_length(bitacora)+1;
	int size = sizeof(ID_MENSAJE) + sizeof(int)*2 + largoBitacora;
	void* buffer = malloc(size);
	memcpy(buffer+desplazamiento,&mensaje,sizeof(ID_MENSAJE));
	desplazamiento+=sizeof(ID_MENSAJE);
	memcpy(buffer+desplazamiento,&tripulante,sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(buffer+desplazamiento,&largoBitacora,sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(buffer+desplazamiento,bitacora,largoBitacora);
	desplazamiento+=largoBitacora;
//	pthread_mutex_lock(&mutexFileSystem);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
//	pthread_mutex_unlock(&mutexFileSystem);
	free(buffer);
	close(conexion);
//	NO HAY RECV
}

void informarMovimiento (int tripulante, int x, int y, int x0, int y0) {
//	SEND A MEMORIA
	int conexion = conexionConMemoria();
	ID_MENSAJE mensaje = INFORMAR_MOVIMIENTO;
	int desplazamiento=0;
	int size = sizeof(ID_MENSAJE) + sizeof(int)*3;
	void* buffer = malloc(size);
	memcpy(buffer+desplazamiento,&mensaje,sizeof(ID_MENSAJE));
	desplazamiento+=sizeof(ID_MENSAJE);
	memcpy(buffer+desplazamiento,&tripulante,sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(buffer+desplazamiento,&x,sizeof(int));
	desplazamiento+=sizeof(int);
	memcpy(buffer+desplazamiento,&y,sizeof(int));
	desplazamiento+=sizeof(int);
	pthread_mutex_lock(&mutexMemoria);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
	pthread_mutex_unlock(&mutexMemoria);
	free(buffer);

//	SEND A FILESYSTEM
	char* bitacora = string_new();
	char* nuevaX = string_itoa(x);
	char* nuevaY = string_itoa(y);
	char* viejaX = string_itoa(x0);
	char* viejaY = string_itoa(y0);
	string_append(&bitacora, "Me movi desde ");
	string_append(&bitacora, viejaX);
	string_append(&bitacora, "|");
	string_append(&bitacora, viejaY);
	string_append(&bitacora, " hacia ");
	string_append(&bitacora, nuevaX);
	string_append(&bitacora, "|");
	string_append(&bitacora, nuevaY);
	string_append(&bitacora, "\n");
	infoParaBitacora(tripulante, bitacora);
	free(nuevaX);
	free(nuevaY);
	free(viejaX);
	free(viejaY);
	free(bitacora);

//	NO HAY RECV
//	int asd;
//	recv(conexion, &asd, sizeof(int), 0);
	close(conexion);
}

void avisarComienzoFSCK (int flag) {
	send(clienteDeMiServidor,&flag, sizeof(int), 0);
}

void hacerTarea(Task tarea) {
//	SEND
	int conexion = conexionConFileSystem();
	ID_MENSAJE mensaje = HACER_TAREA;
	int desplazamiento=0;
	int parametro = tarea.parametro;
	IdTareas idTarea = tarea.id;
	int size = sizeof(ID_MENSAJE) + sizeof(IdTareas) + sizeof(int);
	void* buffer = malloc(size);
	memcpy(buffer+desplazamiento,&mensaje,sizeof(ID_MENSAJE));
	desplazamiento+=sizeof(ID_MENSAJE);
	memcpy(buffer+desplazamiento,&idTarea,sizeof(IdTareas));
	desplazamiento+=sizeof(IdTareas);
	memcpy(buffer+desplazamiento,&parametro,sizeof(int));
	desplazamiento+=sizeof(int);
	pthread_mutex_lock(&mutexFileSystem);
	send(conexion,&size, sizeof(int), 0);
	send(conexion,buffer, size, 0);
	pthread_mutex_unlock(&mutexFileSystem);
	free(buffer);
	close(conexion);

// NO HAY RECV
}

