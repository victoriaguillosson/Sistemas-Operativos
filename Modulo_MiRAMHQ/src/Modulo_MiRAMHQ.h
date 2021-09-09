/*
 * Modulo_MiRAMHQ.h
 *
 *  Created on: 25 abr. 2021
 *      Author: utnso
 */

#ifndef MODULO_MIRAMHQ_H_
#define MODULO_MIRAMHQ_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/temporal.h>

#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>


#include "hilos.h"

#define ASSERT_CREATE(nivel, id, err)                                                   \
    if(err) {                                                                           \
        nivel_destruir(nivel);                                                          \
        nivel_gui_terminar();                                                           \
        fprintf(stderr, "Error al crear '%c': %s\n", id, nivel_gui_string_error(err));  \
        return EXIT_FAILURE;                                                            \
    }

int iniciarServidor(char* ip, char* puerto);
int esperarCliente(int socketServidor);
void atenderTripulantes(int cliente/*, NIVEL* mapa*/);

infoConfigMiRAMHQ obtenerInfoConfigMiRAMHQ();
int responderExpulsarTripulante(int cliente, uint32_t idTripulante);

void realizarDump();
void actualizarColaPaginacion(); //PARA LRU: tenemos que recibir un id de proceso, y tener una cola global

void iniciarSemaforos();
void inicializarIds();

NIVEL* iniciarMapa();
#endif /* MODULO_MIRAMHQ_H_ */
