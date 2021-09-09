#include "tripulante.h"

void* startThread(void* args){

	Tripulante* tripulante;
	tripulante=args;

	sem_wait(&tripulante->semNew);

	if(!errorNew) {

	bool encontrarTripulanteEnLista(Tripulante* trip) {
		return (trip->tripulanteID==tripulante->tripulanteID);
	}

	char* mensaje;
	usleep(5000*(1+tripulante->tripulanteID));
	tripulante->estadoTripulante=READY;
	mensaje = "Me pasaron a ready\n";
//	infoParaBitacora(tripulante->tripulanteID,mensaje);
	pthread_mutex_lock(&mutexColaNew);
	list_remove_by_condition(colaNew,encontrarTripulanteEnLista);
	pthread_mutex_unlock(&mutexColaNew);
	pthread_mutex_lock(&mutexColaReady);
	list_add(colaReady,tripulante);
	pthread_mutex_unlock(&mutexColaReady);
	sem_post(&hayAlguienEnReady);

	tripulante->durmiendo=0;
	tripulante->tengoTarea=0;
	tripulante->tiempoRestante=0;
	tripulante->comienzoAHacerTarea=0;
	tripulante->quantumUsado=0;
	tripulante->actividad=EXE;
	tripulante->k=-1;

	while (tripulante->estadoTripulante!=FINISH) {
		switch (tripulante->actividad) {
			case EXE:
				tripulanteExec(tripulante);
				break;
			case IO:
				tripulanteIO(tripulante);
				break;
			case EM:
				tripulanteEM(tripulante);
				break;
		}
	}

	}
	return 0;
}

