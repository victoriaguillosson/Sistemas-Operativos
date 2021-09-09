/*
 * sabotajes.h
 *
 *  Created on: 23 jun. 2021
 *      Author: utnso
 */

#ifndef SABOTAJES_H_
#define SABOTAJES_H_


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "hilos y mensajes.h"

typedef struct{
	uint32_t x;
	uint32_t y;
}sabotaje;

int posSabotaje;
char* ipLocal;


void inicioSabotaje();

void sabotajeEnSuperBloque();

void sabotajeEnFiles();

t_list* listaDeBlocksEnFiles();
void agregarALista(t_list* listaDeBlocks, int cantDeBlocks, char** blocks);
int perteneceALaLista(t_list* listaDeBlocks, int i);
t_list* listaDeBlocksEnBitacoras();

void chequeoDeSizeEnFile(t_config* file);

char* obtenerInfoUltBloque(t_config* metadata);
int bytesDelBloque(char* info);

void chequeoEnBlockCount(t_config* metadata);

int chequeoEnBlocks(t_config* metadata);

void limpiarArchivo(char caracter);

int crearConexionDiscordiador(char* ip, char* puerto, t_log* log);

#endif /* SABOTAJES_H_ */
