/*
 * segmentacion.h
 *
 *  Created on: 6 jul. 2021
 *      Author: utnso
 */

#ifndef SEGMENTACION_H_
#define SEGMENTACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <semaphore.h>

#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>


#include "mensajes.h"

void* principioSegmentacion;


//fisica = logica + offset
typedef struct {
	uint32_t inicio;//dir logica // - Revisar que este correctamente reservada la memoria para este direccion
	uint32_t tamanio;//offset
	tipoDeEstructura tipo;
	uint32_t elementoID;
	uint32_t nroSegmento;
} segmentacion;

typedef struct {
	tripulanteControlBlock* tcb;
	char* tareas;
	segmentacion* segmentoTripu;
} tcbYtareas;

void crearSegmentosTareas(int tamanio, int direccion, char* tareas);
void crearSegmentosPCB(int direccion, patotaControlBlock* patota);
void crearSegmentosTCB(int direccion, tripulanteControlBlock* tripulante);


void* obtenerDireccionFisica (uint32_t direccionLogica);
int obtenerDireccionLogicaInicio(int tamanio);

void cargarSegmentoEnMemoria(char* inicio, void* elemento);
t_list* ordenarSegmentosPorDirLogica(t_list* segmentosEnMemoria);
void compactar();
void compactarPorqueRecibimosSenial();

void actualizarTCBSegmentos(tripulanteControlBlock tripulante, informarMovimiento asd);
void actualizarTareasTCBSegmentos(tripulanteControlBlock* tripulante, segmentacion* segmento);

void* leerSegmentoDeMemoria (segmentacion* segmentoOrigen);
void escribirSegmentoEnMemoria (segmentacion* segmentoDestino, tripulanteControlBlock* elementoAAgregar);
void removerSegmentoDeMemoria(int valor);

int calcularTamanioRestanteEnMemoria(segmentacion* ultimoSegmento, int tamanioSegmentoAAgregar);
bool hayEspacioEntreSegmentos (segmentacion* primerSegmento, uint32_t inicioSegundoSegmento, int tamanioSegmentoAAgregar);
bool tieneMenorDirLogica (segmentacion* primerSegmento, segmentacion* segundoSegmento);


void destruirSegmento (segmentacion* segmentoADestruir);
t_list* devolverSegmentosConTCB();
bool tieneTCB(segmentacion* segmento);
segmentacion* crearSegmentoAPartirDeOtro (segmentacion* segmentoOrigen);
int coincideIDyEsTCB(segmentacion* tcb);
int coincideIDyTipo(segmentacion* segmento);

tcbYtareas* obtenerTareasSegunTripulante(int tripulante);


#endif /* SEGMENTACION_H_ */