int tripulanteExec (Tripulante* tripulante) {

	int id = tripulante->tripulanteID;
	tripulante->estoyFrenadoExec=1;
	char* mensaje;
	char* nombreTarea;
	int posx = tripulante->posX;
	int posy = tripulante->posY;

	bool encontrarTripulanteEnLista(Tripulante* trip) {
		return (trip->tripulanteID==id);
	}

	///////////////////// borrar cuando me manden las tareas
//	Task tareas[4];
//	int j=0;
//	for(j=0; j<4; j++) {
//		tareas[j].tarea="REGAR_PLANTAS";
//		tareas[j].parametro=9999;
//		tareas[j].tiempo=3;
//		tareas[j].posX=3;
//		tareas[j].posY=3;
//	}
//	tareas[3].tiempo=0;
//	tareas[1].parametro=9;
//	tareas[1].posX=5;
//	tareas[1].posY=5;
//	tareas[1].tarea="GENERAR_COMIDA";
//	tareas[1]=asignarIdTarea(tareas[1]);
	////////////////////

	// Espera a que lo pongan en ejecucion
	sem_wait(&tripulante->semExec);
	tripulante->estoyFrenadoExec=0; if (!haySabotaje && tripulante->estadoTripulante!=FINISH) {

	// Si no tiene tarea, solicita la siguiente
	if (!tripulante->tengoTarea && tripulante->tiempoRestante!=-5) {
		tripulante->k++;
//		tripulante->tarea=tareas[tripulante->k];
		tripulante->tarea = solicitarTarea(id);
		if (tripulante->tarea.tiempo>0) {
			tripulante->tengoTarea=1;
			tripulante->tiempoRestante=tripulante->tarea.tiempo;
			tripulante->comienzoAHacerTarea=0;
		}
	}
	// Si no le dieron una tarea, finaliza
	if (!tripulante->tengoTarea && tripulanteElegido!=id) {
		pthread_mutex_lock(&mutexColaExec);
		tripulante->estadoTripulante=FINISH;
		printf("FINALIZA EL TRIPULANTE %d\n",id);
		mensaje = "Finalizando...\n";
		infoParaBitacora(id,mensaje);
		list_remove_by_condition(colaExec,encontrarTripulanteEnLista);
		list_add(colaExit,tripulante);
		pthread_mutex_unlock(&mutexColaExec);
		sem_post(&hayLugarEnExec);
	} else if (!tripulante->tengoTarea)	tripulante->tiempoRestante=-5;

	// Si tiene nueva tarea, comienza a trabajar
	else {

		// Si no esta en la posicion de la tarea, se mueve
		if (tripulante->posX < tripulante->tarea.posX) tripulante->posX++;
		else if (tripulante->posX > tripulante->tarea.posX) tripulante->posX--;
		else if (tripulante->posY < tripulante->tarea.posY) tripulante->posY++;
		else if (tripulante->posY > tripulante->tarea.posY) tripulante->posY--;

		// Si ya esta en la posicion de la tarea, comienza a hacerla
		else {
			tripulante->tiempoRestante--;
			tripulante->comienzoAHacerTarea=1;
			if (tripulante->tiempoRestante==0) {
				printf("Soy el tripulante %d, termine mi tarea\n",id);
				mensaje = "Termine mi tarea: ";
				nombreTarea=(tripulante->tarea).tarea;
				char* paraBitacora = string_new();
				string_append(&paraBitacora,mensaje);
				string_append(&paraBitacora,nombreTarea);
				string_append(&paraBitacora,"\n");
				infoParaBitacora(id,paraBitacora);
				free(paraBitacora);
			}
			else if (tripulante->tarea.parametro==9999) {
				printf("Soy el tripulante %d, me quedan %d tiempos de mi tarea\n",id,tripulante->tiempoRestante);
				mensaje = "Estoy haciendo mi tarea: ";
				nombreTarea=tripulante->tarea.tarea;
				char* paraBitacora = string_new();
				string_append(&paraBitacora,mensaje);
				string_append(&paraBitacora,nombreTarea);
				string_append(&paraBitacora,"\n");
				infoParaBitacora(id,paraBitacora);
				free(paraBitacora);
			}
		}

		// Si se movio, lo informa
		if (tripulante->comienzoAHacerTarea==0) {
			printf("Soy el tripulante %d, me movi a %d|%d\n",id,tripulante->posX,tripulante->posY);
			informarMovimiento(id,tripulante->posX,tripulante->posY,posx,posy);
		}

		// Si es una tarea sin IO y ya la termino, pide nueva tarea
		if (tripulante->tarea.parametro==9999) {
			if (tripulante->tiempoRestante==0) {
				tripulante->tengoTarea=0;
				free(tripulante->tarea.tarea);
				free(tripulante->tarea.tareaEscrita);
			}
		}

		// Si es una tarea con IO y ya esta en posicion, solicita IO
		else {
			if (tripulante->tiempoRestante==tripulante->tarea.tiempo-1 && tripulanteElegido!=id && planificando) {
				pthread_mutex_lock(&mutexColaExec);
				printf("Soy el tripulante %d, solicito IO\n",id);
				mensaje = "Solicito IO\n";
				infoParaBitacora(id,mensaje);
				tripulante->actividad=IO;
				tripulante->tiempoRestante=tripulante->tarea.tiempo;
				tripulante->estadoTripulante=BLOCK_IO;
				list_remove_by_condition(colaExec,encontrarTripulanteEnLista);
				pthread_mutex_unlock(&mutexColaExec);
				pthread_mutex_lock(&mutexColaBlockIO);
				list_add(colaBlockedIO,tripulante);
				pthread_mutex_unlock(&mutexColaBlockIO);
				if (list_size(colaBlockedIO)>1) tripulante->estoyFrenadoIO=1; else tripulante->estoyFrenadoIO=0;
				sem_post(&hayLugarEnExec);
				sem_post(&hayAlguienEnIO);
			} else if (tripulante->tiempoRestante==tripulante->tarea.tiempo-1) tripulante->tiempoRestante=tripulante->tarea.tiempo;
		}

		// Si estamos en RR, suma quantum y avisa en caso de llegar al limite
		if (!strcmp(config.ALGORITMO,"RR")) {
			tripulante->quantumUsado++;
			if (tripulante->quantumUsado==config.QUANTUM && tripulante->estadoTripulante==EXEC) {
//				pthread_mutex_lock(&mutexQuantum);
				sem_wait(&turnosQuantum);
				sem_post(&alertaQuantum);

			}
		}
		tripulante->durmiendo=1;
		sleep(config.RETARDO_CICLO_CPU);
		tripulante->durmiendo=0;
	}

	// Si sigue en ejecucion, se habilita la re-entrada. Si se pauso la planificacion, lo guarda para restaurarlo
	if (planificando && tripulante->quantumUsado<config.QUANTUM && tripulante->estadoTripulante==EXEC)
		sem_post(&tripulante->semExec);
	else if (tripulante->quantumUsado<config.QUANTUM && tripulante->estadoTripulante==EXEC && !haySabotaje)
		list_add(restaurarSemaforo,&tripulante->semExec);

	}
	return 0;
}

