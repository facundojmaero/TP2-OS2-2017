/** @file func_multithreaded.c
 *  @brief Funciones de la ejecución multithread.
 *
 *  Contiene las funciones que utiliza el programa multihilo
 *  para leer el archivo, procesar los datos y guardarlos en otro archivo.
 *
 *  @author Facundo Maero
 */
#include "../include/multithreaded.h"

/**
* @brief Lee del archivo el numero de pulsos que contiene, para acelerar el procesamiento.
*
* Lee el archivo binario, y cuenta el numero de pulsos que contiene, solo leyendo
* el dato valid_samples de cada tabla de pulso. Además, lee el tamaño total del
* archivo.
*
* @param file_name[] El nombre del archivo a leer
* @param num_pulso Puntero para retornar el número de pulsos leídos.
* @param size_bytes Puntero para retornar el tamaño del archivo en bytes.
* @return 1 si hubo un error, 0 caso contrario.
*/
int
leer_numero_pulsos_archivo(char file_name[], int* num_pulso, int* size_bytes){
	FILE *ptr;
	uint16_t valid_samples = 0;
	*num_pulso = 0;

	ptr=fopen(file_name,"rb");
	if (!ptr) {
		printf(BOLDRED"Unable to open file!\n"RESET);
		return 1;
	}

	if(fseek(ptr,0,SEEK_END) != 0){
		printf(BOLDRED"Error seeking file\n"RESET);
		fclose(ptr);
		return 1;
	}

	*size_bytes = ftell(ptr);
	printf("Tamaño del archivo "BOLDGREEN"'%s': %d"RESET" bytes\n",file_name, *size_bytes);

	if(fseek(ptr,0,SEEK_SET) != 0){
		printf(BOLDRED"Error seeking file\n"RESET);
		fclose(ptr);
		return 1;
	}
	
	while(ftell(ptr) != *size_bytes){
		if(fread(&valid_samples, sizeof(uint16_t), 1, ptr) != 1){
			printf(BOLDRED"Error fread\n"RESET);
		}
		if(fseek(ptr,valid_samples*4*sizeof(float),SEEK_CUR) != 0){
			printf(BOLDRED"Error seeking file\n"RESET);
			fclose(ptr);
			return 1;
		}
		(*num_pulso)++;
	}
	
	fclose(ptr);
	printf("Se encontró informacion de "BOLDGREEN"%d"RESET" pulsos.\n", *num_pulso);
	return 0;
}


/**
* @brief Lee el archivo binario "pulsos.iq" y guarda su contenido en una estructura.
*
* Lee el archivo binario con la salida del ADC del radar, interpreta su contenido y
* lo guarda en un arreglo de pulsos. Cada pulso es una estructura que aloja las lecturas
* de la componente horizontal y vertidal del pulso, y el número de lecturas realizadas.
*
* Se explota el paralelismo al guardar el contenido de cada tabla:
* Se lee el tamaño de la tabla, y luego se leen de una vez todos los floats de la
* misma. Luego, se divide en dos secciones paralelas, aprovechando que la información
* horizontal y vertical está separada.
* Se van llenando los campos dato_v y dato_h de las estructuras Pulso en simultáneo.
*
* @param file_name[] El nombre del archivo a leer
* @param pulsos[] Arreglo de estructuras de tipo pulso, donde guardar la información leida.
* @param len_file Longitud del archivo en bytes, valor ya conocido.
* @return 1 si hubo un error, 0 caso contrario.
*/
int
leer_archivo(char file_name[], struct Pulso pulsos[], int len_file){
	FILE *ptr;
	uint16_t valid_samples = 0;
	float lectura[4*MAX_DATOS_LECTURA];
	int num_pulso=0;

	printf("Leyendo informacion...\n");

	ptr=fopen(file_name,"rb");
	if (!ptr) {
		printf(BOLDRED"Unable to open file!\n"RESET);
		return 1;
	}
	
	while(ftell(ptr) != len_file){

		if(fread(&valid_samples, sizeof(uint16_t), 1, ptr) != 1){
			printf(BOLDRED"Error fread\n"RESET);
		}

		pulsos[num_pulso].valid_samples = valid_samples;
		int j=0;
		//lee 1 pulso (1 tabla)
		if(fread(&lectura, sizeof(float), 4*valid_samples, ptr) != 4*valid_samples){
			printf(BOLDRED"Error fread\n"RESET);
		}

		#pragma omp parallel sections
		{
			#pragma omp section
			{	
				for (int i = 0; i < 2*valid_samples; ++i)
				{
					pulsos[num_pulso].dato_v[j].lectura_i = lectura[i];
					pulsos[num_pulso].dato_v[j].lectura_q = lectura[++i];
					j = (j+1)%valid_samples;
				}
			}
			#pragma omp section
			{
				for (int i = 2*valid_samples; i < 4*valid_samples; ++i)
				{
					pulsos[num_pulso].dato_h[j].lectura_i = lectura[i];
					pulsos[num_pulso].dato_h[j].lectura_q = lectura[++i];
					j = (j+1)%valid_samples;
				}
			}
		}
		num_pulso++;

	}
	
	fclose(ptr);
	return 0;
}

