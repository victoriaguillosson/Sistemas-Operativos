/*
 * createFS.h
 *
 *  Created on: 31 may. 2021
 *      Author: utnso
 */

#ifndef CREATEFS_H_
#define CREATEFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>
#include <commons/collections/list.h>



t_log * logiMongoStore;

sem_t* archivoSuperBloque;
sem_t* archivoBlocks;
sem_t* semMD5;

t_list* listaDeTripulantes;


typedef struct{
	char* montaje;
	char* puerto;
	uint32_t tiempoSync;
	char** posicionesSabotaje;
}infoConfig;

typedef struct{
	uint32_t Block_Size;
	uint32_t Blocks;
	//t_bitarray* bitmap;
}superBloque;

typedef struct{
	uint32_t Size;
	uint32_t BlockCount;
	t_list* Blocks;
	char* CaracterLlenador;
	char* MD5;
}file;

typedef struct{
	uint32_t Size;
	int* Blocks;
}bitacora;

superBloque valoresIniciales;

infoConfig infConf;

void* superBlockPointer;
void* blocksPointer;

//CALCULAR EL SIZE(bites del tamano + cant de bloques + array bitmap)
size_t sizeSuperBloque;
//Calculo: Block_size * Blocks
int sizeBlock;

t_bitarray* bitarray;
size_t sizeBitmap;


void iniciarFileSystem();

void iniciarSuperBloque(char* path);

void iniciarBlocks(char* path);

FILE* crearImsOxigeno(char* pathOxigeno);
FILE* crearImsComida(char* pathComida);
FILE* crearImsBasura(char* pathBasura);

t_config* openOxigeno(int seCreaONo);
t_config* openComida(int seCreaONo);
t_config* openBasura(int seCreaONo);

void crearImsBitacora(char* nombreDelIms);
t_config* openBitacora(uint32_t tripulante);

void triuplantesExistentes(char* path);

void sincronizar();

void obtenerValoresIniciales();

void nuevosValores();

char* calcularMD5(char* caracter, int size);

int largoArray(char** array);

void frezee(char** array);

#endif /* CREATEFS_H_ */
