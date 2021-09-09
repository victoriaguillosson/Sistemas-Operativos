#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <commons/collections/node.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <commons/log.h>
#include <assert.h>
#include <time.h>

typedef struct {
	char* IP_MI_RAM_HQ;
	char* PUERTO_MI_RAM_HQ;
	char* IP_I_MONGO_STORE;
	char* PUERTO_I_MONGO_STORE;
	char* ALGORITMO;
	int GRADO_MULTITAREA;
	int QUANTUM;
	int DURACION_SABOTAJE;
	int RETARDO_CICLO_CPU;
} Config;
Config config;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK_IO,
	BLOCK_EM,
	FINISH
} ESTADO_TRIPULANTE;

typedef struct {
	int commandType;
	char * parameters;
} InfoCommand;

typedef struct {
	uint32_t patotaID;
	int cantidadTripulantes;
	t_list* tareas;
	t_list* tripulantes;
} Patota;

typedef enum {
	EXE,
	IO,
	EM
} ACTIVIDAD;

typedef enum {
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
} IdTareas;

typedef struct{
	char * tarea;
	int parametro;
	int posX;
	int posY;
	int tiempo;
	char* tareaEscrita;
	IdTareas id;
} Task;

typedef struct{
	uint32_t tripulanteID;
	uint32_t patotaID;
	ESTADO_TRIPULANTE estadoTripulante;
	uint32_t posX;
	uint32_t posY;
	sem_t semExec;
	sem_t semIO;
	sem_t semEM;
	sem_t semNew;
	int quantumUsado;
	int tengoTarea;
	int tiempoRestante;
	int comienzoAHacerTarea;
	int estoyFrenadoExec;
	int estoyFrenadoIO;
	ACTIVIDAD actividad;
	Task tarea;
	int k; //BORRAR
	int durmiendo;
//	int socketMemoria;
//	int socketFileSystem;
} Tripulante;

//typedef struct{
//	pthread_t hilo;
//	uint32_t indexDelHilo;
//	int conexion;
//} HiloConFileSystem;

sem_t finDeHilos;
int finDePrograma;
int cantHilos;
t_log * logDiscordiador;

t_list* colaNew;;
t_list* colaReady;
t_list* colaExec;
t_list* colaExit;
t_list* colaBlockedIO;
t_list* colaBlockedEM;
t_list* restaurarSemaforo;

pthread_mutex_t mutexColaExec;
pthread_mutex_t mutexColaBlockIO;
pthread_mutex_t mutexColaReady;
pthread_mutex_t mutexColaNew;
pthread_mutex_t mutexMemoria;
pthread_mutex_t mutexFileSystem;
pthread_mutex_t mutexQuantum;
pthread_mutex_t mutexLog;

#endif