/**
* @brief Calcula el valor absoluto de un número complejo.
*
* Calcula el módulo o valor absoluto de un número complejo, 
* que consiste en la raíz cuadrada de la suma de los cuadrados (Pitágoras).
*
* @param u Componente real.
* @param v Componente imaginaria.
* @return result Módulo del complejo (u,v).
*/
float
valor_absoluto(float u, float v){
	return sqrt(pow(u,2) + pow(v,2));
}

/**
* @brief Calcula promedios de mediciones en cada gate, y el valor absoluto de las mismas.
*
* Dado un pulso, y una cantidad de mediciones asociada, distribuye las mismas en los 500 gates
* que discrimina el radar. Calcula el valor promedio de las mediciones por gate, y luego 
* el valor absoluto de cada promedio.
*
* Se acelera implementando un parallel for en el bucle más externo, que itera sobre
* el numero de pulsos en el archivo. Por lo tanto, se van calculando los promedios
* y el valor absoluto de las mediciones de cada pulso en paralelo, en función del
* número de hilos utilizado para ejecutar el programa.
*
* @param pulsos[] Arreglo de estructuras de tipo pulso, donde guardar la información leida.
* @param gates[] Arreglo de estructuras de tipo gate, donde guardar los promedios y modulos calculados.
* @param num_pulsos Numero de pulsos en la estructura pulsos.
*/
void
promedio_y_valor_absoluto(struct Pulso pulsos[], struct Gate gates[], int num_pulsos){
	float valor_abs_v = 0, valor_abs_h = 0;
	int medicion = 0, limite;

	printf("Calculando valor absoluto y promedio de las mediciones...\n");

	#pragma omp parallel for default(none) shared(num_pulsos, gates, pulsos) private(limite, medicion, valor_abs_v, valor_abs_h)
	for (int i = 0; i < num_pulsos; ++i)
	//itera sobre los pulsos para repartir mediciones en cada gate
	{
		int resto = pulsos[i].valid_samples % NUM_GATES;
		//calculo el resto de la division (cuantas muestras me sobran por gate)

		for (int j = 0; j < NUM_GATES; j++)
		//en un pulso, reparte las mediciones por gate
		{
			if (j >= resto) limite = pulsos[i].valid_samples / NUM_GATES;
			else 			limite = (pulsos[i].valid_samples / NUM_GATES) + 1;
			//calcula cuantas mediciones le tocan al gate dado

			for (int k = 0; k < limite; k++)
			//calcula el valor absoluto de las muestras para ese gate
			//la variable medicion no se limpia para usarla en proximas iteraciones
			//y recorrer todas las muestras del pulso
			{
				valor_abs_v += valor_absoluto(pulsos[i].dato_v[medicion].lectura_i,
				pulsos[i].dato_v[medicion].lectura_q);
				valor_abs_h += valor_absoluto(pulsos[i].dato_h[medicion].lectura_i,
				pulsos[i].dato_h[medicion].lectura_q);
				medicion++;
			}

			gates[j].absol_v[i] = valor_abs_v/limite;
			gates[j].absol_h[i] = valor_abs_h/limite;
			//promedio los valores absolutos

			valor_abs_v = 0;
			valor_abs_h = 0;
			//limpio variables para la proxima iteracion
		}
		medicion = 0;
		//si termine de recorrer un pulso, reinicio el indice medicion
		//para comenzar a leer los primeros valores del pulso siguiente
	}	
}

/**
* @brief Calcula la autocorrelacion normalizada de un vector dado.
*
* Calcula la autocorrelacion del vector dada su longitud, y devuelve el
* resultado en otro vector de la misma longitud.
* Aprovechando la propiedad de simetría, se devuelve un vector de igual
* longitud que el vector de entrada, siendo cada valor en el mismo la correlacion
* sobre si mismo, variando el desplazamiento.
* Para normalizarlo, se dividen los resultados por el numero de elementos del vector.
*
* @param vector[] Vector a calcular la autocorrelacion.
* @param len Longitud del vector.
* @param resultado[] Resultado del calculo.
*/
void
autocorrelacion(float vector[], int len, float resultado[]){
	for (int i = 0; i < len; ++i)
	{
		float suma = 0;
		for (int j = 0; j < len-i; ++j)
		{
			suma += vector[j] * vector[j+i];
		}
		resultado[i] = suma/len;
		//valor normalizado
	}
}

