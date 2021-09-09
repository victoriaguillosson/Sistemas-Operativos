/*
 * sabotajes.c
 *
 *  Created on: 23 jun. 2021
 *      Author: utnso
 */


#include "sabotajes.h"

//bless or hexedit

void inicioSabotaje(){

	char* posicion = infConf.posicionesSabotaje[posSabotaje];
	char posX = posicion[0];
	char posY = posicion[2];

	sabotaje enviarSabotaje;
	enviarSabotaje.x = posX-'0';
	enviarSabotaje.y = posY-'0';

	//AVISAR AL DISCORDIADOR DEL INICIO DEL SABOTAJE
	int conexion = crearConexionDiscordiador("127.0.0.1", "5010", logiMongoStore);

	void* buffer = malloc(sizeof(sabotaje));
	size_t desplazamiento = 0;

	memcpy(buffer + desplazamiento, &(enviarSabotaje.x), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer + desplazamiento, &(enviarSabotaje.y), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	send(conexion, buffer, sizeof(sabotaje), 0);

	//ESPERAR LA ORDEN DEL DISCORDIADOR PARA INICIAR EL PROTOCOLO fsck
	void* bufferRecv = malloc(sizeof(int));
	recv(conexion, bufferRecv, sizeof(int), MSG_WAITALL);

	int puedeEmpezar = (int) &bufferRecv;
	if(puedeEmpezar == 1){

		//INICIAR PROTOCOLO fsck
		sabotajeEnSuperBloque();
		sabotajeEnFiles();

	}


	posSabotaje++;
	free(buffer);
	free(bufferRecv);

}


void sabotajeEnSuperBloque(){

	int cantBloquesEnSB = 0;

	memcpy(&cantBloquesEnSB, superBlockPointer + sizeof(uint32_t), sizeof(uint32_t));

	if(cantBloquesEnSB != valoresIniciales.Blocks){
		memcpy(superBlockPointer + sizeof(uint32_t), &(valoresIniciales.Blocks), sizeof(uint32_t));
	}

	t_list* listaDeBloques = listaDeBlocksEnBitacoras();
	t_list* listaDeFiles = listaDeBlocksEnFiles();

	list_add_all(listaDeBloques, listaDeFiles);

	for(int i=0; i<valoresIniciales.Blocks; i++){
		bool estaSeteado = bitarray_test_bit(bitarray,i);

		int pertenece = perteneceALaLista(listaDeBloques, i);//Si no pertenece devuelve 0

		if(estaSeteado){
			if(pertenece == 0){
				bitarray_clean_bit(bitarray, i);
			}
		}else{
			if(pertenece == 1){
				bitarray_set_bit(bitarray, i);
			}
		}

	}

	list_destroy(listaDeBloques);
	list_destroy_and_destroy_elements(listaDeFiles, (void*) free);


}

void sabotajeEnFiles(){
	t_config* oxigeno = openOxigeno(0);
	t_config* comida = openComida(0);
	t_config* basura = openBasura(0);

	if(oxigeno != NULL){
		chequeoEnBlockCount(oxigeno);

		oxigeno = openOxigeno(0);
		chequeoDeSizeEnFile(oxigeno);

		oxigeno = openOxigeno(0);
		int sizePosta = chequeoEnBlocks(oxigeno);

		if(sizePosta > 0){
			oxigeno = openOxigeno(0);

			tareaAHacer tar;
			tar.parametros = sizePosta;

			agregarEnArchivoBlocks(tar, oxigeno);
		}
	}
	if(comida != NULL){
		chequeoEnBlockCount(comida);

		comida = openComida(0);
		chequeoDeSizeEnFile(comida);
		
		comida = openComida(0);
		int sizePosta = chequeoEnBlocks(comida);

		if(sizePosta > 0){
			comida = openComida(0);

			tareaAHacer tar;
			tar.parametros = sizePosta;

			agregarEnArchivoBlocks(tar, comida);
		}
	}
	if(basura != NULL){
		chequeoEnBlockCount(basura);

		basura = openBasura(0);
		chequeoDeSizeEnFile(basura);

		basura = openBasura(0);
		int sizePosta = chequeoEnBlocks(basura);

		if(sizePosta > 0){
			basura = openBasura(0);

			tareaAHacer tar;
			tar.parametros = sizePosta;

			agregarEnArchivoBlocks(tar, basura);

		}

	}
}







t_list* listaDeBlocksEnFiles(){

	t_list* listaDeBlocks = list_create();

	t_config* oxigeno = openOxigeno(0);
	t_config* comida = openComida(0);
	t_config* basura = openBasura(0);

	if(oxigeno != NULL){
		char** blocksOxigeno = config_get_array_value(oxigeno,"Blocks");
		int cantBlocksOxigeno = config_get_int_value(oxigeno, "BlockCount");

		agregarALista(listaDeBlocks, cantBlocksOxigeno, blocksOxigeno);

		config_destroy(oxigeno);

		frezee(blocksOxigeno);
	}


	if(comida != NULL){
		char** blocksComida = config_get_array_value(comida,"Blocks");
		int cantBlocksComida = config_get_int_value(comida, "BlockCount");

		agregarALista(listaDeBlocks, cantBlocksComida, blocksComida);

		config_destroy(comida);

		frezee(blocksComida);
	}

	if(basura != NULL){
		char** blocksBasura = config_get_array_value(basura,"Blocks");
		int cantBlocksBasura = config_get_int_value(basura, "BlockCount");

		agregarALista(listaDeBlocks, cantBlocksBasura, blocksBasura);

		config_destroy(basura);

		frezee(blocksBasura);
	}

	return listaDeBlocks;
}

void agregarALista(t_list* listaDeBlocks, int cantDeBlocks, char** blocks){

	int* bloq;
	for(int i=0; i<cantDeBlocks; i++){
		int bloqueInt = atoi(blocks[i]);
		bloq = malloc(sizeof(int));
		memcpy(bloq, &bloqueInt, sizeof(int));

		list_add(listaDeBlocks, bloq);
	}
}

int perteneceALaLista(t_list* listaDeBlocks, int b){

	int cantDeBlocks = list_size(listaDeBlocks);
	int* blok;

	for(int i=0; i<cantDeBlocks; i++){
		blok = list_get(listaDeBlocks, i);
		int blocke = *blok;

		if(blocke == b){
			return 1;
		}
	}

	return 0;
}




t_list* listaDeBlocksEnBitacoras(){
	t_list* listaDeBloques = list_create();

	int cantDetripulantes = list_size(listaDeTripulantes);
	int* tripulante;

	for(int i=0; i<cantDetripulantes; i++){
		tripulante = list_get(listaDeTripulantes, i);
		int tripu = *tripulante;

		t_config* bitacora = openBitacora(tripu);
		char** blocks = config_get_array_value(bitacora, "Blocks");

		int* bloq;
		for(int i = 0; blocks[i] != NULL; i++){
			int bloqueInt = atoi(blocks[i]);
			bloq = malloc(sizeof(int));
			memcpy(bloq, &bloqueInt, sizeof(int));

			list_add(listaDeBloques, bloq);
		}
		config_destroy(bitacora);
		//freeze(blocks);
	}


	return listaDeBloques;
}







void chequeoDeSizeEnFile(t_config* file){

	char* info = obtenerInfoUltBloque(file);

	int bloques = config_get_int_value(file, "BlockCount");
	int sizeMetadata = config_get_int_value(file, "Size");

	int bytesUltimoBloque = bytesDelBloque(info);

	int sizeTotalReal = (bloques-1)*valoresIniciales.Block_Size + bytesUltimoBloque;

	char* stringSize;
	if(sizeTotalReal != sizeMetadata){
		stringSize = string_itoa(sizeTotalReal);
		config_set_value(file, "Size", stringSize);

		config_save(file);
		config_destroy(file);

		free(stringSize);
		free(info);
	}else{
		config_save(file);
		config_destroy(file);

		free(info);
	}


}






char* obtenerInfoUltBloque(t_config* metadata){
	char* info = malloc(valoresIniciales.Block_Size);//string_new();

	char** blocks = config_get_array_value(metadata,"Blocks");
	int ultBloque = ultimoBloque(blocks);

	void* punteroABloque = blocksPointer + ultBloque*valoresIniciales.Block_Size;
	memcpy(info, punteroABloque, valoresIniciales.Block_Size);

	frezee(blocks);

	return info;
}


int bytesDelBloque(char* info){
	int size = 0;
	int desplazamiento;
	char* valor;

	for(desplazamiento=0; valoresIniciales.Block_Size > desplazamiento; desplazamiento+= sizeof(char)){

		valor = malloc(sizeof(char));
		memcpy(valor, info+desplazamiento, sizeof(char));

		if(*valor == 'O' || *valor == 'C' || *valor == 'B'){
			size++;
		}

		free(valor);
	}

	return size;
}


void chequeoEnBlockCount(t_config* metadata){
	char** bloques = config_get_array_value(metadata, "Blocks");
	int cantidadDeBloques = config_get_int_value(metadata, "BlockCount");

	int cantidadDeBloquesReal = largoArray(bloques);

	if(cantidadDeBloques != cantidadDeBloquesReal){
		char* stringBloques = string_itoa(cantidadDeBloquesReal);
		config_set_value(metadata, "BlockCount", stringBloques);

		config_save(metadata);
		config_destroy(metadata);

		free(stringBloques);
		frezee(bloques);
	}else{
		config_save(metadata);
		config_destroy(metadata);

		frezee(bloques);
	}


}


int chequeoEnBlocks(t_config* metadata){
	int sizeADevolver = -1;
	
	char* caracter = config_get_string_value(metadata, "Caracter_Llenador");
	int size = config_get_int_value(metadata, "Size");
	char* md5Original = config_get_string_value(metadata, "MD5_Archivo");

	char* md5Posta = calcularMD5(caracter, size);
	
	char* stringSize;
	char* stringCount;
	char* listBloques;
	char* md5;

	if(!(string_equals_ignore_case(md5Original,md5Posta))){

		limpiarArchivo(caracter[0]);
		
		sizeADevolver = size;

		int blockCount = 0;
		listBloques = "[]";
		int size = 0;


		stringSize = string_itoa(size);
		config_set_value(metadata, "Size", stringSize);

		stringCount = string_itoa(blockCount);
		config_set_value(metadata, "BlockCount", stringCount);

		config_set_value(metadata, "Blocks", listBloques);

		md5 = string_itoa(0);
		config_set_value(metadata, "MD5_Archivo", md5);

		config_save(metadata);
		config_destroy(metadata);

		free(stringSize);
		free(stringCount);
		free(md5);
		free(md5Posta);

	}else{

		config_save(metadata);
		config_destroy(metadata);

		free(md5Posta);
	}
	

	


	return sizeADevolver;
}




void limpiarArchivo(char caracter){

	for(int i=0; i<valoresIniciales.Blocks; i++){
		char* info = malloc(sizeof(char));
		void* bloque = blocksPointer + i*valoresIniciales.Block_Size;
		memcpy(info, bloque, sizeof(char));

		if(*info == caracter){
			bitarray_clean_bit(bitarray, i);
		}
		free(info);
	}


}





int crearConexionDiscordiador(char* ip, char* puerto, t_log* log){
	  	struct addrinfo hints;
	    struct addrinfo *server_info;
	    //int setAddr;
	    int clientSocket;

	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE;

	    if(getaddrinfo(ip, puerto, &hints, &server_info)){
	    	log_error(log, "ERROR EN GETADDRINFO");
	    	exit(-1);
	    }

	    clientSocket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	    if(connect(clientSocket, server_info->ai_addr, server_info->ai_addrlen) == -1){
	        log_error(log, "FALLÓ CONEXIÓN ENTRE DISCORDIADOR E iMONGOSOTRE...");
	        exit(-1);
	    }

	    log_info(log, "CONEXIÓN EXITOSA...");

	    freeaddrinfo(server_info);
	    return clientSocket;
}



