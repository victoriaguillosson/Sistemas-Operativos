#include "planificacion.h"

int iniciarPlanificacion() {
	int i;
	sem_t* semaforo;
	if (!primeraVez) {
		sem_wait(&finDeHilos);
		planisBloqueadas=0;
		planificando=1;
		pthread_t hilos[3];
		sem_init(&hayLugarEnExec,1,config.GRADO_MULTITAREA);
		if (!strcmp(config.ALGORITMO,"FIFO")) {
			cantHilos+=2;
			if (pthread_create(&hilos[0], NULL, &readyToExec, NULL) != 0) {} pthread_detach(hilos[0]);
			if (pthread_create(&hilos[1], NULL, &execToIO, NULL) != 0) {} pthread_detach(hilos[1]);
		}
		else if (!strcmp(config.ALGORITMO,"RR")) {
			cantHilos+=3;
			if (pthread_create(&hilos[0], NULL, &readyToExec, NULL) != 0) {} pthread_detach(hilos[0]);
			if (pthread_create(&hilos[1], NULL, &execToIO, NULL) != 0) {} pthread_detach(hilos[1]);
			if (pthread_create(&hilos[2], NULL, &desalojoQuantum, NULL) != 0) {} pthread_detach(hilos[2]);
		}
		else printf ("No se leyo un algoritmo valido en archivo de configuracion");
		primeraVez=1;
	}
	else if (haySabotaje) printf("Hay un sabotaje en curso!\n");
	else if (planificando==1) printf("Ya estamos planificando!\n");
	else {
		planificando=1;
		if (list_size(restaurarSemaforo)>0) {
			for (i=0;i<list_size(restaurarSemaforo);i++) {
				semaforo=list_get(restaurarSemaforo,i);
				sem_post(semaforo);
			}
			list_clean(restaurarSemaforo);
		}
		for (i=0;i<planisBloqueadas;i++) sem_post(&semPlanificando);
		planisBloqueadas=0;
	}
	return 0;
}

int pausarPlanificacion() {
	planificando=0;
	return 0;
}

void readyToExec() {
	Tripulante* tripulante;
	char* mensaje;
	while (1) {
		if(planificando==0) {planisBloqueadas++; sem_wait(&semPlanificando);}
		sem_wait(&hayLugarEnExec);
		sem_wait(&hayAlguienEnReady);
		if (finDePrograma) {cantHilos--; if(cantHilos==0) sem_post(&finDeHilos); pthread_exit(NULL);}
		if (planificando) {
			pthread_mutex_lock(&mutexColaReady);
			tripulante = list_remove(colaReady,0);
			pthread_mutex_unlock(&mutexColaReady);
			printf("Se pone a ejecutar al tripulante %d\n",tripulante->tripulanteID);
			mensaje = "Me ponen a trabajar\n";
//			infoParaBitacora(tripulante->tripulanteID,mensaje);
			tripulante->estadoTripulante=EXEC;
			pthread_mutex_lock(&mutexColaExec);
			list_add(colaExec,tripulante);
			pthread_mutex_unlock(&mutexColaExec);
			if(tripulante->durmiendo==0) sem_post(&tripulante->semExec);
		} else {sem_post(&hayLugarEnExec);sem_post(&hayAlguienEnReady);}
	}
}

void desalojoQuantum() {
	Tripulante* tripulante;
	int x;
	char* mensaje;
	bool encontrarTripulanteEnLista(Tripulante* trip) {
		return (trip->tripulanteID==tripulante->tripulanteID);
	}
	while (1) {
		if(planificando==0) {cantHilos--; if(cantHilos==0) sem_post(&finDeHilos); pthread_exit(NULL);}
		sem_wait(&alertaQuantum);
		if (finDePrograma) {cantHilos--; pthread_exit(NULL);}
		pthread_mutex_lock(&mutexColaExec);
		for (x=0;x<list_size(colaExec);x++) {
			tripulante = list_get(colaExec,x);
			if (tripulante->quantumUsado==config.QUANTUM) {
				x+=100;
				tripulante = list_remove_by_condition(colaExec,encontrarTripulanteEnLista);
				pthread_mutex_lock(&mutexColaReady);
				printf("Se desaloja al tripulante %d por quantum\n",tripulante->tripulanteID);
				mensaje = "Me sacaron de exec por quantum\n";
//				infoParaBitacora(tripulante->tripulanteID,mensaje);
				tripulante->estadoTripulante=READY;
				tripulante->quantumUsado=0;
				list_add(colaReady,tripulante);
				sem_post(&hayAlguienEnReady);
				sem_post(&hayLugarEnExec);
				pthread_mutex_unlock(&mutexColaReady);
			}
		}
		pthread_mutex_unlock(&mutexColaExec);
//		pthread_mutex_unlock(&mutexQuantum);
		sem_post(&turnosQuantum);
	}
}

void execToIO() {
	Tripulante* tripulante;
	char* mensaje;
	while (1) {
		if(planificando==0) {planisBloqueadas++; sem_wait(&semPlanificando);}
		sem_wait(&siguienteIO);
		sem_wait(&hayAlguienEnIO);
		if (finDePrograma) {cantHilos--; if(cantHilos==0) sem_post(&finDeHilos); pthread_exit(NULL);}
		if (planificando) {
			pthread_mutex_lock(&mutexColaBlockIO);
			if (!list_is_empty(colaBlockedIO)) tripulante = list_get(colaBlockedIO,0);
			pthread_mutex_unlock(&mutexColaBlockIO);
			printf("El tripulante %d comienza a hacer su IO\n",tripulante->tripulanteID);
			mensaje = "Comienzo a hacer mi IO\n";
//			infoParaBitacora(tripulante->tripulanteID,mensaje);
			sem_post(&tripulante->semIO);
		} else {sem_post(&siguienteIO);sem_post(&hayAlguienEnIO);}
	}
}

