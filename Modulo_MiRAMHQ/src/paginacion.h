/*
 * paginacion.h
 *
 *  Created on: 6 jul. 2021
 *      Author: utnso
 */

#ifndef PAGINACION_H_
#define PAGINACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include <semaphore.h>

#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>

#include "mensajes.h"

typedef struct{
	int nroMarco;
	int nroPagina;
	int lugarEnDisco;
	bool bitPresencia;
	bool bitModificado;
	bool bitUso;
	char* instanteReferencia;
	uint32_t identificadorDeProceso;
}pagina;

typedef struct{
	uint32_t elementoID;
	t_dictionary* nropaginasConInicioYTam;// [key= nroPagina, value=Desde donde emmpieza el dato en la pagina y el tamanio en un struct]
	uint32_t identificadorDeProceso;
	tipoDeEstructura tipo;
}elemento;

typedef struct{
	uint32_t tamanio;
	uint32_t inicio;
}inicioYtam;

typedef struct{
	char* tareas;
	elemento* elemento;
	tripulanteControlBlock* tcb;
}variasCosas;


t_list* lugaresLibresEnDisco;
t_list* paginasEnDisco;

ssize_t cantEscritoEnDisco;


int instanteDeReferenciaActual;

int ultimoMarcoUsado;
int indiceClock;

t_list* indiceDePaginaSiguiente;

sem_t* semArchivoSwap;

t_bitarray* bitmap;

int archivoSWAP;

int calcularCantidadDeMarcos();
void crearBitmapPaginacion(void* punteroAMemoria, int tamanioMemoria, int tamanioPagina);

void cargarPaginaEnMemoria(int inicio, void* elemento);

t_list* ordenarPaginaPorMarcos(t_list* paginasEnMemoria);

bool tieneMenorMarco (pagina* primeraPagina, pagina* segundoPagina);

void crearArchivoSWAP(infoConfigMiRAMHQ infoConfig);
ssize_t escribirEnArchivoSwapPorPrimeraVez(void* pag, int desplazamiento);
void* leerArchivoSwap(int lugarEnDisco);
void cerrarArchivoSWAP();
void eliminarDeArchivoSwap();
int tamanioTCB();
bool pagEstaEnMemoria(pagina* pag);
bool comparandoInstRefer(pagina* pag1, pagina* pag2);
void destruirPagina(pagina* paginaADestruir);
int obtenerDireccionLogicaEnPaginacion(int tamanioTareas, int nroProceso);
int algoritmoLRU(int nroPagina, int nroProceso);
int algoritmoClock(int nroPagina, int nroProceso);

t_list* ordenarPaginacionPorInstanteDeReferenciaMasLejano(t_list* paginasEnMemoria);
bool esMasViejo(char* primerInstante, char* segundoInstante);

t_list* obtenerPaginasEnMemoria();
bool esIgualA(pagina* pagina);
void* obtenerInfoDeLaPag(int frame, inicioYtam* infoPag);

void agregarPaginaADiccionario(t_dictionary* diccionarioAModificar, int nroPagina,
							int tamanioEnPagina, int inicioPagina);
t_list* obtenerMarcosEnMemoria(t_list* paginasEnMemoria);
elemento* obtenerElementoSegunTCB(int tripulante);
pagina* obtenerEstructuraPaginaSegunElemento(int nroPag, int idProceso);
elemento* obtenerElementoDeTareas(int pid);

t_list* filtrarPorTipoId(t_list* listaDePagsEnMemoria, int id, tipoDeEstructura tipo);
pagina* buscarPaginaEnDisco (int nroPagina);
pagina* buscarPaginaEnMemoria (t_list* paginasPresentesEnMemoria, int nroPagina);
void traerPagADisco(pagina* pag, int nroFrame);
void incrementarNumeroIndice();

void escribirTareaEnPagina(void* contenidoPagina, int* desplazamientoDeLaPag, pagina* infoPagina,
		char* tareas,int* numeroPagina,int largoTareas, int idPCB);
void seguirAgregando(void* pag, int* desplazamientoDeLaPag,void* info, pagina* infoPagina, int espacioQueQueda, int idPCB, int* numeroPagina, int tamanioTotal, elemento* elementoAux);
void agregarAPag(void* pagina, int* desplazamientoDeLaPag,void* info, int tamanio);
int crearPaginas(patotaControlBlock* pcb, t_list* listaTCBs, char* tareas, int cantTareas);
void crearPagina(void* pag, pagina* infoPagina, int id, int* numPag);

void actualizarTCBPaginacion(tripulanteControlBlock* tripulante, elemento* elementoTripulante);
void removerPaginaDeMemoria(int valor);
int espacioLibreEnDisco ();
int espacioQueOcupaPagina(int cantidadTripulantes, int largoTareas);

int crearPaginas2(patotaControlBlock* pcb, t_list* listaTCBs, char* tareas);
void actualizarPagina(pagina* paginaAActualizar, int nroFrame, bool bitDeModif);
void actualizarSusPaginas(elemento* elementoTripulante);
bool puedoReemplazarPagina(pagina* paginaAux, int nroProceso);
void sincronizarConDisco(pagina* pag);

void escribirPcbEnPagina(void* paginaAEscribir, pagina* infoPagina, patotaControlBlock* pcb, int* numeroPagina, int* desplazamientoDeLaPag);
elemento* crearElemento(uint32_t id, int* numeroPagina, uint32_t pid, tipoDeEstructura tipo, inicioYtam* infoPags);

bool existeNumeroDeMarcoEnMemoria(t_list* marcosEnMemoria, int numeroMarco);
void destruirElemento (elemento* elementoADestruir);

variasCosas* obtenerTareasSegunTripuEnPaginacion(int idTripulante);
pagina* obtenerLaPaginaDelMarco(t_list* paginasEnMemoria, int numeroMarco);
void* obtenerInfoCompletaDePaginas(elemento* elementoDeEstructura, t_list* paginasDelElementoDeEstructura, int tamanioDeEstructura, bool bitDeModif);
int tamTareasEnPaginas(elemento* elementoTareas,t_list* paginasDelElementoTareas);

bool primeraHoraMenorALaSegunda(char* hora1, char* hora2);
t_list* obtenerPaginasDelDictionary(t_dictionary* dic);
tripulanteControlBlock* deserializarTCB(void* serializado);
void* serializarTCB(tripulanteControlBlock* tripulante);

pagina* crearPagina2(int pid, int* nroPag);
void* serializarLaPatotaCompleta(patotaControlBlock* pcb, t_list* listaTCBs, char* tareas,
									int* tamanioTotalAsociadaAlProceso);

#endif /* PAGINACION_H_ */






















