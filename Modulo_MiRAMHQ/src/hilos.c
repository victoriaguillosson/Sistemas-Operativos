/*
 * hilos.c
 *
 *  Created on: 4 jun. 2021
 *      Author: utnso
 */

#include "hilos.h"


//TODO: Si hay problemas de memoria fijarse si se pueden cambiar los list_remove_by_condition por list_remove_and_destroy_by_condition
void nuevoHilo(int conexionCliente, NIVEL* mapa){

	pthread_t newHilo;

	arg_struct* args = malloc(sizeof(arg_struct));
	args->conexion = conexionCliente;
	args->mapa = mapa;

	pthread_create(&newHilo, NULL, (void*)mainHilo, (void*)args);
	pthread_detach(newHilo);

}

void mainHilo(void* argumentos){

	arg_struct* args = (arg_struct*) argumentos;

	recibirPaqueteRAM(args);

	close(args->conexion);
	free(args);
}

void recibirPaqueteRAM(arg_struct* argumentos){

	ID_MENSAJE tipoDeMensaje;
	int bytesARecibir;

	recv(argumentos->conexion, &bytesARecibir, sizeof(int), MSG_WAITALL);
	void* buffer = malloc(bytesARecibir);
	recv(argumentos->conexion, buffer, bytesARecibir, 0);

	int desplazamiento = sizeof(ID_MENSAJE);

	memcpy(&tipoDeMensaje, buffer, sizeof(ID_MENSAJE));




/*INICIAR PATOTA*/
if (tipoDeMensaje == INICIAR_PATOTA) {
	int i;
	int largoProxTarea = 0;

	iniciarPatota patota;
	patota.tamTareas = 0;
	patota.id = INICIAR_PATOTA;
	patota.posX=list_create();
	patota.posY=list_create();

	memcpy(&(patota.cantTripulantes), buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	int xs;
	int ys;
	char* tareas = string_new();
	char* tareaAux;

	memcpy(&(patota.cantTareas), buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	for (i=0; i<patota.cantTareas; i++) {
		memcpy(&largoProxTarea,buffer+desplazamiento,sizeof(int));
		desplazamiento+=sizeof(int);
		tareaAux = malloc(largoProxTarea);
		memcpy(tareaAux,buffer+desplazamiento,largoProxTarea);
		string_append(&tareas, tareaAux);
		if(i!=patota.cantTareas-1)string_append(&tareas, "|");
		free(tareaAux);
		desplazamiento+=largoProxTarea;
		patota.tamTareas += largoProxTarea+1;
	}

	patota.tareas=tareas;

	for (i=0; i<patota.cantTripulantes; i++) {
		memcpy(&xs,buffer+desplazamiento,sizeof(int));
		desplazamiento+=sizeof(int);
		memcpy(&ys,buffer+desplazamiento,sizeof(int));
		desplazamiento+=sizeof(int);

		list_add(patota.posX, &xs);
		list_add(patota.posY, &ys);
	}

	/*SEGMENTACION*/
	if (string_equals_ignore_case(infoConfig.esquemaMemoria, "SEGMENTACION")){
	/*-----------------------------TAREAS------------------------------------------------------*/
		int cantTareas = patota.tamTareas;
		int resultado=0;

		patotaControlBlock* pcb = crearPCB();

		int direccionTarea = dirLogica(cantTareas);

		if (direccionTarea == -1){
			sem_wait(semLogMiRAMHQ);
			log_error(logMiRAMHQ, "No hay lugar suficiente en memoria para almacenar las TAREAS");
			sem_post(semLogMiRAMHQ);
			resultado=1;
		}

		crearSegmentosTareas(cantTareas, direccionTarea, patota.tareas);
		free(patota.tareas);
		/*-----------------------------PCB------------------------------------------------------*/
		pcb->direccionLogicaTareas = direccionTarea;

		int direLogPCB = dirLogica(sizeof(patotaControlBlock));

		if (direLogPCB == -1){
			sem_wait(semLogMiRAMHQ);
			log_error(logMiRAMHQ, "No hay lugar suficiente en memoria para almacenar los PCB");
			sem_post(semLogMiRAMHQ);
			resultado=1;
		}

		crearSegmentosPCB(direLogPCB, pcb);
		/*-----------------------------TCB------------------------------------------------------*/
		t_list* listaTCBs = crearTCBs(patota, pcb, direLogPCB,argumentos->mapa);
		int tcbSize = list_size(listaTCBs);
		for(int i=0; i < tcbSize; i++){
			tripulanteControlBlock* tcbAGuardar = list_get(listaTCBs, i);

			int direLogTCB = dirLogica(21);

			if (direccionTarea == -1){
				sem_wait(semLogMiRAMHQ);
				log_error(logMiRAMHQ, "No hay lugar suficiente en memoria para almacenar los TCB");
				sem_post(semLogMiRAMHQ);
				resultado=1;
			}

			crearSegmentosTCB(direLogTCB, tcbAGuardar);
		}

		responderIniciarPatota(argumentos->conexion, resultado, listaTCBs);
<<<<<<< HEAD
		//realizarDump();

=======
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
		list_destroy_and_destroy_elements(listaTCBs, free);
		list_destroy(patota.posX);
		list_destroy(patota.posY);
		free(pcb);

		/*PAGINACION*/
	} else if(string_equals_ignore_case(infoConfig.esquemaMemoria, "PAGINACION")) {
<<<<<<< HEAD
		//TODO: Hay que buscar cuando se deja de usar este PCB y tirarle un free
		patotaControlBlock* pcb = crearPCBPag();
		//TODO: Hay que buscar cuando se deja de usar esta lista y tirarle un list_clean_and_destroy_elements
		t_list* listaTCBs = crearTCBs(patota, pcb, -1, argumentos->mapa);

		//sem_wait(semElementosEnDisco);
		int resultado = crearPaginas2(pcb, listaTCBs, patota.tareas);
		responderIniciarPatota(argumentos->conexion, resultado, listaTCBs);
		//sem_post(semElementosEnDisco);

//		free(pcb); rompe!!!
//		list_clean_and_destroy_elements(listaTCBs, free);
=======
//		//TODO: Hay que buscar cuando se deja de usar este PCB y tirarle un free
//		patotaControlBlock* pcb = crearPCB();
//		//TODO: Hay que buscar cuando se deja de usar esta lista y tirarle un list_clean_and_destroy_elements
//		t_list* listaTCBs = crearTCBs(patota, pcb, -1/*, argumentos->mapa*/);
//
//		sem_wait(semElementosEnDisco);
//		int resultado = crearPaginas(pcb, listaTCBs, patota.tareas, patota.cantTareas);
//		responderIniciarPatota(argumentos->conexion, resultado, listaTCBs);
//		sem_post(semElementosEnDisco);
//
////		free(pcb); rompe!!!
////		list_clean_and_destroy_elements(listaTCBs, free);
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
	}


/*EXPULSAR TRIPULANTE*/
} else if (tipoDeMensaje ==  EXPULSAR_TRIPULANTE){
		expulsarTripulante expulsarTripulante;
		segmentacion* segmentoAux;

		expulsarTripulante.idMensaje = EXPULSAR_TRIPULANTE;
		memcpy(&(expulsarTripulante.idTripulante), buffer + desplazamiento, sizeof(expulsarTripulante.idTripulante));

		int coincideIDyEsTcbSegmentacion (segmentacion* tcb){
			return tcb->elementoID == expulsarTripulante.idTripulante && tcb->tipo == TCB;
		}
		/*SEGMENTACION*/
		if (string_equals_ignore_case(infoConfig.esquemaMemoria, "SEGMENTACION")){

			sem_wait(semElementosEnDisco);
			segmentoAux = list_find(elementosEnDisco, (void*) coincideIDyEsTcbSegmentacion );
			if (segmentoAux == NULL) {
				sem_wait(semLogMiRAMHQ);
				log_error(logMiRAMHQ, "El tripulante no existe en memoria");
				sem_post(semLogMiRAMHQ);
				free(segmentoAux);
			} else {
				list_remove_and_destroy_by_condition(elementosEnDisco,(void*) coincideIDyEsTcbSegmentacion, (void*) destruirSegmento );
				sem_wait(semLogMiRAMHQ);
				log_info(logMiRAMHQ, "El tripulante fue eliminado correctamente");
				sem_post(semLogMiRAMHQ);
				sem_wait(semMapa);
	//			item_borrar(argumentos->mapa, expulsarTripulante.idTripulante);
				sem_post(semMapa);
			}
			sem_post(semElementosEnDisco);

//

			/*PAGINACION*/
		}else if (string_equals_ignore_case(infoConfig.esquemaMemoria, "PAGINACION")){

			elemento* elementoAux = malloc(sizeof(elemento));

			int coincideIDyEsTcbPaginacion (elemento* tcb){
				return tcb->elementoID == expulsarTripulante.idTripulante && tcb->tipo == TCB;
			}

			// Buscar en lista de elementos el TCB
			sem_wait(semElementosEnDisco);
			elementoAux = list_find(elementosEnDisco, (void*) coincideIDyEsTcbPaginacion);
			sem_post(semElementosEnDisco);

			// Para cada pagina que ocupe el TCB buscar si es el unico que ocupaba esa pagina
			t_list* paginasOcupadasPorTCB = list_create();
			paginasOcupadasPorTCB = obtenerPaginasDelDictionary(elementoAux->nropaginasConInicioYTam);

			t_list_iterator* list_iterator = list_iterator_create(paginasOcupadasPorTCB);

			while(list_iterator_has_next(list_iterator)){

				int* paginaOcupadaPorTCB = list_iterator_next(list_iterator);

				bool contienePagina (int pagina){
					return pagina == *paginaOcupadaPorTCB;
				}

				bool estaEnLaPagina (elemento* elemento){
					return list_any_satisfy(obtenerPaginasDelDictionary(elemento->nropaginasConInicioYTam), (void*) contienePagina);
				}

				bool buscarPagina (pagina* paginaEnDisco){
					return paginaEnDisco->nroPagina == *paginaOcupadaPorTCB;
				}

			// Si es el unico que ocupaba esa pagina, elimino las estructuras administrativas y lo elimino de disco
				if(list_count_satisfying(elementosEnDisco, (void*) estaEnLaPagina ) == 1){
					pagina *paginaTemp = malloc(sizeof(pagina));
					sem_wait(semPaginasEnDisco);
					paginaTemp = list_find(paginasEnDisco, (void*) buscarPagina);
					sem_post(semPaginasEnDisco);

					sem_wait(semLugaresLibresEnDisco);
					list_add(lugaresLibresEnDisco, &(paginaTemp->lugarEnDisco)); // Aca lo elimino de disco
					sem_post(semLugaresLibresEnDisco);

					sem_wait(semPaginasEnDisco);
					list_remove_and_destroy_by_condition(paginasEnDisco, (void*) buscarPagina, (void*) destruirElemento);
					sem_post(semPaginasEnDisco);
				}
			}

			sem_wait(semElementosEnDisco);
			list_remove_and_destroy_by_condition(elementosEnDisco, (void*) coincideIDyEsTcbPaginacion, (void*) destruirElemento);
			sem_post(semElementosEnDisco);

			sem_wait(semMapa);
//			item_borrar(argumentos->mapa, expulsarTripulante.idTripulante);
			sem_post(semMapa);
		}


/*SOLICITAR TAREA*/
}else if(tipoDeMensaje == SOLICITAR_TAREA){
		solicitarTarea tarea;
		tarea.idMensaje = SOLICITAR_TAREA;

		memcpy(&(tarea.idTripulante), buffer + desplazamiento, sizeof(uint32_t));


		/*SEGMENTACION*/
		if (string_equals_ignore_case(infoConfig.esquemaMemoria, "SEGMENTACION")){

			tcbYtareas* tcbYtareas = obtenerTareasSegunTripulante(tarea.idTripulante);

			tripulanteControlBlock* tripulante = tcbYtareas->tcb;
			char* tareas = tcbYtareas->tareas;
			segmentacion* segmentoTripu = tcbYtareas->segmentoTripu;

			char** tareasSeparadas = string_split(tareas, "|");
			char* tareaAEnviar;
			if (tareasSeparadas[tripulante->proximaInstruccion]==NULL) tareaAEnviar="FIN";
			else tareaAEnviar = tareasSeparadas[tripulante->proximaInstruccion];
			actualizarTareasTCBSegmentos(tripulante, segmentoTripu);

			int bytesAEnviar = string_length(tareaAEnviar)+1;
			send(argumentos->conexion, &bytesAEnviar, sizeof(int), 0);
			send(argumentos->conexion, tareaAEnviar, bytesAEnviar, 0);

			free(tareas);
			frezee(tareasSeparadas);
			free(tcbYtareas->tcb);
			free(tcbYtareas);

		/*PAGINACION*/
		} else if (string_equals_ignore_case(infoConfig.esquemaMemoria, "PAGINACION")){

//			elemento* elementoTripulante;
//			tripulanteControlBlock* tripulante;
//			//El malloc se hace dentro de la funcion deserializarTCB(). Dejar asi, ya esta revisado.
//			char* tareas = obtenerTareasSegunTripuEnPaginacion(tarea.idTripulante, tripulante, elementoTripulante);
//
//			char** tareasSeparadas = string_split(tareas, "|");
//			char* tareaAEnviar;
//			if (tareasSeparadas[tripulante->proximaInstruccion]==NULL) tareaAEnviar="FIN";
//			else tareaAEnviar = tareasSeparadas[tripulante->proximaInstruccion];
//
//			tripulante->proximaInstruccion++;
//			actualizarTCBPaginacion(tripulante, elementoTripulante);
//
//			int bytesAEnviar = string_length(tareaAEnviar)+1;
//			send(argumentos->conexion, &bytesAEnviar, sizeof(int), 0);
//			send(argumentos->conexion, tareaAEnviar, bytesAEnviar, 0);
		}


/*INFORMAR MOVIMIENTO*/
}else if(tipoDeMensaje == INFORMAR_MOVIMIENTO){
		informarMovimiento mov;
		mov.idMensaje = INFORMAR_MOVIMIENTO;

		memcpy(&(mov.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento+=sizeof(int);
		memcpy(&(mov.posX), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento+=sizeof(int);
		memcpy(&(mov.posY), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento+=sizeof(int);

		tripulanteControlBlock tripulante;

		tripulante.tid = mov.idTripulante;
		tripulante.posicionX = mov.posX;
		tripulante.posicionY = mov.posY;
		tripulante.estado = 'E';

		/*SEGMENTACION*/
		if(string_equals_ignore_case(infoConfig.esquemaMemoria, "SEGMENTACION")) {

			actualizarTCBSegmentos(tripulante,mov);

			sem_wait(semMapa);
//			item_desplazar(argumentos->mapa, tripulante.tid, tripulante.posicionX, tripulante.posicionY);
			sem_post(semMapa);


			/*PAGINACION*/
		} else if (string_equals_ignore_case(infoConfig.esquemaMemoria, "PAGINACION")) {

			elemento* elementoTripulante = obtenerElementoSegunTCB(tripulante.tid);

			actualizarSusPaginas(elementoTripulante);
			actualizarTCBPaginacion(&tripulante, elementoTripulante);

			sem_wait(semMapa);

//			item_desplazar(argumentos->mapa, tripulante->tid, tripulante->posicionX, tripulante->posicionY);
			sem_post(semMapa);
		}

//		free(tripulante);
	}else {
		sem_wait(semLogMiRAMHQ);
		log_error(logMiRAMHQ, "Error en mensaje");
		sem_post(semLogMiRAMHQ);
	}

	free(buffer);
}


void destruirElemento (elemento* elementoADestruir){
	free(elementoADestruir);
}


void responderIniciarPatota(int cliente, int valorOK, t_list* tripulantes) {

	if(valorOK == 1) {
		send(cliente, &valorOK, sizeof(int), 0);

	} else if(valorOK == 0) {
		send(cliente, &valorOK, sizeof(int), 0);

		sem_wait(semLogMiRAMHQ);
		log_info(logMiRAMHQ, "SE RECIBIO BIEN INICIAR PATOTA");
		sem_post(semLogMiRAMHQ);

		int i;
		int tam = list_size(tripulantes);
		void* buffer = malloc(sizeof(uint32_t)*tam);
		uint32_t idAEnviar;
		tripulanteControlBlock* tripulante;

		for(i = 0; i < tam; i++) {
			tripulante = list_get(tripulantes,i);
			idAEnviar = tripulante->tid;
			memcpy(buffer+sizeof(uint32_t)*i,&idAEnviar,sizeof(uint32_t));
		}

		send(cliente, buffer, sizeof(uint32_t)*tam, 0);
		free(buffer);
	}

}

void responderSolicitarTarea(int cliente, uint32_t idTripulante, char* mensaje) {

	send(cliente, mensaje, sizeof(char), 0);

	sem_wait(semLogMiRAMHQ);
	log_info(logMiRAMHQ, "ENVIANDO SOLICITAR_TAREA OK DEL TRIPULANTE: ", idTripulante);
	sem_post(semLogMiRAMHQ);

}

patotaControlBlock* crearPCB() {
	patotaControlBlock* patota = malloc(sizeof(patotaControlBlock));

	sem_wait(semIdPatota);
	patota->pid = idPatota;
	idPatota++;
	sem_post(semIdPatota);

	patota->direccionLogicaTareas = 0;

	return patota;
}

tripulanteControlBlock* crearTCB(patotaControlBlock* pcb, uint32_t posX, uint32_t posY, int i, int direLogPCB) {

	tripulanteControlBlock* tripulante = malloc(sizeof(tripulanteControlBlock));

	tripulante->estado = 'N';
	tripulante->posicionX = posX;
	tripulante->posicionY = posY;
	tripulante->proximaInstruccion = 0;
	if(string_equals_ignore_case(infoConfig.esquemaMemoria, "SEGMENTACION")){
		tripulante->dirLogicaPCB = direLogPCB;
	}else tripulante->dirLogicaPCB = pcb->pid;


	sem_wait(semIdTripulante);
	tripulante->tid = idTripulantes;
	idTripulantes++;
	sem_post(semIdTripulante);

	return tripulante;
}

t_list* crearTCBs(iniciarPatota patota, patotaControlBlock* pcb, int direLogPCB, NIVEL* mapa) {

	t_list* listaTCBs = list_create();

	for (int i=0; i < patota.cantTripulantes; i++) {
		int* x = list_get(patota.posX, i);
		int* y = list_get(patota.posY, i);
		tripulanteControlBlock* tcb = crearTCB(pcb, *x, *y, i, direLogPCB);

		list_add(listaTCBs, tcb);

//		sem_wait(semMapa);
//		personaje_crear(mapa, tcb->tid, *x, *y);
//		nivel_gui_dibujar(mapa);
//		sem_post(semMapa);
	}

	return listaTCBs;
}

void actualizarPCB(int dirLogicaTareas, patotaControlBlock pcb) {

	pcb.direccionLogicaTareas = dirLogicaTareas;

}

int calcularTamanioTareas(iniciarPatota patota){

	int tamanioTareas;

	tamanioTareas = sizeof(patota.tareas);

	return tamanioTareas;

}

int dirLogica(int tamanio){
	int direccionTarea = obtenerDireccionLogicaInicio(tamanio);

	if (direccionTarea == -1){
		compactar();
		direccionTarea = obtenerDireccionLogicaInicio(tamanio);

	}

	return direccionTarea;
}

void frezee(char** array){

	for(int i = 0; array[i] != NULL; i++){
		free(array[i]);
	}
	free(array);
}

