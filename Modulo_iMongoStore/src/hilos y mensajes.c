/*
 * hilos.c
 *
 *  Created on: 15 may. 2021
 *      Author: utnso
 */

#include "hilos y mensajes.h"


void nuevoHilo(int conexionCliente){


	pthread_t newHilo;

	arg_struct* args = malloc(sizeof(arg_struct));
	args->conexion = conexionCliente;

	pthread_create(&newHilo, NULL, (void*)mainHilo, (void*)args);
	pthread_detach(newHilo);

}


//TODO Preguntar si se puede poner un sem para que las bitacos se agarren de a una (E/S de a una) y la metadata por otro lado tambien
//Agregar una lista con los semaforos de las bitacoras

void mainHilo(void* argumentos){


	arg_struct* args = (arg_struct* ) argumentos;

	//RECIBIR PAQUETE
	recibirPaqueteiMongo(args->conexion);

	close(args->conexion);
	free(args);
}





void recibirPaqueteiMongo(int conexion){

	ID_MENSAJE* tipoDeMensaje = malloc(sizeof(ID_MENSAJE));
	size_t bytesARecibir =  0;

	recv(conexion, &bytesARecibir, sizeof(int), MSG_WAITALL);

	void* buffer = malloc(bytesARecibir);
	recv(conexion, buffer, bytesARecibir, 0);


	int desplazamiento = sizeof(ID_MENSAJE);

	memcpy(tipoDeMensaje, buffer, sizeof(ID_MENSAJE));

	if (*tipoDeMensaje == NUEVOS_TRIPULANTES){
		nuevosTripualntes tripulantes;
		tripulantes.nuevosTripulantes = list_create();
		tripulantes.idMensaje = NUEVOS_TRIPULANTES;
		tripulantes.cantNuevosTripulantes = 0;

		memcpy(&(tripulantes.cantNuevosTripulantes), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		int* valor;
		int i;
		for(i=0; i<tripulantes.cantNuevosTripulantes; i++){
			valor = malloc(sizeof(int));
			memcpy(valor, buffer + desplazamiento, sizeof(int));
			desplazamiento += sizeof(int);
			list_add(tripulantes.nuevosTripulantes, valor);

			sem_wait(mutexTripulantes);
			list_add(listaDeTripulantes, valor);
			sem_post(mutexTripulantes);
		}

		crearBitacoras(tripulantes);

		list_destroy(tripulantes.nuevosTripulantes);
		free(valor);

	}else if(*tipoDeMensaje == INFO_BITACORA){
		infoParaBitacora info;
		info.idMensaje = INFO_BITACORA;

		memcpy(&(info.idTripulante), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(&(info.largoDeInfo), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		info.largoDeInfo--;
		char* informacion = malloc(info.largoDeInfo);
		memcpy(informacion, buffer + desplazamiento, (sizeof(char)*info.largoDeInfo));
		info.info = informacion;

		agregarABitacora(info);

		free(informacion);
	}else if (*tipoDeMensaje ==  ENVIAR_BITACORA){
		bitacoraAEnviar bitacora;
		bitacora.idMensaje = ENVIAR_BITACORA;
		memcpy(&(bitacora.idTripulante), buffer + desplazamiento, sizeof(uint32_t));

		enviarBitacora(bitacora, conexion);

	}else if(*tipoDeMensaje == HACER_TAREA){
		tareaAHacer tarea;
		tarea.idMensaje = HACER_TAREA;
		memcpy(&(tarea.tarea), buffer + desplazamiento, sizeof(idTareas));
		desplazamiento += sizeof(idTareas);
		memcpy(&(tarea.parametros), buffer + desplazamiento, sizeof(uint32_t));

		hacerTarea(tarea);

	}
	free(buffer);
	free(tipoDeMensaje);
}






void crearBitacoras(nuevosTripualntes tripulantes){

	int* nuevoTripulante;

	int i;
	for(i=0; i<tripulantes.cantNuevosTripulantes; i++){

		nuevoTripulante = list_get(tripulantes.nuevosTripulantes, i);
		char* nombreDelIMS = string_new();

		char* tripu = string_itoa(*nuevoTripulante);
		string_append(&nombreDelIMS, "Tripulante");
		string_append(&nombreDelIMS, tripu);
		string_append(&nombreDelIMS, ".ims");

		sem_wait(nuevaBitaco);
		crearImsBitacora(nombreDelIMS);
		sem_post(nuevaBitaco);

		sem_wait(agregarADiccionario);
		sem_t* mutexBitaco = malloc(sizeof(sem_t));//TODO QUE PASA SI LA BITACORA YA EXISTE???
		sem_init(mutexBitaco, 0, 1);
		dictionary_put(diccionarioDeSemBitaco, tripu, (void*) mutexBitaco);
		sem_post(agregarADiccionario);
		
		free(tripu);
		free(nombreDelIMS);

	}

}



//INFO_BITACORA
void agregarABitacora(infoParaBitacora info){
	
	char* tripu = string_itoa(info.idTripulante);
	sem_wait(agregarADiccionario);
	sem_t* semaforoMutex = (sem_t*) dictionary_get(diccionarioDeSemBitaco, tripu);
	if(semaforoMutex == NULL){
		semaforoMutex = malloc(sizeof(sem_t));
		sem_init(semaforoMutex, 0, 1);
		dictionary_put(diccionarioDeSemBitaco, tripu, (void*) semaforoMutex);
	}
	sem_post(agregarADiccionario);
	
	sem_wait(semaforoMutex);
	
	t_config* bitacoraFile = openBitacora(info.idTripulante);
	int sizeBitacora = config_get_int_value(bitacoraFile, "Size");
	char** blocks = config_get_array_value(bitacoraFile, "Blocks");
	t_list* listaDeBLocks = arrayATlist(blocks);

	int loQueFalta = info.largoDeInfo;
	int cantDeBloques = listaDeBLocks->elements_count;
	int ultBloque;
	if(cantDeBloques == 0){
		ultBloque = -1;
	} else{
		char* ultimoBloque = list_get(listaDeBLocks, (cantDeBloques-1));
		ultBloque = atoi(ultimoBloque);
	}
	if(ultBloque == -1 || (valoresIniciales.Block_Size*cantDeBloques) == sizeBitacora){
		ultBloque = obtenerProxBloqueVacio();
		char* blok = string_itoa(ultBloque);
		list_add(listaDeBLocks, blok);
		cantDeBloques++;
	}

	int espacioDisponible = valoresIniciales.Block_Size*cantDeBloques - sizeBitacora;
	void* posicionAEscribir = blocksPointer + (ultBloque * valoresIniciales.Block_Size)+ (valoresIniciales.Block_Size - espacioDisponible);

	if(espacioDisponible >= info.largoDeInfo){

		sem_wait(archivoBlocks);
		memcpy(posicionAEscribir, info.info, sizeof(char)*info.largoDeInfo);
		sem_post(archivoBlocks);

	}else{

		sem_wait(archivoBlocks);
		memcpy(posicionAEscribir, info.info, sizeof(char)*espacioDisponible);
		sem_post(archivoBlocks);

		loQueFalta -= espacioDisponible;


		while(loQueFalta >= valoresIniciales.Block_Size){

			int proxBloque = obtenerProxBloqueVacio();
			posicionAEscribir = blocksPointer + (proxBloque * valoresIniciales.Block_Size);

			sem_wait(archivoBlocks);
			memcpy(posicionAEscribir, info.info + espacioDisponible, sizeof(char)*valoresIniciales.Block_Size);
			sem_post(archivoBlocks);

			espacioDisponible += valoresIniciales.Block_Size;
			
			char* blok = string_itoa(proxBloque);
			list_add(listaDeBLocks, blok);
			cantDeBloques++;

			loQueFalta -= valoresIniciales.Block_Size;
		}
		if(loQueFalta != 0){

			int proxBloque = obtenerProxBloqueVacio();
			posicionAEscribir = blocksPointer + (proxBloque * valoresIniciales.Block_Size);

			sem_wait(archivoBlocks);
			memcpy(posicionAEscribir, info.info + espacioDisponible, sizeof(char)*loQueFalta);
			sem_post(archivoBlocks);

			char* blok = string_itoa(proxBloque);
			list_add(listaDeBLocks, blok);
			cantDeBloques++;
		}
	}


	sizeBitacora += info.largoDeInfo;
	char* stringSize = string_itoa(sizeBitacora);
	config_set_value(bitacoraFile, "Size", stringSize);
	char* stringBloks = tlistAString(listaDeBLocks, cantDeBloques);
	config_set_value(bitacoraFile, "Blocks", stringBloks);


	config_save(bitacoraFile);
	config_destroy(bitacoraFile);


	free(stringSize);
	free(stringBloks);
	free(tripu);

	list_destroy_and_destroy_elements(listaDeBLocks, (void*) free);
	cantDeBloques = largoArray(blocks);

	free(blocks);

	sem_post(semaforoMutex);

}






void enviarBitacora(bitacoraAEnviar bitacora, int conexion){


	char* tripu = string_itoa(bitacora.idTripulante);
	sem_wait(agregarADiccionario);
	sem_t* semaforoMutex = (sem_t*) dictionary_get(diccionarioDeSemBitaco, tripu);
	if(semaforoMutex == NULL){
		semaforoMutex = malloc(sizeof(sem_t));
		sem_init(semaforoMutex, 0, 1);
		dictionary_put(diccionarioDeSemBitaco, tripu, (void*) semaforoMutex);
	}
	sem_post(agregarADiccionario);

	sem_wait(semaforoMutex);


	t_config* bitacoraFile = openBitacora(bitacora.idTripulante);
	int sizeBitacora = config_get_int_value(bitacoraFile, "Size");
	char** blocks = config_get_array_value(bitacoraFile, "Blocks");
	t_list* listaDeBLocks = arrayATlist(blocks);

	int cantDeBloques = listaDeBLocks->elements_count;
	int l = 0;
	void* bitacoraAEnviar = malloc(sizeBitacora);
	int desplazamiento = 0;

	void* src;

	int i;
	for(i=1; i<cantDeBloques; i++){

		char* blok = list_get(listaDeBLocks, l);
		src = blocksPointer + atoi(blok) * valoresIniciales.Block_Size;

		sem_wait(archivoBlocks);
		memcpy(bitacoraAEnviar + desplazamiento, src, sizeof(char) * valoresIniciales.Block_Size);
		sem_post(archivoBlocks);

		desplazamiento += sizeof(char) * valoresIniciales.Block_Size;

		l++;
	}

	int loQueFalta;
	if(i == 1){
		loQueFalta = sizeBitacora;
	}else {
		loQueFalta = valoresIniciales.Block_Size - (valoresIniciales.Block_Size*cantDeBloques - sizeBitacora);
	}

	char* blok = list_get(listaDeBLocks, l);
	src = blocksPointer + atoi(blok) * valoresIniciales.Block_Size;


	sem_wait(archivoBlocks);
	memcpy(bitacoraAEnviar + desplazamiento , src, sizeof(char) * loQueFalta);
	sem_post(archivoBlocks);


	char* bitacoAEnviarFinal = string_substring_until(bitacoraAEnviar, sizeBitacora);
//	printf("Bitacora: %s \n", bitacoAEnviarFinal);

	send(conexion, &sizeBitacora, sizeof(int), 0);
	send(conexion, bitacoAEnviarFinal, sizeBitacora, 0);

	frezee(blocks);

	config_destroy(bitacoraFile);

	list_destroy(listaDeBLocks);
	free(bitacoraAEnviar);
	free(bitacoAEnviarFinal);
	free(tripu);

	sem_post(semaforoMutex);

}





//HACER_TAREA
void hacerTarea(tareaAHacer tarea){

	if(tarea.tarea == GENERAR_OXIGENO){

		sem_wait(semOxigeno);

		t_config* fileOxigeno = openOxigeno(1);

		agregarEnArchivoBlocks(tarea, fileOxigeno);

		sem_post(semOxigeno);

	}else if(tarea.tarea == CONSUMIR_OXIGENO){

		sem_wait(semOxigeno);

		t_config* fileOxigeno = openOxigeno(0);

		if(fileOxigeno != NULL){
			sacarDeArchivoBlocks(tarea, fileOxigeno);
		}
		sem_post(semOxigeno);

	}else if(tarea.tarea == GENERAR_COMIDA){

		sem_wait(semComida);

		t_config* fileComida = openComida(1);

		agregarEnArchivoBlocks(tarea, fileComida);

		sem_post(semComida);

	}else if(tarea.tarea == CONSUMIR_COMIDA){

		sem_wait(semComida);

		t_config* fileComida = openComida(0);

		if(fileComida != NULL){
			sacarDeArchivoBlocks(tarea, fileComida);
		}

		sem_post(semComida);

	}else if(tarea.tarea == GENERAR_BASURA){

		sem_wait(semBasura);

		t_config* fileBasura = openBasura(1);

		agregarEnArchivoBlocks(tarea, fileBasura);

		sem_post(semBasura);

	}else if(tarea.tarea == DESCARTAR_BASURA){

		sem_wait(semBasura);

		char* pathBasura = string_duplicate(infConf.montaje);
		string_append(&pathBasura,"/Files/Basura.ims");

		FILE* basura = fopen(pathBasura, "r");

		if(basura != NULL){
			fclose(basura);
			remove(pathBasura);
		}else log_info(logiMongoStore, "NO EXISTE EL ARCHIVO BASURA.IMS");

		sem_post(semBasura);

		free(pathBasura);
	}else {
		printf("ALGO RARO PASO!");
	}

}





void agregarEnArchivoBlocks(tareaAHacer tarea, t_config* configFile){
	char** bloks = config_get_array_value(configFile, "Blocks");
	file metaFile = valoresGenerales(configFile, bloks);

	char* aLlenar = llenar(metaFile.CaracterLlenador, tarea.parametros);
	int loQueFalta = tarea.parametros;

	int ultimoBlock;
	if(metaFile.BlockCount == 0){
		ultimoBlock = -1;
	}else{
	 char* ultBloq = list_get(metaFile.Blocks, (metaFile.BlockCount-1));
	 ultimoBlock = atoi(ultBloq);
	}


	if(ultimoBlock == -1 || (valoresIniciales.Block_Size*metaFile.BlockCount) == metaFile.Size){
		ultimoBlock = obtenerProxBloqueVacio();
		char* blocke = string_itoa(ultimoBlock);
		list_add(metaFile.Blocks, blocke);
		metaFile.BlockCount++;

	}

	int espacioDisponible = metaFile.BlockCount*valoresIniciales.Block_Size - metaFile.Size;
	int cantOcupado = valoresIniciales.Block_Size - espacioDisponible;
	void* posicionAEscribir = blocksPointer + (ultimoBlock * valoresIniciales.Block_Size)+ cantOcupado;

	if(espacioDisponible >= tarea.parametros){

		sem_wait(archivoBlocks);
		memcpy(posicionAEscribir, aLlenar, sizeof(char)*tarea.parametros);
		sem_post(archivoBlocks);

	}else{

		sem_wait(archivoBlocks);
		memcpy(posicionAEscribir, aLlenar, sizeof(char)*espacioDisponible);
		sem_post(archivoBlocks);

		loQueFalta -= espacioDisponible;



		while(loQueFalta >= valoresIniciales.Block_Size){

			int proxBloque = obtenerProxBloqueVacio();
			posicionAEscribir = blocksPointer + (proxBloque * valoresIniciales.Block_Size);

			sem_wait(archivoBlocks);
			memcpy(posicionAEscribir, aLlenar + espacioDisponible, sizeof(char)*valoresIniciales.Block_Size);
			sem_post(archivoBlocks);

			espacioDisponible += valoresIniciales.Block_Size;

			char *blocke = string_itoa(proxBloque);
			list_add(metaFile.Blocks, blocke);
			metaFile.BlockCount++;


			loQueFalta -= valoresIniciales.Block_Size;
		}

		if(loQueFalta != 0){
			int proxBloque = obtenerProxBloqueVacio();
			posicionAEscribir = blocksPointer + (proxBloque * valoresIniciales.Block_Size);

			sem_wait(archivoBlocks);
			memcpy(posicionAEscribir, aLlenar + espacioDisponible, sizeof(char)*loQueFalta);
			sem_post(archivoBlocks);

			char* bloke = string_itoa(proxBloque);
			list_add(metaFile.Blocks, bloke);
			metaFile.BlockCount++;

		}


	}


	metaFile.Size += tarea.parametros;
	char* stringSize = string_itoa(metaFile.Size);
	config_set_value(configFile, "Size", stringSize);

	char* stringCount = string_itoa(metaFile.BlockCount);
	config_set_value(configFile, "BlockCount", stringCount);

	char* stringBloks = tlistAString(metaFile.Blocks, metaFile.BlockCount);
	config_set_value(configFile, "Blocks", stringBloks);

	char* md5 = calcularMD5(metaFile.CaracterLlenador, metaFile.Size);
	config_set_value(configFile, "MD5_Archivo", md5);

	config_save(configFile);
	config_destroy(configFile);

	free(md5);
	free(stringSize);
	free(aLlenar);
	free(stringCount);
	free(stringBloks);

	list_destroy_and_destroy_elements(metaFile.Blocks, (void*) free);

	free(bloks);
}

void sacarDeArchivoBlocks(tareaAHacer tarea, t_config* configFile){

	char** b = config_get_array_value(configFile, "Blocks");
	file metaFile = valoresGenerales(configFile, b);


	if(tarea.parametros >= metaFile.Size){

		log_info(logiMongoStore, "SE QUISIERON ELIMINAR MAS DE LO EXISTENTE");

		metaFile.BlockCount = 0;
		char* listBloques = "[]";
		metaFile.Size = 0;


		char *stringSize = string_itoa(metaFile.Size);
		config_set_value(configFile, "Size", stringSize);

		char * stringCount = string_itoa(metaFile.BlockCount);
		config_set_value(configFile, "BlockCount", stringCount);

		config_set_value(configFile, "Blocks", listBloques);

		char* md5 = string_itoa(0);
		config_set_value(configFile, "MD5_Archivo", md5);

		config_save(configFile);
		config_destroy(configFile);

		free(stringSize);
		free(stringCount);
		free(md5);

	}else{

		metaFile.Size -= tarea.parametros;
		int a = tarea.parametros/valoresIniciales.Block_Size;
		int b = tarea.parametros%valoresIniciales.Block_Size;
		
		if(a == 0 && b > 0){
			metaFile.BlockCount --;
			char* bloqueAEliminar = list_remove(metaFile.Blocks, metaFile.BlockCount);
			int bloqElim = atoi(bloqueAEliminar);

			limpiarBitEnBlock(bloqElim);
		}
		//a++;
		
		for(int i=0; i<a; i++){
			metaFile.BlockCount --;
			char* bloqueAEliminar = list_remove(metaFile.Blocks, metaFile.BlockCount);
			int bloqElim = atoi(bloqueAEliminar);

			limpiarBitEnBlock(bloqElim);
		}


		char * stringSize = string_itoa(metaFile.Size);
		config_set_value(configFile, "Size", stringSize);

		char * stringCount = string_itoa(metaFile.BlockCount);
		config_set_value(configFile, "BlockCount", stringCount);

		char * stringBloks = tlistAString(metaFile.Blocks, metaFile.BlockCount);
		config_set_value(configFile, "Blocks", stringBloks);

		char *md5 = calcularMD5(metaFile.CaracterLlenador, metaFile.Size);
		config_set_value(configFile, "MD5_Archivo", md5);


		config_save(configFile);
		config_destroy(configFile);

		free(stringSize);
		free(stringCount);
		free(stringBloks);
		free(md5);
	}
	list_destroy(metaFile.Blocks);

	free(b);
}




file valoresGenerales(t_config* config, char** bloks){
	file infoFile;
	infoFile.Size = config_get_int_value(config,"Size");
	infoFile.BlockCount = config_get_int_value(config, "BlockCount");
	infoFile.Blocks = arrayATlist(bloks);
	infoFile.CaracterLlenador = config_get_string_value(config, "Caracter_Llenador");
	infoFile.MD5 = config_get_string_value(config, "MD5_Archivo");

	return infoFile;
}

//HACER_TAREA












char* llenar(char* caracter, uint32_t veces){
	char* aLlenar = string_new();

	for(int i=0; i<veces; i++){
		string_append(&aLlenar, caracter);
	}

	return aLlenar;
}

char* borrarUltimoBloque(char* array){

	int largo = string_length(array) - 1;

	array[largo] = ' ';
	largo--;
	array[largo] = ' ';
	largo--;
	array[largo] = ']';

	return array;

}


char* arrayAString(char** array, int largo){
	
	if(largo == 0){
		return "[]";
	}
	
	char* string = string_new();
	string_append(&string, "[");

	for(int i=0; i<largo; i++){
		char* s = array[i];
		string_append(&string, s);
		string_append(&string, ",");
	}

	int size = string_length(string)-1;
	string[size] = ']';

	return string;
}
	
char* tlistAString(t_list* array, int cantDeBloques){
	
	if(cantDeBloques == 0){
		return "[]";
	}
	
	char* string = string_new();
	string_append(&string, "[");
	int l = 0;
	int i;
	for(i=0; i<cantDeBloques; i++){
		char* s = list_get(array, l);
		string_append(&string, s);
		string_append(&string, ",");
		l++;
	}
	
	int size = string_length(string)-1;
	string[size] = ']';

	return string;
}



int obtenerProxBloqueVacio(){
	int bloque, bloqueADevolver;
	bool es1 = true;

	sem_wait(archivoSuperBloque);

	for(bloque = 0; es1 != false; bloque++){
		es1 = bitarray_test_bit(bitarray, bloque);
		bloqueADevolver = bloque;
	}

	bitarray_set_bit(bitarray, bloqueADevolver);

	sem_post(archivoSuperBloque);

	return bloqueADevolver;
}

void limpiarBitEnBlock(int bloque){
	sem_wait(archivoSuperBloque);

	bitarray_clean_bit(bitarray, bloque);

	sem_post(archivoSuperBloque);
}

int ultimoBloque(char** array){
	int bloque, largo = 0;

	if(array[largo] == NULL){
		return -1;
	}
	while(array[largo] != NULL){
		bloque = atoi(array[largo]);
		largo++;
	}

	return bloque;
}










t_list* arrayATlist(char** array){
	t_list* listaDeBloques = list_create();

	int largo;
	for(largo = 0; array[largo] != NULL; largo++){
		char* bloque = array[largo];
		list_add(listaDeBloques, bloque);
	}

	return listaDeBloques;
}














