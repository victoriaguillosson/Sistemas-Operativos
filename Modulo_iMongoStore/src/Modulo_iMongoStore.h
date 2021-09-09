/*
 * Modulo_iMongoStore.h
 *
 *  Created on: 25 abr. 2021
 *      Author: utnso
 */

#ifndef MODULO_IMONGOSTORE_H_
#define MODULO_IMONGOSTORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "sabotajes.h"



t_config * configiMongoStore;



int iniciarServidor(char* ip, char* puerto, t_log * logiMongoStore);
int esperarCliente(int socketServidor, t_log * log);


infoConfig obtenerInfoConfig();
void iniciarSemaforos();

#endif /* MODULO_IMONGOSTORE_H_ */
