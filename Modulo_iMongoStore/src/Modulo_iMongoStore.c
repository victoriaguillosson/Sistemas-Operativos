/*
 ============================================================================
 Name        : Modulo_iMongoStore.c
 Author      : DreamTeam
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Modulo_iMongoStore.h"

int main(void) {

	signal(SIGUSR1,inicioSabotaje);
	ipLocal = "127.0.0.1";
	posSabotaje = 0;
	char* puerto;
	int cliente;
	int servidor;

	pid_t pidModulo = getpid();
	printf("PID: %d\n", pidModulo);

	logiMongoStore = log_create("iMongoStore.log", "iMongoStore", 1, LOG_LEVEL_INFO);
	log_info(logiMongoStore, "INICIANDO LOG iMONGOSTORE...");

	configiMongoStore = config_create("/home/utnso/tp-2021-1c-DreamTeam/Modulo_iMongoStore/iMongoStore.config");
	log_info(logiMongoStore, "CREANDO CONFIG iMONGOSTORE...");


	if(configiMongoStore == NULL){
		log_error(logiMongoStore, "NO SE PUDO CREAR EL CONFIG...");
		exit(-3);
	}
	infConf  = obtenerInfoConfig();

	iniciarSemaforos();
	listaDeTripulantes = list_create();
	diccionarioDeSemBitaco = dictionary_create();

	iniciarFileSystem();

	pthread_t* hiloSincronizador = malloc(sizeof(pthread_t));
	pthread_create(hiloSincronizador, NULL, (void*)sincronizar, NULL);
	pthread_detach(*hiloSincronizador);


	puerto = infConf.puerto;
	servidor = iniciarServidor(ipLocal, puerto, logiMongoStore);
	cliente = esperarCliente(servidor, logiMongoStore);


	while(1){
		nuevoHilo(cliente);

		cliente = esperarCliente(servidor, logiMongoStore);
	}


	//return 0 on success -1 on failure
	munmap(superBlockPointer,sizeSuperBloque);
	munmap(blocksPointer,sizeBlock);

	free(hiloSincronizador);

	list_destroy(listaDeTripulantes);

	bitarray_destroy(bitarray);

	config_destroy(configiMongoStore);
	log_destroy(logiMongoStore);

	dictionary_destroy(diccionarioDeSemBitaco);

	return EXIT_SUCCESS;
}




int iniciarServidor(char* ip, char* puerto, t_log * log) {
		int socketServidor;
	    struct addrinfo hints, *servinfo, *p;

	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE;

	    getaddrinfo(ip, puerto, &hints, &servinfo);


	    for (p=servinfo; p != NULL; p = p->ai_next)
	    {
	        if ((socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
	        	continue;
	        }else{
	    	    int activado = 1;
	    	    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	        }


	        if (bind(socketServidor, p->ai_addr, p->ai_addrlen) == -1) {
	            close(socketServidor);
	            continue;
	        }
	        break;
	    }

		listen(socketServidor, SOMAXCONN);

	    freeaddrinfo(servinfo);

	    log_info(log, "SERVIDOR LISTO PARA RECIBIR CLIENTES...");

	    return socketServidor;
}


int esperarCliente(int socketServidor, t_log * log) {
	struct sockaddr_in dirCliente;
	socklen_t tamDireccion = sizeof(struct sockaddr_in);

	int socketCliente;
	socketCliente = accept(socketServidor, (void*) &dirCliente, &tamDireccion);

	log_info(log, "SE CONECTO UN CLIENTE!!!!");

	return socketCliente;
}



//INFO CONFIG

infoConfig obtenerInfoConfig(){
	infoConfig info;

	info.montaje = config_get_string_value(configiMongoStore, "PUNTO_MONTAJE");
	info.puerto = config_get_string_value(configiMongoStore, "PUERTO");
	info.tiempoSync = config_get_int_value(configiMongoStore, "TIEMPO_SINCRONIZACION");
	info.posicionesSabotaje = config_get_array_value(configiMongoStore, "POSICIONES_SABOTAJE");

	return info;
}


void iniciarSemaforos(){
	archivoSuperBloque = malloc(sizeof(sem_t));
	archivoBlocks = malloc(sizeof(sem_t));
	mutexTripulantes = malloc(sizeof(sem_t));
	nuevaBitaco = malloc(sizeof(sem_t));
	agregarADiccionario = malloc(sizeof(sem_t));
	semOxigeno = malloc(sizeof(sem_t));
	semComida = malloc(sizeof(sem_t));
	semBasura = malloc(sizeof(sem_t));
	semMD5 = malloc(sizeof(sem_t));


	sem_init(archivoSuperBloque, 0, 1);
	sem_init(archivoBlocks, 0, 1);
	sem_init(mutexTripulantes, 0, 1);
	sem_init(nuevaBitaco, 0, 1);
	sem_init(agregarADiccionario, 0, 1);
	sem_init(semOxigeno, 0, 1);
	sem_init(semComida, 0, 1);
	sem_init(semBasura, 0, 1);
	sem_init(semMD5, 0, 1);

}



















