/*
 * createFS.c
 *
 *  Created on: 31 may. 2021
 *      Author: utnso
 */

#include "createFS.h"


//INICIAR FILE SYSTEM



void iniciarFileSystem(){

	char* path = infConf.montaje;

	//INICIO/CREO EL SUPER BLOQUE
	iniciarSuperBloque(path);

	//INICIO/CREO EL ARCHIVO BLOCKS
	iniciarBlocks(path);

	triuplantesExistentes(path);
}

void iniciarSuperBloque(char* path){

	char* superBloquePath = string_duplicate(path);
	string_append(&superBloquePath,"/SuperBloque.ims");

	int flag = 0;

	int superBFile = open(superBloquePath, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);

	if(superBFile == -1){
		perror("Error al abrir el super bloque");
		exit(-1);
	}

	struct stat infoSuperB;
	stat(superBloquePath, &infoSuperB);
	sizeSuperBloque = infoSuperB.st_size;

	if(sizeSuperBloque == 0){
		nuevosValores();
		sizeBitmap = (valoresIniciales.Blocks/8)+1;
		sizeSuperBloque = sizeof(int)*2 + sizeBitmap;
		ftruncate(superBFile, sizeSuperBloque);
		flag = 1;

	}

	superBlockPointer = mmap(NULL, sizeSuperBloque, PROT_READ|PROT_WRITE, MAP_SHARED, superBFile, 0);

	close(superBFile);

	if(flag == 1){
		memcpy(superBlockPointer, &(valoresIniciales.Block_Size), sizeof(uint32_t));
		memcpy(superBlockPointer+sizeof(uint32_t), &(valoresIniciales.Blocks), sizeof(uint32_t));


		char* bitmap = malloc(sizeBitmap);
		memcpy(superBlockPointer+sizeof(uint32_t)*2, bitmap, sizeBitmap);
		bitarray = bitarray_create(superBlockPointer+sizeof(uint32_t)*2, sizeBitmap);

		for(int i=0; i<valoresIniciales.Blocks; i++){
				bitarray_clean_bit(bitarray,i);
		}

		free(bitmap);

	}else{
		obtenerValoresIniciales();
	}
	free(superBloquePath);

}


void iniciarBlocks(char* path){
	char* bloquePath = string_duplicate(path);
	string_append(&bloquePath,"/Blocks.ims");

	int blocksFile = open(bloquePath, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);

	if(blocksFile == -1){
			perror("Error al abrir blocks");
			exit(-1);
	}

	struct stat infoBloks;
	stat(bloquePath, &infoBloks);
	sizeBlock = infoBloks.st_size;

	if(sizeBlock == 0){
		sizeBlock = valoresIniciales.Block_Size*valoresIniciales.Blocks;
		ftruncate(blocksFile,sizeBlock);
	}

	blocksPointer = mmap(NULL, sizeBlock, PROT_READ|PROT_WRITE, MAP_SHARED, blocksFile, 0);

	close(blocksFile);
	free(bloquePath);
}



FILE* crearImsOxigeno(char* pathOxigeno){

	FILE* oxigeno = fopen(pathOxigeno, "a+");

	char* bufS = "Size=0\n";
	char* bufBC = "BlockCount=0\n";
	char* bufB = "Blocks=[]\n";
	char* bufC = "Caracter_Llenador=O\n";
	char* bufMD5 = "MD5_Archivo=0";

	fputs(bufS,oxigeno);
	fputs(bufBC,oxigeno);
	fputs(bufB,oxigeno);
	fputs(bufC,oxigeno);
	fputs(bufMD5,oxigeno);

	return oxigeno;
}


FILE* crearImsComida(char* pathComida){

	FILE* comida = fopen(pathComida, "a+");

	char* bufS = "Size=0\n";
	char* bufBC = "BlockCount=0\n";
	char* bufB = "Blocks=[]\n";
	char* bufC = "Caracter_Llenador=C\n";
	char* bufMD5 = "MD5_Archivo=0";

	fputs(bufS,comida);
	fputs(bufBC,comida);
	fputs(bufB,comida);
	fputs(bufC,comida);
	fputs(bufMD5,comida);

	return comida;
}


