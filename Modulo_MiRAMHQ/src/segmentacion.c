/*
 * segmentacion.c
 *
 *  Created on: 6 jul. 2021
 *      Author: utnso
 */

#include "segmentacion.h"


void crearSegmentosTareas(int tamanio, int direccion, char* tareas) {

	segmentacion* segmento = malloc(sizeof(segmentacion));

	segmento->elementoID = asignadorIdTareas;
	segmento->inicio = direccion;
	segmento->tamanio = tamanio;
	segmento->tipo = TAREAS;

	asignadorIdTareas++;

	sem_wait(semElementosEnDisco);
	list_add(elementosEnDisco, segmento);
	sem_post(semElementosEnDisco);

	sem_wait(semMemoria);
	void* dirFisica = punteroAMemoria + segmento->inicio;
	memcpy(dirFisica, tareas, segmento->tamanio);
	sem_post(semMemoria);

}

void crearSegmentosPCB(int direccionLogica, patotaControlBlock* patota) {
	segmentacion* segmento = malloc(sizeof(segmentacion));

	segmento->inicio = direccionLogica;
	segmento->tamanio = sizeof(patotaControlBlock);
	segmento->tipo = PCB;
	segmento->elementoID = patota->pid;

	sem_wait(semElementosEnDisco);
	list_add(elementosEnDisco, segmento);
	sem_post(semElementosEnDisco);

	sem_wait(semMemoria);
	void* dirFisica = punteroAMemoria + segmento->inicio;
	memcpy(dirFisica, patota, segmento->tamanio);
	sem_post(semMemoria);
}