int tripulanteIO (Tripulante* tripulante) {

	char* mensaje;
	char* nombreTarea;
	int id = tripulante->tripulanteID;
	if (planificando) tripulante->estoyFrenadoIO=1;

	// Espera a que sea su turno de hacer IO
	sem_wait(&tripulante->semIO);
	tripulante->estoyFrenadoIO=0; if(planificando && tripulante->estadoTripulante!=FINISH) {

	if (tripulante->tiempoRestante==tripulante->tarea.tiempo) {
		hacerTarea(tripulante->tarea);
	}
	tripulante->tiempoRestante--;

	// Si finaliza su IO, lo manda a ready
	if (tripulante->tiempoRestante==0 && tripulanteElegido!=id) {
		pthread_mutex_lock(&mutexColaBlockIO);
		printf("Soy el tripulante %d, termine mi IO y vuelvo a Ready\n",id);
		mensaje = "Termine mi IO y vuelvo a ready\n";
		infoParaBitacora(id,mensaje);
		tripulante->tengoTarea=0;
		free(tripulante->tarea.tarea);
		free(tripulante->tarea.tareaEscrita);
		tripulante->estadoTripulante=READY;
		tripulante->actividad=EXE;
		list_remove(colaBlockedIO,0);
		pthread_mutex_unlock(&mutexColaBlockIO);
		pthread_mutex_lock(&mutexColaReady);
		tripulante->quantumUsado=0;
		list_add(colaReady,tripulante);
		sem_post(&hayAlguienEnReady);
		pthread_mutex_unlock(&mutexColaReady);
		sem_post(&siguienteIO);
	} else if (tripulante->tiempoRestante==0) tripulante->tiempoRestante++;

	// Si no finaliza, informa cuanto le queda
	else {
		printf("Soy el tripulante %d, me quedan %d ciclos de IO\n",id,tripulante->tiempoRestante);
		mensaje = "Estoy haciendo IO de la tarea: ";
		nombreTarea=tripulante->tarea.tarea;
		char* paraBitacora = string_new();
		string_append(&paraBitacora,mensaje);
		string_append(&paraBitacora,nombreTarea);
		string_append(&paraBitacora,"\n");
		infoParaBitacora(id,paraBitacora);
		free(paraBitacora);
	}
	sleep(config.RETARDO_CICLO_CPU);

	}
	// Si sigue en su IO, se habilita la re-entrada
	if (tripulante->estadoTripulante==BLOCK_IO && planificando)
		sem_post(&tripulante->semIO);
	else if (tripulante->estadoTripulante==BLOCK_IO)
		list_add(restaurarSemaforo,&tripulante->semIO);

	return 0;
}

int tripulanteEM (Tripulante* tripulante) {

	int id = tripulante->tripulanteID;
	char* mensaje;
	int posx = tripulante->posX;
	int posy = tripulante->posY;

	bool encontrarTripulanteEnLista(Tripulante* trip) {
		return (trip->tripulanteID==id);
	}

	// Espera a ser elegido para resolver el sabotaje
	sem_wait(&tripulante->semEM); if (haySabotaje) {

	// Si no esta en posicion, se mueve hacia el sabotaje
	if (tripulante->posX < sabotajePosX) tripulante->posX++;
	else if (tripulante->posX > sabotajePosX) tripulante->posX--;
	else if (tripulante->posY < sabotajePosY) tripulante->posY++;
	else if (tripulante->posY > sabotajePosY) tripulante->posY--;

	// Si esta en posicion, activa el FSCK para resolver el sabotaje
	else {
		avisarComienzoFSCK(1);
		printf("El sabotaje comienza a ser resuelto...\n");
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador,"El sabotaje comienza a ser resuelto");
		pthread_mutex_unlock(&mutexLog);
		mensaje = "Comienzo a resolver el sabotaje\n";
		infoParaBitacora(id,mensaje);
		tripulante->comienzoAHacerTarea=0;
		list_remove_by_condition(colaBlockedEM,encontrarTripulanteEnLista);
		list_add(colaBlockedEM,tripulante);
		sleep(config.DURACION_SABOTAJE);
		printf("Fin del sabotaje!\n");
		pthread_mutex_lock(&mutexLog);
		log_info(logDiscordiador, "Fin del sabotaje");
		pthread_mutex_unlock(&mutexLog);
		mensaje = "Termine de resolver el sabotaje\n";
		infoParaBitacora(id,mensaje);
		finDeSabotaje();
	}

	// Si se movio, lo informa
	if (tripulanteElegido!=-1) {
		 informarMovimiento(id,tripulante->posX,tripulante->posY,posx,posy);
		printf("Soy el tripulante %d yendo al sabotaje, me movi a %d|%d\n",id,tripulante->posX,tripulante->posY);
	}

	sleep(config.RETARDO_CICLO_CPU);

	// Si no termino de resolver el sabotaje, se habilita la re-entrada
	if (tripulante->estadoTripulante==BLOCK_EM && tripulanteElegido == id)
		sem_post(&tripulante->semEM);

	} return 0;
}
