/*
 * paginacion.c
 *
 *  Created on: 6 jul. 2021
 *      Author: utnso
 */


#include "paginacion.h"

int calcularCantidadDeMarcos(){
	return infoConfig.tamanioMemoria / infoConfig.tamanioPagina;
}

void crearBitmapPaginacion(void* punteroAMemoria, int tamanioMemoria, int tamanioPagina){
	int cantFrames = tamanioMemoria / tamanioPagina;
	int sizeBitmap = (cantFrames/8) + 1;

	void* bitmapAux = malloc(sizeBitmap);
	bitmap = bitarray_create(bitmapAux, sizeBitmap);
	for(int i = 0; i < cantFrames; i++) {
		bitarray_clean_bit(bitmap, i);
	}

	free(bitmapAux);
}

void cargarPaginaEnMemoria(int nroFrame, void* elemento){
	sem_wait(semMemoria);
	void* dirFisica = punteroAMemoria + nroFrame*infoConfig.tamanioPagina;
	memcpy(dirFisica, elemento, sizeof(void)); //TODO: Que onda esto
	sem_post(semMemoria);
}

void crearArchivoSWAP(infoConfigMiRAMHQ infoConfig){
	archivoSWAP = open(infoConfig.pathSwap, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(archivoSWAP, infoConfig.tamanioSwap);
}


//Retorna el lugar donde se empezo a escribir la pagina
ssize_t escribirEnArchivoSwapPorPrimeraVez(void* pag, int desplazamiento) {
	ssize_t sizeFinal;
	//TODO: Aca hay que fijarse primero si hay algun elemento en lugaresLibresEnDisco. SI hay, hay que escribir ahi
	if(list_size(lugaresLibresEnDisco) > 0){
		int* lugar = list_get(lugaresLibresEnDisco, 0);
		sem_wait(semArchivoSwap);
		pwrite(archivoSWAP, pag + desplazamiento, infoConfig.tamanioPagina, *lugar);
		sem_post(semArchivoSwap);
		list_remove(lugaresLibresEnDisco, 0);
	}else{
		sem_wait(semArchivoSwap);
		ssize_t size = write(archivoSWAP, pag + desplazamiento, infoConfig.tamanioPagina);
		sizeFinal = cantEscritoEnDisco;
		cantEscritoEnDisco += size;
		sem_post(semArchivoSwap);
	}
	return sizeFinal;
}

void* leerArchivoSwap(int lugarEnDisco) {//RECIBE EL LUGAR DEL DISCO DE LA PAGINA QUE SE QUIERE TRAER
	void* pagina = malloc(infoConfig.tamanioPagina);
	int aux = lugarEnDisco * infoConfig.tamanioPagina;
	lseek(archivoSWAP, aux, SEEK_SET);
	read(archivoSWAP, pagina, infoConfig.tamanioPagina);

	return pagina;
}

void cerrarArchivoSWAP(){
	close(archivoSWAP);
}

void eliminarDeArchivoSwap(){

}


// ESTO TIENE QUE SER OBTENER DIRECCION FISICA
int obtenerDireccionLogicaEnPaginacion(int nroPagina, int nroProceso){

	int resultado;
	if (string_equals_ignore_case(infoConfig.algoritmoReemplazo, "LRU")){
		sem_wait(semAlgoritmo);
		resultado = algoritmoLRU(nroPagina, nroProceso);
		sem_post(semAlgoritmo);
		return resultado;
	}else{
		sem_wait(semAlgoritmo);
		resultado = algoritmoClock(nroPagina, nroProceso);
		sem_post(semAlgoritmo);
		return resultado;
	}
}


int algoritmoLRU(int nroPagina, int nroProceso){

	bool pagEstaEnMemoria(pagina* pag){
		return (pag->nroPagina == nroPagina && pag->identificadorDeProceso == nroProceso);
	}

	t_list* paginasPresentesEnMemoria = obtenerPaginasEnMemoria();
	pagina* pag = list_find(paginasPresentesEnMemoria, (void*) pagEstaEnMemoria);

	if(pag != NULL){
		return pag->nroMarco;
	}

	int cantidadDeMarcosTotales = infoConfig.tamanioMemoria/infoConfig.tamanioPagina;
	int cantidadDeMarcosOcupados = list_size(paginasPresentesEnMemoria);
	if(cantidadDeMarcosOcupados < cantidadDeMarcosTotales){
		return cantidadDeMarcosOcupados;
	}


	bool comparandoInstRefer(pagina* pag1, pagina* pag2){
		return primeraHoraMenorALaSegunda(pag1->instanteReferencia, pag2->instanteReferencia);
	}

	t_list* paginasOrdenadasPorInstanteDeReferencia = list_sorted(paginasPresentesEnMemoria, (void*) comparandoInstRefer);

	pagina* paginaAFletar = list_get(paginasOrdenadasPorInstanteDeReferencia, 0);

	bool buscarPaginaPorNroPagina (pagina* pagina, int nroProceso){
		return pagina->nroPagina == nroPagina && pagina->identificadorDeProceso == nroProceso;
	}

	sincronizarConDisco(paginaAFletar);

	sem_wait(semPaginasEnDisco);
	list_remove_and_destroy_by_condition(paginasEnDisco, (void*) buscarPaginaPorNroPagina, (void*) destruirPagina );
	list_add(paginasEnDisco, paginaAFletar);
	sem_post(semPaginasEnDisco);

	return paginaAFletar->nroMarco;
}

void destruirPagina(pagina* paginaADestruir){
	free(paginaADestruir);
}

int algoritmoClock(int nroPagina, int nroProceso){
	sem_wait(semPaginasEnDisco);

	pagina* paginaAux;
	t_list* paginasPresentesEnMemoria = obtenerPaginasEnMemoria();
	t_list* paginasPresentesEnMemoriaOrdenadasPorMarco = ordenarPaginaPorMarcos(paginasPresentesEnMemoria);

	//Nos fijamos si ya esta presente en memoria
	paginaAux = buscarPaginaEnMemoria(paginasPresentesEnMemoria, nroPagina);

	//Si la pagina esta presente en memoria, la traigo de ahi
	if(paginaAux != NULL){
		bool buscarPaginaPorNroPagina (pagina* pagina){
			return pagina->nroPagina == nroPagina && pagina->identificadorDeProceso == nroProceso;
		}
		list_remove_and_destroy_by_condition(paginasEnDisco, (void*) buscarPaginaPorNroPagina, (void*) destruirPagina );
		paginaAux->bitUso = 1;
		paginaAux->instanteReferencia = temporal_get_string_time("%H:%M:S");

		list_add(paginasEnDisco, paginaAux);

		return paginaAux->nroMarco;
	}

	//Si no esta presente en memoria pero hay un marco libre, la cargo
	int cantidadDeMarcosTotales = infoConfig.tamanioMemoria/infoConfig.tamanioPagina;
	int cantidadDeMarcosOcupados = list_size(paginasPresentesEnMemoria);
	if(cantidadDeMarcosOcupados < cantidadDeMarcosTotales){
		return cantidadDeMarcosOcupados;
	}

	// En estas dos lineas hago que el primer elemento de la lista sea el apuntado por el indice
	t_list* listaAux = list_take_and_remove(paginasPresentesEnMemoriaOrdenadasPorMarco, indiceClock);
	list_add_all(paginasPresentesEnMemoriaOrdenadasPorMarco, listaAux);
	t_list_iterator* paginasEnMemoriaIterator = list_iterator_create(paginasPresentesEnMemoriaOrdenadasPorMarco);

	paginaAux = list_get(paginasPresentesEnMemoriaOrdenadasPorMarco, 0);

	if(puedoReemplazarPagina(paginaAux, nroProceso))
		return paginaAux->nroMarco;

	// Si la primer pagina no es reemplazable, itero toda la lista
	while( list_iterator_has_next(paginasEnMemoriaIterator) ){
		paginaAux = list_iterator_next(paginasEnMemoriaIterator);
		if(puedoReemplazarPagina(paginaAux, nroProceso))
			return paginaAux->nroMarco;
	}

	paginaAux = list_get(paginasPresentesEnMemoriaOrdenadasPorMarco, 0);

	if(puedoReemplazarPagina(paginaAux, nroProceso))
		return paginaAux->nroMarco;

	sem_post(semPaginasEnDisco);

	return -1; // Porque es un error
}

t_list* ordenarPaginacionPorInstanteDeReferenciaMasLejano(t_list* paginasEnMemoria) {

	t_list* pagOrdenados = NULL;
	int i;
	pagina* pag;
	pagina* paginaAnterior;


	// PODEMOS USAR LIST_SORTED ACA??
	for (i=0; i < list_size(paginasEnMemoria); i++) {
		pag = list_get(paginasEnMemoria, i);
		paginaAnterior = list_get(paginasEnMemoria, i+1);
		list_sort( paginasEnMemoria, (void*) esMasViejo);
	}


	return pagOrdenados;
}

//
// En principio que los instantes tengan en cuenta solo la hora, minutos y segundos. EN el siguiente formato --> %H:%M:%S
// Pregunto si el primerInstante es mas viejo con respecto al segundo
bool esMasViejo(char* primerInstante, char* segundoInstante){
// SI primerInstante es un string menor a segundoInstante quiere decir que el primer instante es mas viejo que el segundo
// if(>0) es true, cualquier otro numero es false
	if( strncmp(primerInstante,segundoInstante, 8) )
		return true;
	else
		return false;

}

// Si devuelvo true es porque encontre una pagina que puedo reemplazar
bool puedoReemplazarPagina(pagina* paginaAux, int nroProceso){
	sem_wait(semPaginasEnDisco);

	//Si el bit de uso es igual a 0, sincronizo y saco la pagina de memoria
	if(! paginaAux->bitUso){
		paginaAux->bitPresencia = false;
		if(paginaAux->bitModificado)
			sincronizarConDisco(paginaAux);
	}
	else
		paginaAux->bitUso = false;

	bool buscarPaginaPorNroPagina (pagina* pagina){
		return pagina->nroPagina == paginaAux->nroPagina;
	}

	list_remove_and_destroy_by_condition(paginasEnDisco, (void*) buscarPaginaPorNroPagina, (void*) destruirPagina );

	list_add(paginasEnDisco, paginaAux );

	incrementarNumeroIndice();

	sem_post(semPaginasEnDisco);

	return !(paginaAux->bitPresencia);

}

void traerPagADisco(pagina* pag, int nroFrame){

	void* paginaEnDisco = leerArchivoSwap(pag->lugarEnDisco);
	sem_wait(semMemoria);
	void* dirFisica = punteroAMemoria + nroFrame*infoConfig.tamanioPagina;
	memcpy(dirFisica, paginaEnDisco, infoConfig.tamanioPagina);
	sem_post(semMemoria);

}


void sincronizarConDisco(pagina* pag){
	sem_wait(semMemoria);
	sem_wait(semArchivoSwap);
	void* dirFisica = punteroAMemoria + pag->nroMarco*infoConfig.tamanioPagina;
	void* paginaEnMem = malloc(infoConfig.tamanioPagina);
	memcpy(paginaEnMem, dirFisica, infoConfig.tamanioPagina);
	sem_post(semMemoria);

	pwrite(archivoSWAP, paginaEnMem, infoConfig.tamanioPagina, pag->lugarEnDisco);
	sem_post(semArchivoSwap);

	free(paginaEnMem);
}


//Tenemos que retornar un t_list de las paginas con bitPresencia = 1

t_list* obtenerPaginasEnMemoria(){
	return list_filter(paginasEnDisco, (void*) esIgualA);
}

bool esIgualA(pagina* pagina){
	return pagina->bitPresencia == 1;
}

t_list* obtenerMarcosEnMemoria(t_list* paginasEnMemoria){

	t_list* marcosEnMemoria = list_create();
	pagina* pagina;
	int cantidadPaginasEnMemoria = list_size(paginasEnMemoria);

	for (int i = 0; i < cantidadPaginasEnMemoria; i++){
		pagina = list_get(paginasEnMemoria, i);
		if (pagina->bitPresencia == 1){
			list_add(marcosEnMemoria, &(pagina->nroMarco));
		}

	}

	return marcosEnMemoria;
}



//TODO: NICO FIJATE SI ESTO SE PUEDE BORRAR O LO VAS A USAR AL FINAL

//t_list* filtrarPorTipoId(t_list* listaDePagsEnMemoria, int id, tipoDeEstructura tipo){
//	tipoEId t;
//	t.id = id;
//	t.tipo = tipo;
//
//	bool filtrarTipoId(pagina* pagina){
//		if(pagina->id == t.id && pagina->tipo == t.tipo){
//			return true;
//		}else return false;
//	}
//
//	return list_filter(listaDePagsEnMemoria, (void*) filtrarTipoId);
//}

pagina* buscarPaginaEnMemoria (t_list* paginasPresentesEnMemoria, int nroPagina){
	int estaPresente(pagina* pagina){
		return pagina->nroPagina == nroPagina ? 1 : 0;
	}
	pagina* pag = list_find(paginasPresentesEnMemoria, (void*) estaPresente);
	return pag;
}

pagina* buscarPaginaEnDisco (int nroPagina){
	int estaPresente(pagina* pagina){
		return pagina->nroPagina == nroPagina ? 1 : 0;
	}
	pagina* pag = malloc(sizeof(pagina));
	pag = list_find(paginasEnDisco, (void*) estaPresente);
	return pag;
}

void incrementarNumeroIndice(){//NO HACE FALTA SEM -> POR ALGOERITMO
	if (indiceClock == (calcularCantidadDeMarcos()-1))
		indiceClock = 0;
	else indiceClock++;
}
//
//void escribirPcbEnPagina(void* paginaAEscribir, pagina* infoPagina, patotaControlBlock* pcb, int* numeroPagina, int* desplazamientoDeLaPag){
//	crearPagina(paginaAEscribir, infoPagina, pcb->pid, numeroPagina);
//
//	agregarAPag(paginaAEscribir, desplazamientoDeLaPag, &pcb->pid,sizeof(pcb->pid) );
//	pcb->direccionLogicaTareas = 0;
//	agregarAPag(paginaAEscribir, desplazamientoDeLaPag, &pcb->direccionLogicaTareas,sizeof(pcb->direccionLogicaTareas) );
//
//	inicioYtam infoPags;
//	infoPags.inicio = 0;
//	infoPags.tamanio = sizeof(patotaControlBlock);
//
//	elemento* elem = crearElemento(pcb->pid, numeroPagina - 1, pcb->pid, PCB, infoPags);
//
//	sem_wait(semElementosEnDisco);
//	list_add(elementosEnDisco,  elem);
//	sem_post(semElementosEnDisco);
//	free (elem);
//}
//
//
int espacioLibreEnDisco (){
	return infoConfig.tamanioSwap - ( cantEscritoEnDisco - list_size(lugaresLibresEnDisco) * infoConfig.tamanioPagina );
}
//
int espacioQueOcupaPagina(int cantidadTripulantes, int largoTareas){
	return sizeof(patotaControlBlock) + sizeof(tripulanteControlBlock) * cantidadTripulantes + largoTareas;
}
//
//int crearPaginas(patotaControlBlock* pcb, t_list* listaTCBs, char* tareas, int largoTareas){
//	// Si no hay lugar en disco retorno 1
//	if( espacioLibreEnDisco() < espacioQueOcupaPagina(list_size(listaTCBs), largoTareas ) )
//		return 1;
//
//	int tamanioDePagina = infoConfig.tamanioPagina;
//	int numeroPagina = 0;
//	pagina* infoPagina;
//	void* contenidoPagina;
//	int desplazamientoDeLaPag = 0;
//
//	escribirPcbEnPagina(contenidoPagina, infoPagina, pcb, &numeroPagina, &desplazamientoDeLaPag);
//
//	int espacioQueQueda;
//
//	void escribirTcbEnPagina (tripulanteControlBlock* tcb){
//
//		int pagActual;
////		if(desplazamientoDeLaPag != 0){
//			pagActual = numeroPagina-1;
////		}
//
//		inicioYtam	infoPagTCB;
//		infoPagTCB.inicio = desplazamientoDeLaPag;
//		espacioQueQueda = tamanioDePagina - desplazamientoDeLaPag;
//		// Si el TCB se va a poner en toda la pagina
//		if( espacioQueQueda >= sizeof(tripulanteControlBlock) )
//			infoPagTCB.tamanio = sizeof(tripulanteControlBlock);
//		// Si el TCB se va a dividir en mas de solo la pagina actual
//		else
//			infoPagTCB.tamanio = espacioQueQueda;
//
//
//
//		elemento* elemAux = crearElemento(tcb->tid , &pagActual, pcb->pid, TCB, infoPagTCB);
//
//		seguirAgregando(contenidoPagina, &desplazamientoDeLaPag, &(tcb->tid), infoPagina, espacioQueQueda, pcb->pid,
//						&numeroPagina, sizeof(tcb->tid), elemAux);
//
//		espacioQueQueda = tamanioDePagina - desplazamientoDeLaPag;
//		seguirAgregando(contenidoPagina, &desplazamientoDeLaPag, &(tcb->estado), infoPagina, espacioQueQueda, pcb->pid,
//						&numeroPagina, sizeof(tcb->estado), elemAux);
//
//		espacioQueQueda = tamanioDePagina - desplazamientoDeLaPag;
//		seguirAgregando(contenidoPagina, &desplazamientoDeLaPag, &(tcb->posicionX), infoPagina, espacioQueQueda, pcb->pid,
//						&numeroPagina, sizeof(tcb->posicionX), elemAux);
//
//		espacioQueQueda = tamanioDePagina - desplazamientoDeLaPag;
//		seguirAgregando(contenidoPagina, &desplazamientoDeLaPag, &(tcb->posicionY), infoPagina, espacioQueQueda, pcb->pid,
//						&numeroPagina, sizeof(tcb->posicionY), elemAux);
//
//		espacioQueQueda = tamanioDePagina - desplazamientoDeLaPag;
//		seguirAgregando(contenidoPagina, &desplazamientoDeLaPag, &(tcb->proximaInstruccion), infoPagina, espacioQueQueda, pcb->pid,
//						&numeroPagina, sizeof(tcb->proximaInstruccion), elemAux);
//
//		espacioQueQueda = tamanioDePagina - desplazamientoDeLaPag;
//		seguirAgregando(contenidoPagina, &desplazamientoDeLaPag, &(tcb->dirLogicaPCB), infoPagina, espacioQueQueda, pcb->pid,
//						&numeroPagina, sizeof(tcb->dirLogicaPCB), elemAux);
//
//		sem_wait(semElementosEnDisco);
//		list_add(elementosEnDisco, elemAux);
//		sem_post(semElementosEnDisco);
//
//		free(elemAux);
//	}
//
//	list_iterate(listaTCBs, (void*) escribirTcbEnPagina);
//
//	escribirTareaEnPagina(contenidoPagina, &desplazamientoDeLaPag, infoPagina, tareas, &numeroPagina, largoTareas, pcb->pid);
//
//	//free(contenidoPagina);
//	return 0;
//}
//
//
//
////pag es la pagina que va a disco e infoPagina es la estructura administrativa
//void seguirAgregando(void* pag, int* desplazamientoDeLaPag,void* info, pagina* infoPagina, int espacioQueQuedaLibreEnPagina, int idPCB,
//					int* numeroPagina, int tamanioTotal, elemento* elementoAux ){
//
//	if(espacioQueQuedaLibreEnPagina >= tamanioTotal)
//		agregarAPag(pag, desplazamientoDeLaPag, info, tamanioTotal);
//	else{
//		if(espacioQueQuedaLibreEnPagina != 0)
//			agregarAPag(pag, desplazamientoDeLaPag, info, espacioQueQuedaLibreEnPagina);
//		infoPagina->lugarEnDisco = escribirEnArchivoSwapPorPrimeraVez(pag);
//
//		sem_wait(semPaginasEnDisco);
//		list_add(paginasEnDisco, infoPagina);
//		sem_post(semPaginasEnDisco);
//
//		int loQFaltaEscribir = tamanioTotal - espacioQueQuedaLibreEnPagina;
//
//		free(pag);
//		free(infoPagina);
//
//		crearPagina(pag, infoPagina, idPCB, numeroPagina);
//		*desplazamientoDeLaPag = 0;
//		inicioYtam	infoPagTCB;
//		infoPagTCB.inicio = 0;
//		infoPagTCB.tamanio = loQFaltaEscribir;
//		// NumeroPAgina -1 porque siempre tiene el numero de la pagina siguiente
//		char numPagString[5];
//		sprintf(numPagString, "%d", (*numeroPagina) - 1);
//		dictionary_put(elementoAux->nropaginasConInicioYTam, numPagString, &infoPagTCB);
//		free(numPagString);
//		agregarAPag(pag, desplazamientoDeLaPag, (info+espacioQueQuedaLibreEnPagina), loQFaltaEscribir);
//	}
//
//}
//
//
//void agregarAPag(void* pagina, int* desplazamientoDeLaPag,void* info, int tamanio){
//	sem_wait(semMemoria);
//	memcpy(pagina + *desplazamientoDeLaPag, info, tamanio);
//	*desplazamientoDeLaPag += tamanio;
//	sem_post(semMemoria);
//}

elemento* crearElemento(uint32_t id, int* numeroPagina, uint32_t pid, tipoDeEstructura tipo, inicioYtam*  infoPags){
	elemento* info = malloc(sizeof(elemento));

	info->elementoID = id;
	info->nropaginasConInicioYTam = dictionary_create();
	char* numPagString = string_itoa(*numeroPagina);
	dictionary_put(info->nropaginasConInicioYTam, numPagString, infoPags);
	info->identificadorDeProceso = pid;
	info->tipo = tipo;
	free(numPagString);
	return info;
}

//
//void escribirTareaEnPagina(void* contenidoPagina, int* desplazamientoDeLaPag, pagina* infoPagina,
//							char* tareas,int* numeroPagina,int largoTareas, int idPCB){
//	elemento* elementoAux;
//	int pagActual = (*numeroPagina)-1;
//	int tamanioDePagina = infoConfig.tamanioPagina;
//	int espacioQueQuedaLibreEnPagina = tamanioDePagina - (*desplazamientoDeLaPag);
//	inicioYtam	infoPagTareasAux;
//	infoPagTareasAux.inicio = *desplazamientoDeLaPag;
//	int cantParcialEscrita = 0;
//	char* numPagString;
//
//	//Si la pagina NO esta llena
//	if(*desplazamientoDeLaPag < infoConfig.tamanioPagina){
//		//Si NO entran todas las tareas en la pagina
//		if(espacioQueQuedaLibreEnPagina <= largoTareas){
//			agregarAPag(contenidoPagina, desplazamientoDeLaPag, tareas, espacioQueQuedaLibreEnPagina);
//			cantParcialEscrita = espacioQueQuedaLibreEnPagina;
//			infoPagTareasAux.tamanio = cantParcialEscrita;
//		// Si entran todas las tareas en la pagina
//		}else{
//			agregarAPag(contenidoPagina, desplazamientoDeLaPag, tareas, largoTareas);
//			cantParcialEscrita = largoTareas;
//			infoPagTareasAux.tamanio = largoTareas;
//		}
//
//		elementoAux = crearElemento(idPCB , &pagActual, idPCB, TAREAS, infoPagTareasAux);
//
//		// Aca o se lleno la pagina y quedan tareas por escribir
//		//O se escribieron todas las tareas y ya se guarda la pagina aunque quede espacio porque es lo ultimo
//		infoPagina->lugarEnDisco = escribirEnArchivoSwapPorPrimeraVez(contenidoPagina);
//
//		sem_wait(semPaginasEnDisco);
//		list_add(paginasEnDisco, infoPagina);
//		sem_post(semPaginasEnDisco);
//
//		numPagString = malloc(5);
//		sprintf(numPagString, "%d", pagActual);
//		dictionary_put(elementoAux->nropaginasConInicioYTam, numPagString, &infoPagTareasAux);
//		free(numPagString);
//
//	}
//
//	// Si quedan tareas por escribir o si la pagina estaba llena
//	while(cantParcialEscrita < largoTareas){
//		crearPagina(contenidoPagina, infoPagina, idPCB, numeroPagina);
//		*desplazamientoDeLaPag = 0;
//
//		void* tareaAEscribir = tareas+cantParcialEscrita;
//		int cantAEscribir = largoTareas-cantParcialEscrita;
//		// Si lo que queda ocupa mas de una pagina entonces escribo toda la pagina y no me paso
//		if(cantAEscribir > infoConfig.tamanioPagina){
//			cantAEscribir = infoConfig.tamanioPagina;
//		}
//		agregarAPag(contenidoPagina, desplazamientoDeLaPag, tareaAEscribir, cantAEscribir);
//		cantParcialEscrita += cantAEscribir;
//
//		infoPagina->lugarEnDisco = escribirEnArchivoSwapPorPrimeraVez(contenidoPagina);
//
//		infoPagTareasAux.inicio=0;
//		infoPagTareasAux.tamanio=cantAEscribir;
//		numPagString = malloc(5);
//		sprintf(numPagString, "%d", (*numeroPagina) - 1);
//		dictionary_put(elementoAux->nropaginasConInicioYTam, numPagString, &infoPagTareasAux);
//
//		sem_wait(semPaginasEnDisco);
//		free(numPagString);
//		list_add(paginasEnDisco, infoPagina);
//		sem_post(semPaginasEnDisco);
//
//	}
//
//	sem_wait(semElementosEnDisco);
//	list_add(elementosEnDisco, elementoAux);
//	sem_post(semElementosEnDisco);
//
//	free(elementoAux);
//	free(numPagString);
//
//}
//
//void crearPagina(void* pag, pagina* infoPagina, int id, int* numPag){
//
//	infoPagina = malloc(sizeof(pagina));
//	pag = malloc(infoConfig.tamanioPagina);
//
//	infoPagina->bitModificado = false;
//	infoPagina->bitPresencia = false;
//	infoPagina->bitUso = false;
//	infoPagina->identificadorDeProceso = id;
//	infoPagina->instanteReferencia = "0";
//	infoPagina->nroMarco = -1;
//	infoPagina->nroPagina = *numPag;
//	infoPagina->lugarEnDisco = -1;
//
//	(*numPag)++;
//
//
//}

t_list* ordenarPaginaPorMarcos(t_list* paginasEnMemoria){
	return list_sorted(paginasEnMemoria, (void*) tieneMenorMarco);
}

//Devuelve si el primer segmento tiene menor direccion logica que el segundo
bool tieneMenorMarco (pagina* primeraPagina, pagina* segundoPagina){
	return primeraPagina->nroMarco <= segundoPagina->nroMarco;

}

bool existeNumeroDeMarcoEnMemoria(t_list* marcosEnMemoria, int numeroMarco){

	bool coincideMarcos(uint32_t marcoEnMemoria) {
		return marcoEnMemoria == numeroMarco;
	}

	return list_any_satisfy(marcosEnMemoria,(void*) coincideMarcos);

}


pagina* obtenerLaPaginaDelMarco(t_list* paginasEnMemoria, int numeroMarco){

	bool coincideNumeroMarco(pagina* pagina){
		return pagina->nroMarco == numeroMarco;
	}

	return list_find(paginasEnMemoria, (void*) coincideNumeroMarco);

}

elemento* obtenerElementoDeTareas(int pid){
	bool segunIdTarea(elemento* elemento){
		if(elemento->elementoID == pid && elemento->tipo == TAREAS){
			return true;
		} else{
			return false;
		}
	}

	elemento* elem = list_find(elementosEnDisco, (void*) segunIdTarea);

	return elem;
}

elemento* obtenerElementoSegunTCB(int tripulante){

	elemento* elem = malloc(sizeof(elemento));

	bool segunIdTCB(elemento* elemento){
		if(elemento->elementoID == tripulante && elemento->tipo == TCB){
			return true;
		} else{
			return false;
		}
	}

	elem = list_find(elementosEnDisco, (void*) segunIdTCB);

	return elem;
}

pagina* obtenerEstructuraPaginaSegunElemento(int nroPag, int idProceso){

	bool segunNroPagina(pagina* pagAux){
		if(pagAux->nroPagina == nroPag && pagAux->identificadorDeProceso == idProceso){
			return true;
		} else{
			return false;
		}
	}

	pagina* pag = list_find(paginasEnDisco, (void*) segunNroPagina);

	return pag;
}



void actualizarTCBPaginacion(tripulanteControlBlock* tripulante, elemento* elementoTripulante){
	sem_wait(semMemoria);

	t_list* paginasDelElementoTripulante = obtenerPaginasDelDictionary(elementoTripulante->nropaginasConInicioYTam);

	void* tcbSerializado = serializarTCB(tripulante);
	int desplazamiento = 0;
	//Recorro las paginas en las cuales el TCB tiene informacion
	for(int i=0; i<list_size(paginasDelElementoTripulante); i++){
		int* nroPagina = list_get(paginasDelElementoTripulante, i);
		char* keyPagina = string_itoa(*nroPagina);

		inicioYtam* info = dictionary_get(elementoTripulante->nropaginasConInicioYTam, keyPagina);
		//retorno la pagina en disco
		pagina* pagDeTripu = buscarPaginaEnDisco(*nroPagina);
		//Obtengo un puntero que apunta a un buffer con la parte del TCB que estaba en esa pagina
		void* pagina = obtenerInfoDeLaPag(pagDeTripu->nroMarco, info);
		//Copio eso que estaba en la pagina a
		memcpy(pagina, tcbSerializado+desplazamiento, info->tamanio);
<<<<<<< HEAD
		sem_post(semMemoria);
=======
		//TODO: Aca seguro haya que mover el desplazamiento
		sem_wait(semMemoria);
>>>>>>> b3bd1f7b7f63050f45e3a964262776568e1b25a4
		desplazamiento += info->tamanio;
		sem_post(semMemoria);

	}

}

void* obtenerInfoDeLaPag(int frame, inicioYtam* infoPag){
	void* info = malloc(infoPag->tamanio);
	void* dirFisica = punteroAMemoria + (frame*infoConfig.tamanioPagina) + infoPag->inicio;
	memcpy(info, dirFisica, infoPag->tamanio);

	return info;
}



variasCosas* obtenerTareasSegunTripuEnPaginacion(int idTripulante){

	variasCosas* variasCosas = malloc(sizeof(variasCosas));

	elemento* elementoTripulante = obtenerElementoSegunTCB(idTripulante);
	elemento* elementoTareas = obtenerElementoDeTareas(elementoTripulante->identificadorDeProceso);
	
	t_list* paginasDelElementoTripulante = obtenerPaginasDelDictionary(elementoTripulante->nropaginasConInicioYTam);
	t_list* paginasDelElementoTareas = obtenerPaginasDelDictionary(elementoTareas->nropaginasConInicioYTam);

	void* contenidoTcbEnMemoria = obtenerInfoCompletaDePaginas(elementoTripulante, paginasDelElementoTripulante, sizeof(tripulanteControlBlock), true);
	tripulanteControlBlock* tripulante = deserializarTCB(contenidoTcbEnMemoria);
//	tripulanteControlBlock* tripulante = obtenerTCBCompletoDePaginas(elementoTripulante, paginasDelElementoTripulante, sizeof(tripulanteControlBlock));

	int tamTareas = tamTareasEnPaginas(elementoTareas, paginasDelElementoTareas);
	char* tareas = obtenerInfoCompletaDePaginas(elementoTareas, paginasDelElementoTareas, sizeof(char)*tamTareas, false);

	variasCosas->tcb=tripulante;
	variasCosas->tareas=tareas;
	variasCosas->elemento=elementoTripulante;

	return variasCosas;
}


void* obtenerInfoCompletaDePaginas(elemento* elementoDeEstructura, t_list* paginasDelElementoDeEstructura, int tamanioDeEstructura, bool bitDeModif){
	void* estructuraADevolver = malloc(tamanioDeEstructura);
	int desplazamiento = 0;

	for(int i=0; i<list_size(paginasDelElementoDeEstructura); i++){
		int* nroPagina = list_get(paginasDelElementoDeEstructura, i);
		char* keyPagina = string_itoa(*nroPagina);

		inicioYtam* info = dictionary_get(elementoDeEstructura->nropaginasConInicioYTam, keyPagina);
		pagina* pagDeTripu = buscarPaginaEnDisco(*nroPagina);

		int nroFrame = obtenerDireccionLogicaEnPaginacion(*nroPagina, pagDeTripu->identificadorDeProceso);
		if(pagDeTripu->bitPresencia == 0) traerPagADisco(pagDeTripu, nroFrame);


		actualizarPagina(pagDeTripu, nroFrame, bitDeModif);


		void* pagina = obtenerInfoDeLaPag(nroFrame, info);
		memcpy(estructuraADevolver+desplazamiento, pagina, info->tamanio);
		desplazamiento += info->tamanio;
	}

	return estructuraADevolver;
}


void actualizarPagina(pagina* paginaAActualizar, int nroFrame, bool bitDeModif){

	paginaAActualizar->bitModificado = bitDeModif;
	paginaAActualizar->bitPresencia = true;
	paginaAActualizar->bitUso = true;
	paginaAActualizar->instanteReferencia = temporal_get_string_time("%H:%M:%S:%MS");
	paginaAActualizar->nroMarco = nroFrame;

	bool buscarPaginaPorNroPagina (pagina* pagina){
		return pagina->nroPagina == paginaAActualizar->nroPagina;
	}
	sem_wait(semPaginasEnDisco);
//	list_remove_and_destroy_by_condition(paginasEnDisco, (void*) buscarPaginaPorNroPagina, (void*) destruirPagina );
	list_remove_by_condition(paginasEnDisco, (void*) buscarPaginaPorNroPagina);
	list_add(paginasEnDisco, paginaAActualizar);
	sem_post(semPaginasEnDisco);
}

void actualizarSusPaginas(elemento* elementoTripulante){
	t_list* paginasDelElementoTripulante = obtenerPaginasDelDictionary(elementoTripulante->nropaginasConInicioYTam);

	for(int i=0; i<list_size(paginasDelElementoTripulante); i++){
		int* nroPagina = list_get(paginasDelElementoTripulante, i);
		pagina* pagDeTripu = buscarPaginaEnDisco(*nroPagina);

		int nroFrame = obtenerDireccionLogicaEnPaginacion(*nroPagina, pagDeTripu->identificadorDeProceso);
		if(pagDeTripu->bitPresencia == 0) traerPagADisco(pagDeTripu, nroFrame);

		actualizarPagina(pagDeTripu, nroFrame, true);
	}
}

int tamTareasEnPaginas(elemento* elementoTareas,t_list* paginasDelElementoTareas){
	int tamanio = 0;

	for(int i=0; i<list_size(paginasDelElementoTareas); i++){
		int* nroPagina = list_get(paginasDelElementoTareas, i);
		char* keyPagina = string_itoa(*nroPagina);

		inicioYtam* info = dictionary_get(elementoTareas->nropaginasConInicioYTam, keyPagina);
		tamanio += info->tamanio;
	}

	return tamanio;
}


bool primeraHoraMenorALaSegunda(char* hora1, char* hora2){

	char** horaSeparada1 = string_split(hora1, ":");
	char** horaSeparada2 = string_split(hora1, ":");

	int h1 = atoi(horaSeparada1[0]);
	int h2 = atoi(horaSeparada2[0]);

	if(h1 < h2){
		return true;
	}else if(h1 > h2){
		return false;
	}else{
		int m1 = atoi(horaSeparada1[1]);
		int m2 = atoi(horaSeparada2[1]);

		if(m1 < m2){
			return true;
		}else if(m1 > m2){
			return false;
		}else{
			int s1 = atoi(horaSeparada1[2]);
			int s2 = atoi(horaSeparada2[2]);

			if(s1 < s2){
				return true;
			}else if(s1 > s2){
				return false;
			}else{
				int ms1 = atoi(horaSeparada1[3]);
				int ms2 = atoi(horaSeparada2[3]);

				if(ms1 < ms2){
					return true;
				}else return false;

			}

		}
	}
}


t_list* obtenerPaginasDelDictionary(t_dictionary* dic){
	t_list* listaConPaginas = list_create();

	int cantDePagsTotales = list_size(paginasEnDisco);
	int* pag;
	for(int i=0; i<cantDePagsTotales; i++){
		pag = malloc(sizeof(int));
		memcpy(pag, &i, sizeof(int));
		char* keyPag = string_itoa(i);
		if(dictionary_has_key(dic, keyPag))
			list_add(listaConPaginas, pag);
		free(keyPag);
	}

	bool esMenorA(int* a, int* b){
		return *a < *b;
	}

	list_sort(listaConPaginas, (void*) esMenorA);

	return listaConPaginas;
}



tripulanteControlBlock* deserializarTCB(void* serializado){
	tripulanteControlBlock* tripulante = malloc(sizeof(tripulanteControlBlock));
	int desplazamiento = 0;

	memcpy(&(tripulante->estado), serializado+desplazamiento, sizeof(char));
	desplazamiento += sizeof(char);
	memcpy(&(tripulante->tid), serializado+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(tripulante->posicionX), serializado+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(tripulante->posicionY), serializado+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(tripulante->proximaInstruccion), serializado+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&(tripulante->dirLogicaPCB), serializado+desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return tripulante;
}



int calcularPaginasDadaUnaCantidadDeBytes(int tamanioTotalAsociadaAlProceso){
	if(tamanioTotalAsociadaAlProceso % infoConfig.tamanioPagina == 0)
		return tamanioTotalAsociadaAlProceso/infoConfig.tamanioPagina;
	else
		return ( tamanioTotalAsociadaAlProceso/infoConfig.tamanioPagina )+ 1;
}

void agregarPaginaADiccionario(t_dictionary* diccionarioAModificar, int nroPagina,
							int tamanioEnPagina, int inicioPagina){
	inicioYtam* infoElementoAux = malloc(sizeof(inicioYtam));
	infoElementoAux->inicio = inicioPagina;
	infoElementoAux->tamanio = tamanioEnPagina;
	char* numPagString = string_itoa(nroPagina);
	dictionary_put(diccionarioAModificar, numPagString, infoElementoAux);
	free(numPagString);
}



int crearPaginas2(patotaControlBlock* pcb, t_list* listaTCBs, char* tareas){

	// Si no hay lugar en disco retorno 1
	if( espacioLibreEnDisco() < espacioQueOcupaPagina(list_size(listaTCBs), string_length(tareas)+1 ) )
		return 1;

	int nroPag = 0;
	int tamanioTotalAsociadaAlProceso = 0;
	int desplazamiento = 0;
	void* infoARepartirEnPaginas = serializarLaPatotaCompleta(pcb, listaTCBs, tareas,
																&tamanioTotalAsociadaAlProceso);
	// Este while es para cargar las estructuras de pagina
	while(tamanioTotalAsociadaAlProceso > desplazamiento){
		pagina* infoPag = crearPagina2(pcb->pid, &nroPag);
		infoPag->lugarEnDisco = escribirEnArchivoSwapPorPrimeraVez(infoARepartirEnPaginas, desplazamiento);
		desplazamiento += infoConfig.tamanioPagina;
		sem_wait(semPaginasEnDisco);
		list_add(paginasEnDisco, infoPag);
		sem_post(semPaginasEnDisco);
	}

	int contadorElementoAux = 0;
	nroPag=0;

	tripulanteControlBlock* tcbAux;
	elemento* elementoAux;
	inicioYtam* infoElementoAux = malloc(sizeof(inicioYtam));
	int paginaActual = 0;
	// ** ------------Cargar elementos PCB -------------  **
	infoElementoAux->inicio = contadorElementoAux;
	infoElementoAux->tamanio = sizeof(patotaControlBlock);
	elementoAux = crearElemento(pcb->pid, &paginaActual, pcb->pid, PCB, infoElementoAux );
	contadorElementoAux += infoElementoAux->tamanio;
	sem_wait(semElementosEnDisco);
	list_add(elementosEnDisco,  elementoAux);
	sem_post(semElementosEnDisco);
	// ** ------------Cargar elementos TCB -------------  **
	t_list_iterator* listaTCBsIterator = list_iterator_create(listaTCBs);
	while( list_iterator_has_next(listaTCBsIterator) ){
		tcbAux = malloc(sizeof(tripulanteControlBlock));
		tcbAux = list_iterator_next(listaTCBsIterator);
		infoElementoAux->inicio = contadorElementoAux;
		// Si entra el TCB entero en la pagina
		if(sizeof(tripulanteControlBlock) <= ( infoConfig.tamanioPagina - contadorElementoAux ) ){
			infoElementoAux->tamanio = tamanioTCB();
			elementoAux = crearElemento(tcbAux->tid, &paginaActual, pcb->pid, TCB, infoElementoAux );
			contadorElementoAux += infoElementoAux->tamanio;

			inicioYtam* dsa = dictionary_get(elementoAux->nropaginasConInicioYTam, "0");
			printf("inicio: %d\n",dsa->inicio);
			printf("tamanio: %d\n",dsa->tamanio);

		}
		// Si no entra en lo que queda de la pagina
		else{
			infoElementoAux->tamanio = infoConfig.tamanioPagina - contadorElementoAux;
		// Si el tamanio te da 0, significa que la pagina esta llena y deberia crear el elemento con la pagina+1
			if(infoElementoAux->tamanio==0){
				int salvaPapas = paginaActual + 1;
				elementoAux = crearElemento(tcbAux->tid, &salvaPapas, pcb->pid, TCB, infoElementoAux );
			}
			else
				elementoAux = crearElemento(tcbAux->tid, &paginaActual, pcb->pid, TCB, infoElementoAux );
			contadorElementoAux = 0;
			paginaActual++;
			int loQueQuedaDelTCB = tamanioTCB() - infoElementoAux->tamanio;
			agregarPaginaADiccionario(elementoAux->nropaginasConInicioYTam, paginaActual, loQueQuedaDelTCB , 0);
			contadorElementoAux += loQueQuedaDelTCB;
		}
		sem_wait(semElementosEnDisco);
		list_add(elementosEnDisco,  elementoAux);
		sem_post(semElementosEnDisco);

//		free (elementoAux); // TODO: Revisar si este free va aca o afuera del while
//		free(tcbAux); // TODO: Revisar si este free va aca o afuera del while
	}
	// ** ------------Cargar elemento Tareas -------------  **
	int contadorAuxTareas;
	infoElementoAux->inicio = contadorElementoAux;
	infoElementoAux->tamanio = infoConfig.tamanioPagina - contadorElementoAux;
	elementoAux = crearElemento(pcb->pid, &paginaActual,	pcb->pid, TAREAS, infoElementoAux );
	contadorAuxTareas = infoElementoAux->tamanio;
	paginaActual++;
	while( strlen(tareas)+1 > contadorAuxTareas){
		// Si puedo llenar una pagina entera
		if( strlen(tareas)+1 - contadorAuxTareas >= infoConfig.tamanioPagina){
			agregarPaginaADiccionario(elementoAux->nropaginasConInicioYTam, paginaActual, infoConfig.tamanioPagina , 0);
			paginaActual++;
			contadorAuxTareas += infoConfig.tamanioPagina;
		}
		// Si no puedo llenar una pagina entera
		else{
			agregarPaginaADiccionario(elementoAux->nropaginasConInicioYTam, paginaActual,
									strlen(tareas)+1 - contadorAuxTareas , 0);
			contadorAuxTareas += (strlen(tareas)+1 - contadorAuxTareas);
		}
	}
	sem_wait(semElementosEnDisco);
	list_add(elementosEnDisco,  elementoAux);
	sem_post(semElementosEnDisco);
	//free(tcbAux); // TODO: Cuando ande todo fijarse si se puede hacer este free
	free(infoARepartirEnPaginas);
return 0;
}

int tamanioTCB (){
	return 21;
}

elemento* crearElemento2(uint32_t id, uint32_t pid, tipoDeEstructura tipo){
	elemento* elem =  malloc(sizeof(elemento));
	elem->elementoID = id;
	elem->identificadorDeProceso = pid;
	elem->nropaginasConInicioYTam = dictionary_create();
	elem->tipo = tipo;

	return elem;
}

pagina* crearPagina2(int pid, int* nroPag){

	pagina* infoPagina = malloc(sizeof(pagina));

	infoPagina->bitModificado = false;
	infoPagina->bitPresencia = false;
	infoPagina->bitUso = false;
	infoPagina->identificadorDeProceso = pid;
	infoPagina->instanteReferencia = "0";
	infoPagina->nroMarco = -1;
	infoPagina->nroPagina = *nroPag;
	infoPagina->lugarEnDisco = -1;
	(*nroPag)++;
	return infoPagina;
}


void guardarPcbContenidoEnMemoriaAux(void* aSerializar,int* desplazamiento, patotaControlBlock* pcb ){
	memcpy(aSerializar+ *desplazamiento, &(pcb->pid), sizeof(pcb->pid));
	*desplazamiento += sizeof(pcb->pid);
	memcpy(aSerializar+ *desplazamiento, &(pcb->direccionLogicaTareas), sizeof(pcb->direccionLogicaTareas));
	*desplazamiento += sizeof(pcb->direccionLogicaTareas);
}

void guardarTcbContenidoMemoriaAux( void* aSerializar, int* desplazamiento, tripulanteControlBlock* tcb ){

	memcpy(aSerializar+ *desplazamiento, &(tcb->estado), sizeof(tcb->estado));
	*desplazamiento += sizeof(tcb->estado);
	memcpy(aSerializar+ *desplazamiento, &(tcb->tid), sizeof(tcb->tid));
	*desplazamiento += sizeof(tcb->tid);
	memcpy(aSerializar+ *desplazamiento, &(tcb->posicionX), sizeof(tcb->posicionX));
	*desplazamiento += sizeof(tcb->posicionX);
	memcpy(aSerializar+ *desplazamiento, &(tcb->posicionY), sizeof(tcb->posicionY));
	*desplazamiento += sizeof(tcb->posicionY);
	memcpy(aSerializar+ *desplazamiento, &(tcb->proximaInstruccion), sizeof(tcb->proximaInstruccion));
	*desplazamiento += sizeof(tcb->proximaInstruccion);
	memcpy(aSerializar+ *desplazamiento, &(tcb->dirLogicaPCB), sizeof(tcb->dirLogicaPCB));
	*desplazamiento += sizeof(tcb->dirLogicaPCB);
}

void guardarListaTcbContenidoEnMemoriaAux(void* aSerializar, int* desplazamiento, t_list* listaTCBs ){
	t_list_iterator* listaTCBsIterator = list_iterator_create(listaTCBs);
	// El iterator arranca en -1
	while(list_iterator_has_next(listaTCBsIterator))
		guardarTcbContenidoMemoriaAux( aSerializar, desplazamiento, list_iterator_next( listaTCBsIterator ) );
}

void guardarTareasEnMemoriaAux(void* aSerializar, int* desplazamiento, char* tareas){
	memcpy(aSerializar+ *desplazamiento, tareas, string_length(tareas)+1);
	*desplazamiento += (string_length(tareas)+1);
}

void* serializarLaPatotaCompleta(patotaControlBlock* pcb, t_list* listaTCBs, char* tareas,
									int* tamanioTotalAsociadaAlProceso){
	void* aSerializar = malloc(sizeof(patotaControlBlock) + sizeof(tripulanteControlBlock)*list_size(listaTCBs)
						+ string_length(tareas)+1 );
	int desplazamiento = 0;

	guardarPcbContenidoEnMemoriaAux(aSerializar, &desplazamiento, pcb );
	guardarListaTcbContenidoEnMemoriaAux(aSerializar, &desplazamiento, listaTCBs );
	guardarTareasEnMemoriaAux(aSerializar, &desplazamiento, tareas);
	*tamanioTotalAsociadaAlProceso = desplazamiento;
	return aSerializar;
}

void* serializarTCB(tripulanteControlBlock* tripulante){
	void* aSerializar = malloc(tamanioTCB());
	int desplazamiento = 0;

	memcpy(aSerializar+desplazamiento, &(tripulante->estado), sizeof(char));
	desplazamiento += sizeof(char);
	memcpy(aSerializar+desplazamiento, &(tripulante->tid), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(aSerializar+desplazamiento, &(tripulante->posicionX), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(aSerializar+desplazamiento, &(tripulante->posicionY), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(aSerializar+desplazamiento, &(tripulante->proximaInstruccion), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(aSerializar+desplazamiento, &(tripulante->dirLogicaPCB), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return aSerializar;
}

