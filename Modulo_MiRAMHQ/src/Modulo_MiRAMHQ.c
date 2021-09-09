/*
 ============================================================================
 Name        : Modulo_MiRAMHQ.c
 Author      : DreamTeam
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Modulo_MiRAMHQ.h"

int main(void) {

	iniciarSemaforos();
	inicializarIds();

	signal(SIGUSR1,realizarDump);
	signal(SIGUSR2, compactarPorqueRecibimosSenial);

	ultimoMarcoUsado = -1;
	idTripulantes = 1;

	char* ipLocal = "10.108.80.2";
	char* puerto;
	int servidor;
	int cliente;

	logMiRAMHQ = log_create("MiRAMHQ.log", "MiRAMHQ", 1, LOG_LEVEL_INFO);
	sem_wait(semLogMiRAMHQ);
	log_info(logMiRAMHQ, "INICIANDO LOG MI-RAM-HQ...");
	sem_post(semLogMiRAMHQ);

	configMiRAMHQ = config_create("/home/utnso/tp-2021-1c-DreamTeam/Modulo_MiRAMHQ/miRAMHQ.config");
	sem_wait(semLogMiRAMHQ);
	log_info(logMiRAMHQ, "CREANDO CONFIG MI-RAM-HQ...");
	sem_post(semLogMiRAMHQ);

	if(configMiRAMHQ == NULL){
		sem_wait(semLogMiRAMHQ);
		log_error(logMiRAMHQ, "NO SE PUDO CREAR EL CONFIG MI-RAM-HQ...");
		sem_post(semLogMiRAMHQ);
		exit(-3);
	}
	elementosEnDisco = list_create();

	infoConfig = obtenerInfoConfigMiRAMHQ();

	indiceClock = 0;
	cantEscritoEnDisco = 0;
	lugaresLibresEnDisco = list_create();
	paginasEnDisco = list_create();
<<<<<<< HEAD
	asignadorSegmentos = 0;
	NIVEL* mapaRAM = iniciarMapa();
=======
//	NIVEL* mapaRAM; // = iniciarMapa();
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4

	punteroAMemoria = malloc(infoConfig.tamanioMemoria);

	puerto = infoConfig.puerto;
	servidor = iniciarServidor(ipLocal, puerto);
	cliente = esperarCliente(servidor);

	if (string_equals_ignore_case(infoConfig.esquemaMemoria, "PAGINACION")) {
//		crearArchivoSWAP(infoConfig);
	}

	while(1) {
		nuevoHilo(cliente, mapaRAM);
		cliente = esperarCliente(servidor);
	}

	free(punteroAMemoria);
	config_destroy(configMiRAMHQ);
	log_destroy(logMiRAMHQ);
	nivel_destruir(mapaRAM);

	return EXIT_SUCCESS;

}

int iniciarServidor(char* ip, char* puerto) {
		int socketServidor;
	    struct addrinfo hints, *servinfo, *p;

	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE;

	    getaddrinfo(ip, puerto, &hints, &servinfo);

	    for (p=servinfo; p != NULL; p = p->ai_next)
	    {
	        if ((socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
	    sem_wait(semLogMiRAMHQ);
	    log_info(logMiRAMHQ, "SERVIDOR LISTO PARA RECIBIR CLIENTES EN MI-RAM-HQ..");
	    sem_post(semLogMiRAMHQ);

	    return socketServidor;
}

int esperarCliente(int socketServidor) {
	struct sockaddr_in dirCliente;
	socklen_t tamDireccion = sizeof(struct sockaddr_in);

	int socketCliente = accept(socketServidor, (void*) &dirCliente, &tamDireccion);
	sem_wait(semLogMiRAMHQ);
	log_info(logMiRAMHQ, "SE CONECTO UN CLIENTE EN MI-RAM-Q");
	sem_post(semLogMiRAMHQ);
	return socketCliente;
}

infoConfigMiRAMHQ obtenerInfoConfigMiRAMHQ() {
	infoConfigMiRAMHQ info;

	info.algoritmoReemplazo = config_get_string_value(configMiRAMHQ, "ALGORITMO_REEMPLAZO");
	info.esquemaMemoria = config_get_string_value(configMiRAMHQ, "ESQUEMA_MEMORIA");
	info.pathSwap = config_get_string_value(configMiRAMHQ, "PATH_SWAP");
	info.puerto = config_get_string_value(configMiRAMHQ, "PUERTO");
	info.tamanioMemoria = config_get_int_value(configMiRAMHQ, "TAMANIO_MEMORIA");
	info.tamanioPagina = config_get_int_value(configMiRAMHQ, "TAMANIO_PAGINA");
	info.tamanioSwap = config_get_int_value(configMiRAMHQ, "TAMANIO_SWAP");
	info.criterioSeleccion = config_get_string_value(configMiRAMHQ, "CRITERIO_SELECCION");

	return info;
}

void realizarDump(){
	int variableIdPatotaAux = 0;
	char* pathDump = string_new();

	char* horaActual = temporal_get_string_time("%H:%M:%S:%MS");
	char* fechaActual = temporal_get_string_time("%d/%m/%y %H:%M:%S");

	string_append(&pathDump, "Dump_");
	string_append(&pathDump, horaActual);
	string_append(&pathDump, ".dmp");

	char* formato = string_new();

	string_append(&formato, "Dump: ");
	string_append(&formato, fechaActual);

	FILE* dumpFile = fopen(pathDump , "a+");
	fputs(formato, dumpFile);

	if(string_equals_ignore_case(infoConfig.esquemaMemoria, "SEGMENTACION")) {

		char* proceso = string_new();
		int i;

		for(i=0; i < list_size(elementosEnDisco); i++){
			segmentacion* segmento = list_get(elementosEnDisco, i);

			void* direccionInicio;
			uint32_t posicionDirLogicaPCB;

			if (segmento->tipo == PCB){
				string_append(&proceso, "Proceso: ");
				string_append(&proceso, string_itoa(segmento->elementoID));
				string_append(&proceso, "  ");
				string_append(&proceso, "Segmento: ");
				string_append(&proceso, string_itoa(segmento->nroSegmento));
				string_append(&proceso, "  ");
				string_append(&proceso, "Inicio: ");
				direccionInicio = punteroAMemoria + segmento->inicio;
				string_append(&proceso, string_itoa(direccionInicio));
				string_append(&proceso, "  ");
				string_append(&proceso, "Tam: ");
				string_append(&proceso, string_itoa(segmento->tamanio));
				string_append(&proceso, "\n");

				fputs(proceso, dumpFile);

			}else if (segmento->tipo == TCB){
				string_append(&proceso, "Proceso: ");
				tripulanteControlBlock* tripulante = malloc(sizeof(tripulanteControlBlock));
				patotaControlBlock* patota = malloc(sizeof(patotaControlBlock));

				int despl = 0;
				void* direccionFisica = punteroAMemoria + segmento->inicio;
				memcpy(&(tripulante->tid), direccionFisica+despl, sizeof(uint32_t));
				despl += sizeof(uint32_t);
				memcpy(&(tripulante->estado), direccionFisica+despl, sizeof(char));
				memcpy(&(tripulante->posicionX), direccionFisica+despl, sizeof(uint32_t));
				memcpy(&(tripulante->posicionY), direccionFisica+despl, sizeof(uint32_t));
				memcpy(&(tripulante->proximaInstruccion), direccionFisica+despl, sizeof(uint32_t));
				memcpy(&(tripulante->dirLogicaPCB), direccionFisica+despl, sizeof(uint32_t));

				void* direccionFisicaPCB = punteroAMemoria + tripulante->dirLogicaPCB;
				memcpy(patota, direccionFisicaPCB, sizeof(patotaControlBlock));

				direccionInicio = punteroAMemoria + segmento->inicio;
<<<<<<< HEAD
				printf(prueba, "%p", (void*) direccionInicio);
				//char* dir = mem_hexstring(direccionInicio, segmento->tamanio);
				string_append(&proceso, string_itoa(patota->pid));
=======

				string_append(&proceso, string_itoa(patota.pid));
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
				string_append(&proceso, "  ");
				string_append(&proceso, "Segmento: ");
				string_append(&proceso, string_itoa(segmento->nroSegmento));
				string_append(&proceso, "  ");
				string_append(&proceso, "Inicio: ");
				string_append(&proceso, string_itoa(direccionInicio));
				string_append(&proceso, "  ");
				string_append(&proceso, "Tam: ");
				string_append(&proceso, string_itoa(segmento->tamanio));
				string_append(&proceso, "\n");

				fputs(proceso, dumpFile);

			}else if (segmento->tipo == TAREAS){
				direccionInicio = punteroAMemoria + segmento->inicio;

				string_append(&proceso, "Proceso: ");
				string_append(&proceso, string_itoa(segmento->elementoID));
				string_append(&proceso, "  ");
				string_append(&proceso, "Segmento: ");
				string_append(&proceso, string_itoa(segmento->nroSegmento));
				string_append(&proceso, "  ");
				string_append(&proceso, "Inicio: ");
				string_append(&proceso, string_itoa(direccionInicio));
				string_append(&proceso, "  ");
				string_append(&proceso, "Tam: ");
				string_append(&proceso, string_itoa(segmento->tamanio));
				string_append(&proceso, "\n");

				fputs(proceso, dumpFile);
			}
		}

	}else if (string_equals_ignore_case(infoConfig.esquemaMemoria, "PAGINACION")){
//		char* proceso = string_new();
//
//		t_list* paginasEnMemoria = obtenerPaginasEnMemoria(paginasEnDisco);
//		t_list* marcosEnMemoria = obtenerMarcosEnMemoria(paginasEnMemoria);
//
//		int cantidadDeMarcos = calcularCantidadDeMarcos(infoConfig);
//
//		for (int i = 0; i < cantidadDeMarcos; i++ ){
//
//			if (existeNumeroDeMarcoEnMemoria(marcosEnMemoria, i)){
//
//				pagina* pagina = obtenerLaPaginaDelMarco(paginasEnMemoria, i);
//
//				string_append(&proceso, "Marco: ");
//				string_append(&proceso, string_itoa(pagina->nroMarco));
//				string_append(&proceso, "  ");
//				string_append(&proceso, "Estado: ");
//				string_append(&proceso, "Ocupado");
//				string_append(&proceso, "  ");
//				string_append(&proceso, "Proceso: ");
//				string_append(&proceso, string_itoa(pagina->identificadorDeProceso));
//				string_append(&proceso, "  ");
//				string_append(&proceso, "Pagina: ");
//				string_append(&proceso, string_itoa(pagina->nroPagina));
//				string_append(&proceso, "\n");
//
//			}else{
//
//				string_append(&proceso, "Marco: ");
//				string_append(&proceso, string_itoa(i));
//				string_append(&proceso, "  ");
//				string_append(&proceso, "Estado: ");
//				string_append(&proceso, "Libre");
//				string_append(&proceso, "  ");
//				string_append(&proceso, "Proceso: ");
//				string_append(&proceso, "-");
//				string_append(&proceso, "  ");
//				string_append(&proceso, "Pagina: ");
//				string_append(&proceso, "-");
//				string_append(&proceso, "\n");
//
//			}
//
//			fputs(proceso, dumpFile);
//
//		}
//
//	free(pathDump);
//	free(formato);
//	fclose(dumpFile);
	}
}

NIVEL* iniciarMapa(){
	NIVEL* mapa;
	int filas;
	int columnas;
	int err;

	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&columnas, &filas);

	mapa = nivel_crear("Mapa DreamTeam");

	err = caja_crear(mapa, 'F', 3, 5, 5);
	ASSERT_CREATE(mapa, 'F', err);

	err = caja_crear(mapa, 'D', 8, 15, 3);
	ASSERT_CREATE(mapa, 'D', err);

	if(err) {
		printf("WARN: %s\n", nivel_gui_string_error(err));
	}

	nivel_gui_dibujar(mapa);

	return mapa;
}

void iniciarSemaforos(){
	semIdTripulante = malloc(sizeof(sem_t));
	semIdPatota = malloc(sizeof(sem_t));

	semMemoria = malloc(sizeof(sem_t));
	semArchivoSwap = malloc (sizeof(sem_t));

	semAlgoritmo = malloc(sizeof(sem_t));
	semMapa = malloc(sizeof(sem_t));

	semElementosEnDisco = malloc(sizeof(sem_t));

	semLogMiRAMHQ = malloc (sizeof(sem_t));
	semPaginasEnDisco = malloc(sizeof(sem_t));

	semLugaresLibresEnDisco = malloc(sizeof(sem_t));

	sem_init(semIdTripulante, 0, 1);
	sem_init(semIdPatota, 0, 1);
	sem_init(semMemoria, 0, 1);
	sem_init(semMapa, 0, 1);
	sem_init(semArchivoSwap, 0, 1);
	sem_init(semElementosEnDisco, 0, 1);
	sem_init(semLogMiRAMHQ, 0, 1);
	sem_init(semPaginasEnDisco, 0, 1);
	sem_init(semAlgoritmo, 0, 1);
	sem_init(semLugaresLibresEnDisco, 0, 1);

}

void inicializarIds(){
	idTripulantes = 0;
	idPatota= 0;
}