void crearSegmentosTCB(int direccion, tripulanteControlBlock* tripulante){
	segmentacion* segmento = malloc(sizeof(segmentacion));

	segmento->inicio = direccion;
	segmento->tamanio = sizeof(tripulanteControlBlock);
	segmento->elementoID = tripulante->tid;
	segmento->tipo = TCB;

	sem_wait(semElementosEnDisco);
	list_add(elementosEnDisco, segmento);
	sem_post(semElementosEnDisco);

	sem_wait(semMemoria);
	void* dirFisica = punteroAMemoria + segmento->inicio;
<<<<<<< HEAD
	memcpy(dirFisica+despl, &(tripulante->tid), sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(dirFisica+despl, &(tripulante->estado), sizeof(char));
	despl += sizeof(char);
	memcpy(dirFisica+despl, &(tripulante->posicionX), sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(dirFisica+despl, &(tripulante->posicionY), sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(dirFisica+despl, &(tripulante->proximaInstruccion), sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(dirFisica+despl, &(tripulante->dirLogicaPCB), sizeof(uint32_t));
	despl += sizeof(uint32_t);
=======
	memcpy(dirFisica, tripulante, segmento->tamanio);
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
	sem_post(semMemoria);

}

bool hayEspacioEntreSegmentos (segmentacion* primerSegmento, uint32_t inicioSegundoSegmento, int tamanioSegmentoAAgregar){
	return inicioSegundoSegmento - (primerSegmento->inicio + primerSegmento->tamanio)  > tamanioSegmentoAAgregar;
}

int espacioEntreSegmentos (segmentacion* primerSegmento, uint32_t inicioSegundoSegmento, int tamanioSegmentoAAgregar){
	//Si hay espacio para meter el segmento nuevo, devuelvo el espacio total entre segmentos, sino -1
	if(inicioSegundoSegmento - (primerSegmento->inicio + primerSegmento->tamanio)  > tamanioSegmentoAAgregar)
		return inicioSegundoSegmento - (primerSegmento->inicio + primerSegmento->tamanio);
	else
		return -1;
}

int calcularTamanioRestanteEnMemoria(segmentacion* ultimoSegmento, int tamanioSegmentoAAgregar){
	if(infoConfig.tamanioMemoria - (ultimoSegmento->inicio + ultimoSegmento->tamanio) > tamanioSegmentoAAgregar )
		return infoConfig.tamanioMemoria - (ultimoSegmento->inicio + ultimoSegmento->tamanio);
	else
		return -1;
}

int obtenerDireccionLogicaInicio(int tamanio){

	t_list* segmentosOrdenados = ordenarSegmentosPorDirLogica(elementosEnDisco);
	int cantSegmentosEnMemoria = list_size(segmentosOrdenados);
	int mejorEspacioLibreEntreSegmentos = 10000000;
	int mejorIndice=0;
	int espacioLibreEntreSegmentos = 0;
	bool encontreLugarFlag = false;
	segmentacion* primerSegmento;
	segmentacion* segundoSegmento;

	// Si es el primer segmento
	if (cantSegmentosEnMemoria == 0) {
		list_destroy(segmentosOrdenados);
		return 0;}
	// Si es el segundo segmento
	else if (cantSegmentosEnMemoria == 1) {
		segmentacion* seg = list_get(segmentosOrdenados,0);
		list_destroy(segmentosOrdenados);
		return seg->tamanio;
	}

	else if (string_equals_ignore_case(infoConfig.criterioSeleccion, "FF")){

		for(int i = 0; i < (list_size(segmentosOrdenados) - 1) && (!encontreLugarFlag) ; i++){
			primerSegmento = list_get(segmentosOrdenados, i);
			segundoSegmento = list_get(segmentosOrdenados, i+1);

			encontreLugarFlag = hayEspacioEntreSegmentos(primerSegmento, segundoSegmento->inicio, tamanio);

			if(encontreLugarFlag) {
				list_destroy(segmentosOrdenados); return primerSegmento->inicio + primerSegmento->tamanio; }
			//Si estoy en la ultima iteracion me fijo si hay espacio al final del ultimo segmento
			else if(i == ( list_size(segmentosOrdenados) - 2) && calcularTamanioRestanteEnMemoria(segundoSegmento, tamanio) > tamanio)
				{list_destroy(segmentosOrdenados); return segundoSegmento->inicio + segundoSegmento->tamanio;}
		}
		list_destroy(segmentosOrdenados);
		return -1;
	}

	else if (string_equals_ignore_case(infoConfig.criterioSeleccion, "BF")){

		for(int i = 0; i < (list_size(segmentosOrdenados) - 1) ; i++){
			primerSegmento = list_get(segmentosOrdenados, i);
			segundoSegmento = list_get(segmentosOrdenados, i+1);
			//Si hay espacio para meter el segmento nuevo, devuelvo el espacio total entre segmentos, sino -1
			if(segundoSegmento->inicio - (primerSegmento->inicio + primerSegmento->tamanio)  > tamanio)
				espacioLibreEntreSegmentos = segundoSegmento->inicio - (primerSegmento->inicio + primerSegmento->tamanio);
			else
				espacioLibreEntreSegmentos = -1;
			// espacioLIbre = -1 significa que no habia espacio libre entre los segmentos
			if( espacioLibreEntreSegmentos < mejorEspacioLibreEntreSegmentos && espacioLibreEntreSegmentos != -1 ){
				mejorEspacioLibreEntreSegmentos = espacioLibreEntreSegmentos;
				mejorIndice = i;
			}
			//Si estoy en la ultima iteracion me fijo si hay espacio al final del ultimo segmento, y si ese espacio es mejor que el ultimo que tengo
			if(i == ( list_size(segmentosOrdenados) - 2) && calcularTamanioRestanteEnMemoria(segundoSegmento, tamanio) > tamanio && calcularTamanioRestanteEnMemoria(segundoSegmento, tamanio) < mejorEspacioLibreEntreSegmentos){
				mejorEspacioLibreEntreSegmentos = calcularTamanioRestanteEnMemoria(segundoSegmento, tamanio);
				mejorIndice = i+1;
			}
		}

		if(mejorEspacioLibreEntreSegmentos == -1) {
			list_destroy(segmentosOrdenados);
			return mejorEspacioLibreEntreSegmentos; }
		else{
			primerSegmento = list_get(segmentosOrdenados, mejorIndice);
			list_destroy(segmentosOrdenados);
			return primerSegmento->inicio + primerSegmento->tamanio;
		}
	}
	list_destroy(segmentosOrdenados);
	return -1;
}

//La direcion de inicio es la FISICA
void cargarSegmentoEnMemoria(char* inicio, void* elemento){
	sem_wait(semMemoria);
	memcpy(inicio, elemento, sizeof(void));
	sem_post(semMemoria);
}

void compactar() {

	sem_wait(semMemoria);
	
	t_list* segmentosOrdenados = ordenarSegmentosPorDirLogica(elementosEnDisco);

	if (list_size(segmentosOrdenados) == 0){
		sem_wait(semLogMiRAMHQ);
		log_info(logMiRAMHQ, "No hay segmentos en memoria, no es necesario compactar");
		sem_post(semLogMiRAMHQ);
	}else{
		segmentacion* segmento1 = list_get(segmentosOrdenados, 0);

		if (segmento1->inicio != 0){
			
			void* dirFisica = punteroAMemoria + segmento1->inicio;
			memmove(punteroAMemoria, dirFisica, segmento1->tamanio);
			segmento1->inicio = 0;

			segmentacion* nuevoSeg = crearSegmentoAPartirDeOtro(segmento1);

			int coincideIDyTipo(segmentacion* segmento){
				return segmento->elementoID == segmento1->elementoID && segmento->tipo == segmento1->tipo;
			}

			sem_wait(semElementosEnDisco);
			list_remove_and_destroy_by_condition(elementosEnDisco, (void*) coincideIDyTipo, (void*) destruirSegmento );
			list_add(elementosEnDisco, nuevoSeg);
			sem_post(semElementosEnDisco);

		}

		if (list_size(segmentosOrdenados) > 1){

			int i;
			for (i = 1; i <= list_size(segmentosOrdenados); i++){

				segmentacion* SegmentoActual = list_get(segmentosOrdenados, i);
				segmentacion* SegmentoAnterior = list_get(segmentosOrdenados, i-1);

				if ( (SegmentoActual->inicio - (SegmentoAnterior->inicio + SegmentoAnterior->tamanio + 1)) >= 1){
					memmove((SegmentoAnterior->inicio + SegmentoAnterior->tamanio + 1), SegmentoActual->inicio, SegmentoActual->tamanio);
					SegmentoActual->inicio = SegmentoAnterior->inicio + SegmentoAnterior->tamanio + 1;

					segmentacion* nuevoSeg = crearSegmentoAPartirDeOtro(SegmentoActual);

					int coincideIDyTipo(segmentacion* segmento){
						return segmento->elementoID == SegmentoActual->elementoID && segmento->tipo == SegmentoActual->tipo;
					}

					sem_wait(semElementosEnDisco);
					list_remove_and_destroy_by_condition(elementosEnDisco, (void*) coincideIDyTipo, (void*) destruirSegmento );
					list_add(elementosEnDisco, nuevoSeg);
					sem_post(semElementosEnDisco);
				}

			}
		}
	}
	list_destroy(segmentosOrdenados);

	sem_post(semMemoria);
}

void compactarPorqueRecibimosSenial(){
	compactar();
}

t_list* ordenarSegmentosPorDirLogica(t_list* segmentosEnMemoria){
	return list_sorted(segmentosEnMemoria, (void*) tieneMenorDirLogica);
}

//Devuelve si el primer segmento tiene menor direccion logica que el segundo
bool tieneMenorDirLogica (segmentacion* primerSegmento, segmentacion* segundoSegmento){
	return primerSegmento->inicio <= segundoSegmento->inicio;

}

void removerSegmentoDeMemoria(int valor){
	segmentacion* seg = list_get(elementosEnDisco, valor);

	sem_wait(semElementosEnDisco);
	list_remove(elementosEnDisco, seg);
	sem_post(semElementosEnDisco);

}

void actualizarTCBSegmentos(tripulanteControlBlock tripulante, informarMovimiento posicionesNuevas){

	tripulanteControlBlock* tcbAux;
//	segmentacion* segmentoAux = malloc(sizeof(segmentacion));
	segmentacion* segmentoAux;

	int coincideID (segmentacion* tcb){
		return tcb->elementoID == tripulante.tid  ? 1 : 0; //Cuando hagamos las pruebas, probar si se puede sacar el operador ternario y solo dejar la comparacion por ==
	}



	int coincideIDyEsTCB (segmentacion* tcb){
		return tcb->elementoID == tripulante.tid && tcb->tipo == TCB;
	}
	sem_wait(semElementosEnDisco);
	segmentoAux = list_find(elementosEnDisco, (void*) coincideIDyEsTCB);
	sem_post(semElementosEnDisco);

	tcbAux = leerSegmentoDeMemoria(segmentoAux);

	tcbAux->posicionX = posicionesNuevas.posX;
	tcbAux->posicionY = posicionesNuevas.posY;
	tcbAux->estado = 'E';

	escribirSegmentoEnMemoria(segmentoAux, tcbAux );
	free(tcbAux);

//	sem_wait(semElementosEnDisco);
//	list_add(elementosEnDisco, segmentoAux);
//	list_remove_by_condition(elementosEnDisco, (void*) coincideIDyEsTCB);
//	sem_post(semElementosEnDisco);

}


void actualizarTareasTCBSegmentos(tripulanteControlBlock* tripulante, segmentacion* segmento){

	tripulanteControlBlock* tripuAux  = malloc(sizeof(tripulanteControlBlock));
	tripuAux->dirLogicaPCB = tripulante->dirLogicaPCB;
	tripuAux->estado = tripulante->estado;
	tripuAux->posicionX = tripulante->posicionX;
	tripuAux->posicionY = tripulante->posicionY;
	tripuAux->proximaInstruccion = tripulante->proximaInstruccion+1;
	tripuAux->tid = tripulante->tid;

	sem_wait(semMemoria);
	memcpy(punteroAMemoria+segmento->inicio, tripuAux, segmento->tamanio);
	sem_post(semMemoria);

	free(tripuAux);
}


void* obtenerDireccionFisica (uint32_t direccionLogica){
	return punteroAMemoria + direccionLogica;
}

void* leerSegmentoDeMemoria (segmentacion* segmentoOrigen){
	sem_wait(semMemoria);
	void* segmentoEnMemoria = malloc(segmentoOrigen->tamanio);
	memcpy(segmentoEnMemoria, obtenerDireccionFisica(segmentoOrigen->inicio), segmentoOrigen->tamanio);//TODO
	sem_post(semMemoria);

	return segmentoEnMemoria;
}

void escribirSegmentoEnMemoria (segmentacion* segmentoDestino, tripulanteControlBlock* elementoAAgregar){
	sem_wait(semMemoria);
	memcpy(obtenerDireccionFisica(segmentoDestino->inicio), elementoAAgregar, segmentoDestino->tamanio);
	sem_post(semMemoria);
}

segmentacion* crearSegmentoAPartirDeOtro (segmentacion* segmentoOrigen){
	segmentacion* segmentoDestino = malloc(sizeof(segmentacion));

	segmentoDestino->inicio = segmentoOrigen->inicio;
	segmentoDestino->tamanio = segmentoOrigen->tamanio;
	segmentoDestino->tipo = segmentoOrigen->tipo;
	segmentoDestino->elementoID = segmentoOrigen->elementoID;

	return segmentoDestino;
}


void destruirSegmento (segmentacion* segmentoADestruir){
	free(segmentoADestruir);
}

t_list* devolverSegmentosConTCB(){
	return list_filter(elementosEnDisco, (void*) tieneTCB);
}

bool tieneTCB(segmentacion* segmento){
	return segmento->tipo == TCB;
}


tcbYtareas* obtenerTareasSegunTripulante(int tripulante){

	char* tareas;
	tcbYtareas* tcbYtareas = malloc(sizeof(tcbYtareas));
	segmentacion* segmentoTripu = malloc(sizeof(segmentacion));



	bool segmentoTCBConId(segmentacion* seg){
		if(seg->tipo == TCB && seg->elementoID == tripulante){
			return true;
		}else return false;
	}
	sem_wait(semElementosEnDisco);
	segmentoTripu = list_find(elementosEnDisco, (void*) segmentoTCBConId);
	sem_post(semElementosEnDisco);

	tripulanteControlBlock* tripu = malloc(sizeof(tripulanteControlBlock));

	sem_wait(semMemoria);
<<<<<<< HEAD

	int despl = 0;
	void* dirFisica = punteroAMemoria + segmentoTripu->inicio;
	memcpy(&(tripu->tid), dirFisica+despl, sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(&(tripu->estado), dirFisica+despl, sizeof(char));
	despl += sizeof(char);
	memcpy(&(tripu->posicionX), dirFisica+despl, sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(&(tripu->posicionY), dirFisica+despl, sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(&(tripu->proximaInstruccion), dirFisica+despl, sizeof(uint32_t));
	despl += sizeof(uint32_t);
	memcpy(&(tripu->dirLogicaPCB), dirFisica+despl, sizeof(uint32_t));
	despl += sizeof(uint32_t);

=======
	memcpy(tripu, punteroAMemoria+segmentoTripu->inicio, segmentoTripu->tamanio);
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
	patotaControlBlock* pcb = malloc(sizeof(patotaControlBlock));
	memcpy(pcb, punteroAMemoria+tripu->dirLogicaPCB, sizeof(patotaControlBlock));
	sem_post(semMemoria);

	bool segmentoTareasConDirLog(segmentacion* seg){
		if(seg->tipo == TAREAS && seg->inicio == pcb->direccionLogicaTareas){
			return true;
		}else return false;
	}

	segmentacion* segmentoTareas = malloc(sizeof(segmentacion));
	sem_wait(semElementosEnDisco);
<<<<<<< HEAD
	segmentoTareas = list_find(elementosEnDisco, (void*) segmentoTareasConDirLog);
=======
	segmentacion* segmentoTareas = list_find(elementosEnDisco, (void*) segmentoTareasConDirLog);
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
	sem_post(semElementosEnDisco);

	tareas = malloc(segmentoTareas->tamanio);

	sem_wait(semMemoria);
	memcpy(tareas, punteroAMemoria + segmentoTareas->inicio, segmentoTareas->tamanio);
	sem_post(semMemoria);

	free(pcb);
	tcbYtareas->tcb=tripu;
	tcbYtareas->tareas=tareas;
	tcbYtareas->segmentoTripu=segmentoTripu;
	return tcbYtareas;
}