/**
* @brief Dado un conjunto de gates, calcula la autocorrelacion normalizada de cada uno.
*
* Calcula la autocorrelacion de cada vector columna gate, y guarda en ese gate el vector resultado.
*
* Se paraleliza el bucle for, permitiendo calcular la autocorrelacion de cada gate por separado.
*
* @param gates[] Arreglo de estructuras de tipo gate, de donde saca el vector de modulos, y donde 
* guarda la correlacion calculada.
* @param num_pulsos Numero de pulsos en cada gate.
*/
void
calcular_autocorrelacion(struct Gate gates[], int num_pulsos){
	printf("Calculando autocorrelacion de cada gate...\n");
	
	#pragma omp parallel for default(none) shared(num_pulsos, gates)
	for (int i = 0; i < NUM_GATES; ++i)
	{	
		autocorrelacion(gates[i].absol_v, num_pulsos, gates[i].vector_autocorr_v);
		autocorrelacion(gates[i].absol_h, num_pulsos, gates[i].vector_autocorr_h);
	}
}

/**
* @brief Guarda en un archivo binario el resultado de los calculos.
*
* Guarda en un archivo binario los valores calculados, con la siguiente estructura:
* numero de pulsos -> uint16_t
* 	numero de gate actual (0) -> uint16_t
* 	correlacion[0] -> float
* 	correlacion[1] -> float
* 	...
* 	correlacion[numero de pulsos] -> float
*	
*	...
*	numero de gate actual (500) -> uint16_t
*	correlacion[0] ->float
*	...
*	correlacion[numero de pulsos] -> float
*
* Dicho de otra manera, los datos se estructuran en una matriz, donde el eje x son
* los gates, y el eje y contiene el vector de autocorrelacion de cada gate.
* Se guarda el vector correspondiente al gate 0, luego el vector del gate 1, hasta el último.
*
* @param gates[] Arreglo de estructuras de tipo gate, que contiene los resultados de la correlacion a guardar.
* @param filename[] Nombre del archivo donde se quieren guardar los datos.
* @param num_pulsos Numero de pulsos en cada gate.
* @return 1 si hubo un error, 0 caso contrario.
*/
int
guardar_archivo(struct Gate gates[], char filename[], int num_pulsos){
	printf("Guardando resultados...\n");
	FILE* f = fopen(filename,"wb");
	if(!f){
		printf(BOLDRED"Error abriendo archivo para escritura\n"RESET);
		return 1;
	}
	uint16_t nro_pulsos = num_pulsos;
	uint16_t nro_gate = 0;
	if(fwrite(&nro_pulsos, sizeof(uint16_t), 1, f) < 0){
		printf(BOLDRED"Error fwrite\n"RESET);
		fclose(f);
		return 1;
	}

	for (int i = 0; i < NUM_GATES; ++i, nro_gate++)
	{
		if(fwrite(&nro_gate, sizeof(uint16_t), 1, f) < 0){
			printf(BOLDRED"Error fwrite\n"RESET);
			fclose(f);
			return 1;
		}
		if(fwrite(gates[i].vector_autocorr_v, sizeof(float), num_pulsos, f) < 0){
			printf(BOLDRED"Error fwrite\n"RESET);
			fclose(f);
			return 1;
		}
		if(fwrite(gates[i].vector_autocorr_h, sizeof(float), num_pulsos, f) < 0){
			printf(BOLDRED"Error fwrite\n"RESET);
			fclose(f);
			return 1;
		}
	}

	fclose(f);
	return 0;
}

/**
* @brief Wrapper de la syscall malloc con control del valor de retorno.
*
* Funcion wrapper que verifica el return value de malloc, y si es NULL, 
* avisa al usuario y finaliza la ejecucion del programa.
*
* @param n Cantidad de memoria a reservar.
* @return p Puntero a la direccion de memoria reservada.
*/
void *safe_malloc(size_t n)
{
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", n);
        exit(EXIT_FAILURE);
    }
    return p;
}


