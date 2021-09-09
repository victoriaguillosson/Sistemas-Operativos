/*
 ============================================================================
 Name        : Modulo_Discordiador.c
 Author      : DreamTeam
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Modulo_Discordiador.h"

int main(void){

	char ** commandAux;
	char* lectura;
	int commandValue;
	int i=-1;
	int salir=0;

	configDiscordiador = config_create("/home/utnso/tp-2021-1c-DreamTeam/Modulo_Discordiador/discordiador.config");
	llenarConfig(configDiscordiador);
	crearUtilidades();
	fillDictionary(consoleCommandDictionary);

	pthread_mutex_lock(&mutexLog);
	log_info(logDiscordiador, "INICIANDO LOG...");
	log_info(logDiscordiador, "CREANDO CONFIG...");
	pthread_mutex_unlock(&mutexLog);

	if (configDiscordiador == NULL) {
		pthread_mutex_lock(&mutexLog);
		log_error(logDiscordiador, "NO SE PUDO CREAR EL CONFIG...");
		pthread_mutex_unlock(&mutexLog);
		exit (-1);
	}

//	clienteConMemoria = crearConexionDiscordiador(config.IP_MI_RAM_HQ, config.PUERTO_MI_RAM_HQ, logDiscordiador);
	if (pthread_create(&hiloServidor, NULL, &iniciarServidor, NULL) != 0) {}
	pthread_detach(hiloServidor);

	pthread_mutex_lock(&mutexLog);
	log_info(logDiscordiador, "Estoy a la espera de recibir comandos por consola");
	pthread_mutex_unlock(&mutexLog);

	printf("Ingresar comando\n");
	while(!salir){
		lectura=readline("");
		commandAux = string_n_split(lectura, 2, " ");
		i++;
		commandValue = dictionary_get( consoleCommandDictionary, commandAux[0] );
		if(commandValue==NULL)
			commandValue=-1;
		switch(commandValue)
		{
			case SALIR:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando SALIR");
				pthread_mutex_unlock(&mutexLog);
				salir=1;
				break;
			case INICIAR_PATOTA_COMANDO:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando INICIAR_PATOTA %s",commandAux[1]);
				pthread_mutex_unlock(&mutexLog);
				iniciarPatota(commandAux[1]);
				break;
			case LISTAR_TRIPULANTES:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando LISTAR_TRIPULANTES");
				pthread_mutex_unlock(&mutexLog);
				listarTripulantes();
				break;
			case EXPULSAR_TRIPULANTE_COMANDO:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando EXPULSAR_TRIPULANTE %s",commandAux[1]);
				pthread_mutex_unlock(&mutexLog);
				expulsarTripulante(commandAux[1]);
				break;
			case INICIAR_PLANIFICACION:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando INICIAR_PLANIFICACION");
				pthread_mutex_unlock(&mutexLog);
				iniciarPlanificacion();
				break;
			case PAUSAR_PLANIFICACION:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando PAUSAR_PLANIFICACION");
				pthread_mutex_unlock(&mutexLog);
				pausarPlanificacion();
				break;
			case OBTENER_BITACORA:
				pthread_mutex_lock(&mutexLog);
				log_info(logDiscordiador, "Se ingreso el comando OBTENER_BITACORA %s",commandAux[1]);
				pthread_mutex_unlock(&mutexLog);
				obtenerBitacora(commandAux[1]);
				break;
			case CONECTAR_FS:
//				clienteConFileSystem = crearConexionDiscordiador(config.IP_I_MONGO_STORE, config.PUERTO_I_MONGO_STORE, logDiscordiador);
				break;
			default: printf("Comando invalido\n"); free(commandAux[1]);
		}
		free(commandAux[0]); free(commandAux); free(lectura);
	}
	liberarTodo();
	printf("Soy el main finalizando\n");
	return EXIT_SUCCESS;
}

void llenarConfig (t_config* configDiscordiador) {
	config.IP_I_MONGO_STORE = config_get_string_value(configDiscordiador, "IP_I_MONGO_STORE");
	config.PUERTO_I_MONGO_STORE = config_get_string_value(configDiscordiador, "PUERTO_I_MONGO_STORE");
	config.IP_MI_RAM_HQ = config_get_string_value(configDiscordiador, "IP_MI_RAM_HQ");
	config.PUERTO_MI_RAM_HQ = config_get_string_value(configDiscordiador, "PUERTO_MI_RAM_HQ");
	config.ALGORITMO = config_get_string_value(configDiscordiador, "ALGORITMO");
	config.DURACION_SABOTAJE = atoi(config_get_string_value(configDiscordiador, "DURACION_SABOTAJE"));
	config.GRADO_MULTITAREA = atoi(config_get_string_value(configDiscordiador, "GRADO_MULTITAREA"));
	config.QUANTUM = atoi(config_get_string_value(configDiscordiador, "QUANTUM"));
	config.RETARDO_CICLO_CPU = atoi(config_get_string_value(configDiscordiador, "RETARDO_CICLO_CPU"));
}

void fillDictionary(t_dictionary * dictionary)
{
	dictionary_put(dictionary, "INICIAR_PATOTA", (void *)INICIAR_PATOTA_COMANDO);
	dictionary_put(dictionary, "LISTAR_TRIPULANTES", (void *)LISTAR_TRIPULANTES);
	dictionary_put(dictionary, "EXPULSAR_TRIPULANTE", (void *)EXPULSAR_TRIPULANTE_COMANDO);
	dictionary_put(dictionary, "INICIAR_PLANIFICACION", (void *)INICIAR_PLANIFICACION);
	dictionary_put(dictionary, "PAUSAR_PLANIFICACION", (void *)PAUSAR_PLANIFICACION);
	dictionary_put(dictionary, "OBTENER_BITACORA", (void *)OBTENER_BITACORA);
	dictionary_put(dictionary, "SALIR", (void *)SALIR);
	dictionary_put(dictionary, "CONECTAR_FS", (void*) CONECTAR_FS);
}

int iniciarPatota(char * parameters){
	char** splitParameters = string_n_split(parameters,3, " ");
	free(parameters);
	int huboProblema=0;
	errorNew=0;
	Patota* patota = malloc(sizeof(Patota));
	cargarCantidadTripulantes (patota, splitParameters[0]);
	cargarTareas(patota, splitParameters[1]);
	iniciarTripulantes(patota, splitParameters[2]);
	huboProblema = iniciarPatotaMemoria(patota);
	if (huboProblema==1) {
		errorNew=1;
		printf("Hubo un problema con la carga de los tripulantes\n");
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Hubo un problema con la carga de los tripulantes");
		pthread_mutex_unlock(&mutexLog);
	}
	else {
		patota->patotaID=contadorPatotas;
		contadorPatotas++;
		list_add(patotas,patota);
		nuevosTripulantes(patota);
		printf("Los tripulantes estan listos para trabajar\n");
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Los tripulantes estan listos para trabajar");
		pthread_mutex_unlock(&mutexLog);
	}
	desbloquearTripulantesNew(patota);
	if(huboProblema==1) destruirPatota(patota);
	free(splitParameters);
	return 0;
}

void cargarCantidadTripulantes(Patota * patota, char * cantidadTripulantes)
{
	patota->cantidadTripulantes= atoi(cantidadTripulantes);
	free(cantidadTripulantes);
}

void cargarTareas(Patota * patota, char * rutaArchivoTareas)
{
	// formato de tarea: TAREA PARAMETRO;POS X;POS Y;TIEMPO    ejemplo:  CONTARHASTA 10;2;3;10
	ssize_t lectura;
	size_t largo = 0;
	char *linea = NULL;
	int i;
	Task *task;
	char* tareaEscrita;
	FILE* archivoTareas = fopen(rutaArchivoTareas,"r");
	patota->tareas = list_create();
	if (!archivoTareas) {
		printf("No existe el archivo '%s'\n", rutaArchivoTareas);
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "No existe el archivo de tareas %s",rutaArchivoTareas);
		pthread_mutex_unlock(&mutexLog);
	}
	else {
		char **auxTarea;
		char **auxParametro;
		lectura = getline(&linea, &largo, archivoTareas);
		while (lectura != -1) {
			linea[strcspn(linea, "\n")] = 0;
			task = malloc(sizeof(Task));
			tareaEscrita=string_new();
			auxTarea = string_n_split(linea, 4, ";");  //[TAREA PARAMETRO; POSX; POSY; TIEMPO]
			auxParametro = string_n_split(auxTarea[0], 2, " "); //[TAREA; PARAMETRO]
			string_append(&tareaEscrita,auxParametro[0]);
//			string_append(&tareaEscrita,";");
			task->tarea = auxParametro[0];
			if (auxParametro[1]!=NULL) {
				task->parametro = atoi(auxParametro[1]);
				string_append(&tareaEscrita," ");
				string_append(&tareaEscrita,auxParametro[1]);
			}
			else task->parametro = 9999; //string_append(&tareaEscrita,"9999");
			task->posX = atoi(auxTarea[1]);
			task->posY = atoi(auxTarea[2]);
			task->tiempo = atoi(auxTarea[3]);
			string_append(&tareaEscrita,";");
			string_append(&tareaEscrita,auxTarea[1]);
			string_append(&tareaEscrita,";");
			string_append(&tareaEscrita,auxTarea[2]);
			string_append(&tareaEscrita,";");
			string_append(&tareaEscrita,auxTarea[3]);
			task->tareaEscrita=tareaEscrita;
			if (task->parametro!=9999) task=asignarIdTask(task);
			list_add(patota->tareas, task);
			for(i=0;i<4;i++) free(auxTarea[i]); free(auxTarea);
			for(i=0;i<2;i++) free(auxParametro[i]); free(auxParametro);
			lectura = getline(&linea, &largo, archivoTareas);
		}
	}
	free(linea);
	fclose(archivoTareas);
	free(rutaArchivoTareas);
}

void iniciarTripulantes(Patota * patota, char * posiciones)
{
	Tripulante* tripulante;
	char** auxPosiciones;
	char** parPosicion;
	char* hola = "0 0";
	patota->tripulantes=list_create();
	int cantidadPosiciones=0, i, j;
	if (posiciones!=NULL) {
		auxPosiciones = string_n_split(posiciones,50," ");
		for (j=0; j<100; j++) {
				if (auxPosiciones[j] == NULL) {cantidadPosiciones=j; j+=100;}
			}
	}
	else parPosicion=string_n_split(hola,2," ");
	pthread_t th[patota->cantidadTripulantes];

	for (i = 0; i < patota->cantidadTripulantes; i++) {
	    tripulante = malloc(sizeof(Tripulante));
	    if (cantidadPosiciones == i && i>0) {free(parPosicion[0]); free(parPosicion[1]);parPosicion[0]="0"; parPosicion[1]="0";}
	    else if (cantidadPosiciones > i) parPosicion=string_n_split(auxPosiciones[i],2,"|");
	    tripulante->estadoTripulante=NEW;
	    tripulante->patotaID=patota->patotaID;
	    tripulante->posX=atoi(parPosicion[0]);
	    tripulante->posY=atoi(parPosicion[1]);
	    tripulante->tripulanteID=9999;
	    tripulante->durmiendo=0;
	    sem_init(&(tripulante->semExec),0,0);
	    sem_init(&(tripulante->semIO),0,0);
	    sem_init(&(tripulante->semEM),0,0);
	    sem_init(&(tripulante->semNew),0,0);
//	    tripulante->socketMemoria=crearConexionDiscordiador(config.IP_MI_RAM_HQ, config.PUERTO_MI_RAM_HQ, logDiscordiador);
//	    tripulante->socketFileSystem=crearConexionDiscordiador(config.IP_I_MONGO_STORE, config.PUERTO_I_MONGO_STORE, logDiscordiador);
	    contadorTripulantes++;
	    list_add (patota->tripulantes, tripulante);
	    pthread_mutex_lock(&mutexColaNew);
	    list_add (colaNew, tripulante);
	    pthread_mutex_unlock(&mutexColaNew);
		if (pthread_create(&th[i], NULL, &startThread, tripulante) != 0) {
			log_error(logDiscordiador, "Failed to create the thread");
	    }
		pthread_detach(th[i]);
		if(cantidadPosiciones>i+1) {free(parPosicion[0]); free(parPosicion[1]); free(parPosicion);}
		if(cantidadPosiciones==patota->cantidadTripulantes && cantidadPosiciones==i+1)
			{free(parPosicion[0]); free(parPosicion[1]); free(parPosicion);}
	}
	for (i=0; i<cantidadPosiciones; i++) free(auxPosiciones[i]);
	if (posiciones!=NULL) free(auxPosiciones);
	else {free(parPosicion[0]); free(parPosicion[1]);}
	if (cantidadPosiciones<patota->cantidadTripulantes) free(parPosicion); //ok
	free(posiciones);
}

void crearUtilidades() {
	haySabotaje=0;
	primeraVez=0;
	finDePrograma=0;
	primerConexionMemoria=0;
	primerConexionFS=0;
	cantHilos=0;
	consoleCommandDictionary = dictionary_create();
	logDiscordiador= log_create("discordiador.log", "Discordiador", 0, LOG_LEVEL_INFO);
	pthread_mutex_lock(&mutexLog);
	log_info(logDiscordiador, "-----------------------------------------------------------");
	pthread_mutex_unlock(&mutexLog);
	patotas = list_create();
	colaNew = list_create();
	colaReady = list_create();
	colaExec = list_create();
	colaExit = list_create();
	colaBlockedIO = list_create();
	colaBlockedEM = list_create();
	restaurarSemaforo = list_create();
	pthread_mutex_init(&mutexColaExec, NULL);
	pthread_mutex_init(&mutexColaBlockIO, NULL);
	pthread_mutex_init(&mutexColaReady, NULL);
	pthread_mutex_init(&mutexColaNew, NULL);
	pthread_mutex_init(&mutexMemoria, NULL);
	pthread_mutex_init(&mutexFileSystem, NULL);
	pthread_mutex_init(&mutexQuantum, NULL);
	pthread_mutex_init(&mutexLog, NULL);
	sem_init(&hayLugarEnExec,1,config.GRADO_MULTITAREA);
	sem_init(&hayAlguienEnReady,1,0);
	sem_init(&alertaQuantum,1,0);
	sem_init(&siguienteIO,1,1);
	sem_init(&hayAlguienEnIO,1,0);
	sem_init(&semaforoNew,1,0);
	sem_init(&semPlanificando,1,0);
	sem_init(&turnosQuantum,1,1);
	sem_init(&finDeHilos,1,1);
	sem_init(&semLogCambioCola,1,1);
}

void liberarTodo() {
	matarHilosPlanificadores();
	pthread_cancel(hiloServidor);
	config_destroy(configDiscordiador);
	dictionary_destroy(consoleCommandDictionary);
	pthread_mutex_destroy(&mutexColaExec);
	pthread_mutex_destroy(&mutexColaBlockIO);
	pthread_mutex_destroy(&mutexColaReady);
	pthread_mutex_destroy(&mutexColaNew);
	pthread_mutex_destroy(&mutexMemoria);
	pthread_mutex_destroy(&mutexFileSystem);
	pthread_mutex_destroy(&mutexQuantum);
	pthread_mutex_destroy(&mutexLog);
	log_destroy(logDiscordiador);
	free(bufferServer);
	list_destroy_and_destroy_elements(colaNew,destruirTripulante);
	list_destroy_and_destroy_elements(colaReady,destruirTripulante);
	list_destroy_and_destroy_elements(colaExec,destruirTripulante);
	list_destroy_and_destroy_elements(colaExit,destruirTripulante);
	list_destroy_and_destroy_elements(colaBlockedIO,destruirTripulante);
	list_destroy_and_destroy_elements(colaBlockedEM,destruirTripulante);
	list_destroy_and_destroy_elements(restaurarSemaforo,destruirSemaforo);
	list_destroy_and_destroy_elements(patotas,destruirPatota);
	sem_destroy(&hayLugarEnExec);
	sem_destroy(&hayAlguienEnReady);
	sem_destroy(&alertaQuantum);
	sem_destroy(&siguienteIO);
	sem_destroy(&hayAlguienEnIO);
	sem_destroy(&semaforoNew);
	sem_destroy(&semPlanificando);
	sem_destroy(&semLogCambioCola);
	sem_destroy(&turnosQuantum);
	close(clienteDeMiServidor);
	close(clienteConMemoria);
	close(clienteConFileSystem);
	sem_wait(&finDeHilos);
	sem_destroy(&finDeHilos);
	usleep(200000);
}

void destruirTripulante(Tripulante* trip) {
	sem_destroy(&(trip->semExec));
	sem_destroy(&(trip->semEM));
	sem_destroy(&(trip->semIO));
	sem_destroy(&(trip->semNew));
//	close(trip->socketFileSystem);
//	close(trip->socketMemoria);
	free(trip);
}

void destruirPatota(Patota* patota) {
	list_destroy_and_destroy_elements(patota->tareas,destruirTarea);
	list_destroy(patota->tripulantes);
	free(patota);
}

void destruirTarea(Task* tarea){
	free(tarea->tareaEscrita);
	free(tarea);
}

void destruirSemaforo(sem_t* semaforo) {
	sem_destroy(semaforo);
}

int listarTripulantes() {
	time_t tiempo = time(NULL);
	struct tm *tm = localtime(&tiempo);
	char diaHora[64];
	int i,j;
	assert(strftime(diaHora, sizeof(diaHora), "%c", tm));
	int cantidadPatotas = list_size(patotas);
	int cantidadTripulantes;
	t_list* tripulantes;
	char* estado;
	Tripulante* tripulante;
	Patota* patotaActual;
	printf("==================================================\n");
	printf("Estado de la nave: %s\n",diaHora);
	for (i=0; i<cantidadPatotas; i++) {
		patotaActual = list_get(patotas,i);
		tripulantes = patotaActual->tripulantes;
		cantidadTripulantes = patotaActual->cantidadTripulantes;
		for (j=0; j<cantidadTripulantes; j++) {
			tripulante = list_get(tripulantes,j);
			switch (tripulante->estadoTripulante) {
				case 0: estado="NEW"; break;
				case 1: estado="READY"; break;
				case 2: estado="EXEC"; break;
				case 3: estado="BLOCKED_IO"; break;
				case 4: estado="BLOCKED_EM"; break;
				case 5: estado="FINISHED"; break;
			}
			printf("Tripulante: %d    Patota: %d    Estado: %s\n", tripulante->tripulanteID, patotaActual->patotaID,estado);
		}
	}
	printf("==================================================\n");
	return 0;
}

int expulsarTripulante (char* parametro) {
	int i,j;
	int id = atoi(parametro);
	free(parametro);
	int encontrado=0;
	int cantidadPatotas = list_size(patotas);
	int cantidadTripulantes;
	t_list* tripulantes;
	Tripulante* tripulante;
	Patota* patotaActual;
	bool encontrarTripulanteEnLista(Tripulante* trip) {
		return (trip->tripulanteID==tripulante->tripulanteID);
	}
	for (i=0; i<cantidadPatotas && !encontrado; i++) {
		patotaActual = list_get(patotas,i);
		tripulantes = patotaActual->tripulantes;
		cantidadTripulantes = patotaActual->cantidadTripulantes;
		for (j=0; j<cantidadTripulantes && !encontrado; j++) {
			tripulante = list_get(tripulantes,j);
			if (tripulante->tripulanteID == id) encontrado=1;
		}
	}
	if (!encontrado) {
		printf("Tripulante %d no encontrado\n",id);
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Tripulante %d no encontrado",id);
		pthread_mutex_unlock(&mutexLog);
	}
	else if (tripulante->estadoTripulante==5) {
		printf("El tripulante %d ya habia sido finalizado",id);
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "El tripulante %d ya habia sido finalizado",id);
		pthread_mutex_unlock(&mutexLog);
	}
	else {
		char* mensaje = "Me expulsaron de la nave...";
		mensajeExpulsarTripulante(id);
		infoParaBitacora(id,mensaje);
		printf("Tripulante %d expulsado de la nave\n",tripulante->tripulanteID);
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Tripulante %d expulsado de la nave", tripulante->tripulanteID);
		pthread_mutex_unlock(&mutexLog);
		pthread_mutex_lock(&mutexColaBlockIO);
		pthread_mutex_lock(&mutexColaReady);
		pthread_mutex_lock(&mutexColaExec);
		switch (tripulante->estadoTripulante) {
			case 0: list_remove_by_condition(colaNew,encontrarTripulanteEnLista); break;
			case 1: list_remove_by_condition(colaReady,encontrarTripulanteEnLista); sem_wait(&hayAlguienEnReady); break;
			case 2: list_remove_by_condition(colaExec,encontrarTripulanteEnLista); sem_post(&hayLugarEnExec); break;
			case 3: list_remove_by_condition(colaBlockedIO,encontrarTripulanteEnLista);
				if (tripulante->estoyFrenadoIO==1) {sem_wait(&hayAlguienEnIO);} else {sem_post(&siguienteIO);} break;
			case 4: list_remove_by_condition(colaBlockedEM,encontrarTripulanteEnLista); break;
			case 5: break;
		}
		tripulante->estadoTripulante=FINISH;
		list_add(colaExit, tripulante);
		pthread_mutex_unlock(&mutexColaExec);
		pthread_mutex_unlock(&mutexColaReady);
		pthread_mutex_unlock(&mutexColaBlockIO);
		sem_post(&tripulante->semIO);
		sem_post(&tripulante->semExec);
		sem_post(&tripulante->semEM);
	}

	return 0;
}

int obtenerBitacora(char* parametro) {
	int id = atoi(parametro);
	int i,j;
	int encontrado=0;
	int cantidadPatotas = list_size(patotas);
	int cantidadTripulantes;
	t_list* tripulantes;
	Tripulante* tripulante;
	Patota* patotaActual;
	for (i=0; i<cantidadPatotas && !encontrado; i++) {
		patotaActual = list_get(patotas,i);
		tripulantes = patotaActual->tripulantes;
		cantidadTripulantes = patotaActual->cantidadTripulantes;
		for (j=0; j<cantidadTripulantes && !encontrado; j++) {
			tripulante = list_get(tripulantes,j);
			if (tripulante->tripulanteID == id) encontrado=1;
		}
	}
	if (!encontrado) printf("Tripulante %d no encontrado\n",id);
	else {
		char* bitacora = "Soy una bitacora";
		char* string = string_new();
		bitacora = pedirBitacora(id);
		string_append(&string, "Bitacora del tripulante ");
		string_append(&string, parametro);
		string_append(&string, ":\n");
		string_append(&string, bitacora);
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, string);
		pthread_mutex_unlock(&mutexLog);
		printf("La bitacora del tripulante %d fue agregada al log\n",id);
		free(bitacora);
		free(string);
	}
	free(parametro);
	return 0;
}

int procedimientoSabotaje(char* posiciones) {
	int cantidadDeTripulantesEnExec=0, i;
	if (haySabotaje) {
		printf("Llego un sabotaje nuevo pero ya hay uno en curso\n");
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Llego un sabotaje nuevo pero ya hay uno en curso");
		pthread_mutex_unlock(&mutexLog);
		avisarComienzoFSCK(0);
	}
	else if(list_size(colaExec)+list_size(colaReady)==0) {
		printf("No hay tripulantes para resolver el sabotaje\n");
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "No hay tripulantes para resolver el sabotaje");
		pthread_mutex_unlock(&mutexLog);
		avisarComienzoFSCK(0);
	}
	else {
		char* mensaje;
		planificando=0;
		distanciaMinimaAlSabotaje=100000;
		char** aux = string_n_split(posiciones, 2, " ");
		sabotajePosX = atoi(aux[0]);
		sabotajePosY = atoi(aux[1]);
		free(aux[0]); free(aux[1]); free(aux);
		haySabotaje=1;
		Tripulante* tripulante;
		Tripulante* tripulanteFinal;
		bool ordenarTripulantesPorID (Tripulante* trip1, Tripulante* trip2) {
			return (trip1->tripulanteID < trip2->tripulanteID);
		}
		void elegirTripulante () {
			int i, distanciaDeEste;
			for (i=0; i<list_size(colaBlockedEM); i++) {
				tripulante = list_get(colaBlockedEM,i);
				if(i==0) tripulanteFinal = tripulante;
				distanciaDeEste = abs(sabotajePosX-tripulante->posX) + abs(sabotajePosY-tripulante->posY);
				if (distanciaDeEste<distanciaMinimaAlSabotaje) {
					tripulanteFinal = tripulante;;
					distanciaMinimaAlSabotaje = distanciaDeEste;
				}
			}
		}
		pthread_mutex_lock(&mutexColaExec);
		if (list_size(colaExec)>1) list_sort(colaExec, ordenarTripulantesPorID);
		while (list_size(colaExec)>0) {
			tripulante = list_remove(colaExec,0);
			printf("Saco de ejecucion al tripulante %d por sabotaje\n",tripulante->tripulanteID);
			pthread_mutex_lock(&mutexLog);
			log_info(logDiscordiador, "Saco de ejecucion al tripulante %d por sabotaje",tripulante->tripulanteID);
			pthread_mutex_unlock(&mutexLog);
			mensaje = "Me sacaron de ejecucion por sabotaje\n";
			infoParaBitacora(tripulante->tripulanteID,mensaje);
			tripulante->estadoTripulante=BLOCK_EM;
			tripulante->quantumUsado=0;
			list_add(colaBlockedEM,tripulante);
			cantidadDeTripulantesEnExec++;
		}
		pthread_mutex_unlock(&mutexColaExec);
		pthread_mutex_lock(&mutexColaReady);
		if (list_size(colaReady)>1) list_sort(colaReady, ordenarTripulantesPorID);
		while (list_size(colaReady)>0) {
			tripulante = list_remove(colaReady,0);
			printf("Saco de ready al tripulante %d por sabotaje\n",tripulante->tripulanteID);
			pthread_mutex_lock(&mutexLog);
			log_info(logDiscordiador, "Saco de ready al tripulante %d por sabotaje",tripulante->tripulanteID);
			pthread_mutex_unlock(&mutexLog);
			mensaje = "Me sacaron de ready por sabotaje\n";
			infoParaBitacora(tripulante->tripulanteID,mensaje);
			tripulante->estadoTripulante=BLOCK_EM;
			tripulante->quantumUsado=0;
			list_add(colaBlockedEM,tripulante);
			sem_wait(&hayAlguienEnReady);
		}
		pthread_mutex_unlock(&mutexColaReady);
		for (i=0; i<cantidadDeTripulantesEnExec; i++) {sem_post(&hayLugarEnExec);}
		if (list_size(colaBlockedEM)>0) {
			elegirTripulante();
			tripulanteElegido=tripulanteFinal->tripulanteID;
			printf("El tripulante %d es elegido para resolver el sabotaje\n",tripulanteElegido);
			pthread_mutex_lock(&mutexLog);
			log_info(logDiscordiador, "El tripulante %d es elegido para resolver el sabotaje",tripulanteElegido);
			pthread_mutex_unlock(&mutexLog);
			mensaje = "Me eligieron para resolver el sabotaje\n";
			infoParaBitacora(tripulanteElegido,mensaje);
			tripulanteFinal->actividad=EM;
			if (tripulanteFinal->estoyFrenadoExec==1) sem_post(&tripulanteFinal->semExec);
			sem_post(&tripulanteFinal->semEM);
		}
		else {
			printf("No hay tripulantes para resolver el sabotaje\n");
			pthread_mutex_lock(&mutexLog);
			log_info(logDiscordiador, "No hay tripulantes para resolver el sabotaje");
			pthread_mutex_unlock(&mutexLog);
			avisarComienzoFSCK(0);
		}
	}
	free(posiciones);
	return 0;
}

int finDeSabotaje () {
	Tripulante* tripulante;
	char* mensaje;
	int tripulanteQueResolvioElSabotaje = tripulanteElegido;
	tripulanteElegido=-1;
	haySabotaje=0;
	pthread_mutex_lock(&mutexColaReady);
	while (list_size(colaBlockedEM)>0) {
		tripulante = list_remove(colaBlockedEM,0);
		if (tripulante->tripulanteID==tripulanteQueResolvioElSabotaje) tripulante->actividad=EXE;;
		printf("Pongo en ready al tripulante %d por fin de sabotaje\n",tripulante->tripulanteID);
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Pongo en ready al tripulante %d por fin de sabotaje",tripulante->tripulanteID);
		pthread_mutex_unlock(&mutexLog);
		mensaje = "Vuelvo a ready por fin de sabotaje\n";
		infoParaBitacora(tripulante->tripulanteID,mensaje);
		tripulante->estadoTripulante=READY;
		tripulante->quantumUsado=0;
		list_add(colaReady,tripulante);
		sem_post(&hayAlguienEnReady);
	}
	pthread_mutex_unlock(&mutexColaReady);
	iniciarPlanificacion();
	return 0;
}

int crearConexionDiscordiador(char* ip, char* puerto, t_log* logDiscordiador)
	{
	  	struct addrinfo hints;
	    struct addrinfo *server_info;
//	    int* clientSocket = malloc(sizeof(int));
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
	        if (ip == config.IP_MI_RAM_HQ) log_error(logDiscordiador, "FALLO LA CONEXION CON MEMORIA...");
	        else log_error(logDiscordiador, "FALLO LA CONEXION CON FILESYSTEM...");
	        exit(-1);
	    }

//	    if (ip == config.IP_MI_RAM_HQ) log_info(logDiscordiador, "CONEXION EXITOSA CON MEMORIA...");
//	    else log_info(logDiscordiador, "CONEXION EXITOSA CON FILESYSTEM...");
//
//	    if (ip==config.IP_MI_RAM_HQ && !primerConexionMemoria)
//	    {printf("------ Conexion exitosa con Memoria ------\n"); primerConexionMemoria=1;}
//	    if (ip!=config.IP_MI_RAM_HQ && !primerConexionFS)
//	    {printf("------ Conexion exitosa con FileSystem ------\n"); primerConexionFS=1;}

	    freeaddrinfo(server_info);
	    return clientSocket;
}

int conexionConMemoria() {
	int a;
	a = crearConexionDiscordiador(config.IP_MI_RAM_HQ, config.PUERTO_MI_RAM_HQ, logDiscordiador);
	return a;
}

int conexionConFileSystem() {
	int a;
	a = crearConexionDiscordiador(config.IP_I_MONGO_STORE, config.PUERTO_I_MONGO_STORE, logDiscordiador);
	return a;
}

void desbloquearTripulantesNew(Patota* patota) {
	int i;
	Tripulante* tripulante;
	for (i=0; i<patota->cantidadTripulantes; i++) {
		tripulante = list_get(patota->tripulantes,i);
		sem_post(&tripulante->semNew);
	}
}

void matarHilosPlanificadores() {
	int i;
	finDePrograma=1;
	for (i=0;i<planisBloqueadas;i++) sem_post(&semPlanificando);
	planisBloqueadas=0;
	sem_post(&hayLugarEnExec);
	sem_post(&hayAlguienEnReady);
	sem_post(&alertaQuantum);
	sem_post(&siguienteIO);
	sem_post(&hayAlguienEnIO);
}