FILE* crearImsBasura(char* pathBasura){

	FILE* basura = fopen(pathBasura, "a+");

	char* bufS = "Size=0\n";
	char* bufBC = "BlockCount=0\n";
	char* bufB = "Blocks=[]\n";
	char* bufC = "Caracter_Llenador=B\n";
	char* bufMD5 = "MD5_Archivo=0";

	fputs(bufS,basura);
	fputs(bufBC,basura);
	fputs(bufB,basura);
	fputs(bufC,basura);
	fputs(bufMD5,basura);

	return basura;

}


t_config* openOxigeno(int seCreaONo){
	char* pathOxigeno = string_duplicate(infConf.montaje);
	string_append(&pathOxigeno,"/Files/Oxigeno.ims");

	FILE* oxigeno = fopen(pathOxigeno, "r+");

	if(oxigeno == NULL){
		if(seCreaONo == 1){
			oxigeno = crearImsOxigeno(pathOxigeno);
			fclose(oxigeno);
		}else{
			log_info(logiMongoStore, "NO EXISTE EL ARCHIVO OXIGENO.IMS");
		}
	}

	t_config* fileOxigeno = config_create(pathOxigeno);

	free(pathOxigeno);
	return fileOxigeno;
}

t_config* openComida(int seCreaONo){
	char* pathComida = string_duplicate(infConf.montaje);
	string_append(&pathComida,"/Files/Comida.ims");

	FILE* comida = fopen(pathComida, "r+");

	if(comida == NULL){
		if(seCreaONo == 1){
			comida = crearImsComida(pathComida);
			fclose(comida);
		}else{
			log_info(logiMongoStore, "NO EXISTE EL ARCHIVO COMIDA.IMS");
		}
	}

	t_config* fileComida = config_create(pathComida);

	free(pathComida);
	return fileComida;
}

t_config* openBasura(int seCreaONo){
	char* pathBasura = string_duplicate(infConf.montaje);
	string_append(&pathBasura,"/Files/Basura.ims");

	FILE* basura = fopen(pathBasura, "r+");

	if(basura == NULL){
		if(seCreaONo == 1){
			basura = crearImsBasura(pathBasura);
			fclose(basura);
		}else{
			log_info(logiMongoStore, "NO EXISTE EL ARCHIVO BASURA.IMS");
		}
	}

	t_config* fileBasura = config_create(pathBasura);

	free(pathBasura);
	return fileBasura;
}







void crearImsBitacora(char* nombreDelIms){

	char* pathBitacora = string_duplicate(infConf.montaje);
	string_append(&pathBitacora,"/Files/Bitacora/");
	string_append(&pathBitacora, nombreDelIms);

	char* comando = string_new();
	string_append(&comando, "touch ");
	string_append(&comando, pathBitacora);

	system(comando);

	t_config* bitacora = config_create(pathBitacora);

	config_set_value(bitacora, "Size", "0");
	config_set_value(bitacora, "Blocks", "[]");

	config_save(bitacora);
	config_destroy(bitacora);

	free(pathBitacora);
	free(comando);
}


t_config* openBitacora(uint32_t tripulante){
	char* nombreDelIMS = string_duplicate(infConf.montaje);
	string_append(&nombreDelIMS,"/Files/Bitacora/Tripulante");
	char* stringTripu = string_itoa(tripulante);
	string_append(&nombreDelIMS, stringTripu);
	string_append(&nombreDelIMS, ".ims");

	t_config* file = config_create(nombreDelIMS);

	free(stringTripu);
	free(nombreDelIMS);
	return file;
}