/**
* @brief Inicializa los campos de la estructura de tipo Gate para alojar los pulsos leidos.
*
* Realiza el malloc de los elementos del arreglo gates.
* Si se quisiera leer un archivo donde no se sabe la cantidad de pulsos ahi contenidos,
* es necesario asignar memoria de manera dinamica.
* Esta reserva de memoria depende de la cantidad de pulsos, y se realiza mediante el
* wrapper safe_malloc, que verifica la correcta ejecucion de la syscall malloc().
*
* Se paraleliza el bucle for, ya que la operacion realizada en él no tiene relación
* de una iteración a otra, y el bucle tiene NUM_GATES iteraciones, valor considerado
* lo suficientemente grande para que justifique el uso de threads.
*
* @param gates[] Arreglo de estructuras de tipo gate, con punteros sin inicializar.
* @param cant_pulsos_archivo Cantidad de pulsos leida en el archivo, para reservar memoria.
*/
void
initialize_gates(struct Gate gates[], int cant_pulsos_archivo){
	#pragma omp parallel for default(none) shared(cant_pulsos_archivo, gates)
	for (int i = 0; i < NUM_GATES; ++i)
	{
		gates[i].absol_v = safe_malloc(sizeof(float) * cant_pulsos_archivo);
		gates[i].absol_h = safe_malloc(sizeof(float) * cant_pulsos_archivo);
		gates[i].vector_autocorr_v = safe_malloc(sizeof(float) * cant_pulsos_archivo);
		gates[i].vector_autocorr_h = safe_malloc(sizeof(float) * cant_pulsos_archivo);
	}
}

/**
* @brief Libera la memoria reservada previamente para los valores absolutos de las mediciones.
*
* Libera la porcion de memoria reservada por el arreglo de struct Gate, para guardar
* los valores absolutos de las mediciones, ya que no se necesitan mas.
*
* Se paraleliza el bucle for, ya que la operacion realizada en él no tiene relación
* de una iteración a otra, y el bucle tiene NUM_GATES iteraciones, valor considerado
* lo suficientemente grande para que justifique el uso de threads.
*
* @param gates[] Arreglo de estructuras de tipo gate, con los datos a liberar.
*/
void
free_absolute_values_gates(struct Gate gates[]){
	#pragma omp parallel for default(none) shared(gates)
	for (int i = 0; i < NUM_GATES; ++i)
	{
		free(gates[i].absol_v);
		free(gates[i].absol_h);
	}
}

/**
* @brief Guarda en un archivo de texto el numero de hilos y el tiempo de ejecucion.
*
* @param execution_time Tiempo de ejecucion [s]
* @param hilos Numero de hilos que ejecutaron el programa.
* @param filename[] Nombre del archivo a guardar los datos.
* @return 1 si hubo un error, 0 caso contrario.
*/
int
save_time_to_file(double execution_time, int hilos, char filename[]){
	FILE* f = fopen(filename,"a");
	if(!f){
		printf(BOLDRED"Error abriendo archivo para escritura\n"RESET);
		return 1;
	}
	if(fprintf(f, "%d %f\n",hilos, execution_time) < 0){
		printf(BOLDRED"Error fwrite\n"RESET);
		fclose(f);
		return 1;
	}
	fclose(f);
	return 0;
}

/**
* @brief Procesa los argumentos con los que se llamó el programa y setea los flags acorde a ello.
*
* Evalúa en un bucle los argumentos.
* Los valores aceptados son:
* * -t Muestra por salida standard el tiempo de ejecución.
* * -s Guarda en un archivo de texto el número de hilos utilizado y el tiempo.
* * <nro_hilos> Número de hilos a utilizar. Si es un valor incorrecto avisa error.
* Si el argumento no existe, se informa del error.
*
* @param argc Numero de argumentos con que se llamó el programa.
* @param argv Argumentos con los que se llamó el programa.
* @param time_flag Puntero al flag de mostrar tiempo, para modificarlo si es necesario.
* @param save_flag Puntero al flag de guardar el tiempo, para modificarlo si es necesario.
* @param num_threads Puntero al numero de hilos a usar, para modificarlo si es necesario.
*/
void
process_arguments(int argc, char *argv[], int* time_flag, int* save_flag, int* num_threads){
	if(argc > 1){
		for (int i = 1; i < argc; ++i)
		{
			if(strcmp(argv[i],"-t") == 0){
				*time_flag = 1;
			}
			else if(strcmp(argv[i],"-s") == 0){
				*save_flag = 1;
			}
			else if(atoi(argv[i]) != 0){
				int aux = atoi(argv[i]);
				if((aux > 0) && (aux < MAX_NUM_THREADS)){
					*num_threads = aux;
				}
				else{
					*num_threads = omp_get_max_threads();
					printf(BOLDRED"Error"RESET", no puede ejecutarse el programa con "BOLDRED"%s"RESET" hilos.\n", argv[i]);
				}
			}
			else{
				printf("No se reconoce el comando "BOLDRED"%s\n"RESET, argv[i]);
			}
		}
	}

	else {
		*num_threads = omp_get_max_threads();
	}
}