void triuplantesExistentes(char* path){

	char* bitacosPath = string_duplicate(path);
	string_append(&bitacosPath,"/Files/Bitacora");

	DIR* dir;
	struct dirent* ent;

	dir = opendir(bitacosPath);

	if(dir == NULL){
		printf("NO SE PUDO ABRIR EL DIR");
		exit(-3);
	}

	int* tripulanteID;

	while((ent = readdir (dir)) != NULL){

		if(strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0 ){

			char** nombre = string_split(ent->d_name, ".");
			char* bitacora = nombre[0];
			char* tripulante = string_substring_from(bitacora, 10);
			int tripu = atoi(tripulante);
			tripulanteID = malloc(sizeof(int));
			memcpy(tripulanteID, &tripu, sizeof(int));

			list_add(listaDeTripulantes, tripulanteID);

			free(tripulante);
			frezee(nombre);

		}

	}

	free(bitacosPath);
	free(dir);

}








//SINCRONIZA CADA X TIEMPO LOS ARCHIVOS MAPEADOS A MEMORIA
void sincronizar(){
	while(1){
		sleep(infConf.tiempoSync);

		sem_wait(archivoSuperBloque);
		sem_wait(archivoBlocks);

		msync(superBlockPointer, sizeSuperBloque, MS_SYNC);
		msync(blocksPointer, sizeBlock, MS_SYNC);

		sem_post(archivoSuperBloque);
		sem_post(archivoBlocks);
	}
}



void obtenerValoresIniciales(){

	memcpy(&(valoresIniciales.Block_Size), superBlockPointer, sizeof(uint32_t));
	memcpy(&(valoresIniciales.Blocks), superBlockPointer + sizeof(uint32_t), sizeof(uint32_t));

	sizeBitmap = (valoresIniciales.Blocks/8)+1;
	bitarray = bitarray_create(superBlockPointer+sizeof(uint32_t)*2, sizeBitmap);

}


void nuevosValores(){

	printf("NO EXISTE NINGUN FILE SYSTEM PORFAVOR,\n");
	char* leerBloques = readline("Ingrese cantidad de bloques: ");
	char* leerSize = readline("Ingrese tamanio de bloques: ");
	valoresIniciales.Blocks = atoi(leerBloques);
	valoresIniciales.Block_Size = atoi(leerSize);
	printf("Cantidad de bloques: %d \n", valoresIniciales.Blocks);
	printf("Tamanio de bloques: %d \n", valoresIniciales.Block_Size);

	char* pathFiles = string_duplicate(infConf.montaje);
	string_append(&pathFiles, "/Files");
	char* pathBitacora = string_duplicate(pathFiles);
	string_append(&pathBitacora, "/Bitacora");

	mkdir(pathFiles, 0777);
	mkdir(pathBitacora, 0777);

	free(leerBloques);
	free(leerSize);
	free(pathFiles);
	free(pathBitacora);

	rl_clear_history();
}



char* calcularMD5(char* caracter, int size){

	char* aHashear = string_repeat(*caracter, size);

	char* hasheado = malloc(32);

	sem_wait(semMD5);

	FILE* archivoACalcular = fopen("infoParaCalculoDelMD5.txt", "w+");
	fwrite(aHashear, sizeof(char), size, archivoACalcular);
	fclose(archivoACalcular);

	system("md5sum infoParaCalculoDelMD5.txt > resultadoMD5.txt");

	FILE* md5Calculado = fopen("resultadoMD5.txt", "r");
	fread(hasheado, 32, sizeof(char), md5Calculado);
	fclose(md5Calculado);

	system("rm resultadoMD5.txt");
	system("rm infoParaCalculoDelMD5.txt");

	sem_post(semMD5);

	char* hasheado32 = string_substring_until(hasheado, 32);
	free(aHashear);
	free(hasheado);

	return hasheado32;
}




int largoArray(char** array){
	int largo = 0;

	if(array[largo] == NULL){
		return largo;
	}
	while(array[largo] != NULL){
		largo ++;
	}

	return largo;
}




void frezee(char** array){

	for(int i = 0; array[i] != NULL; i++){
		free(array[i]);
	}
	free(array);
}


















