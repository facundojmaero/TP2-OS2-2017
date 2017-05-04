/** @file func_single_thread.c
 *  @brief Funciones de la ejecución single thread.
 *
 *  Contiene las funciones que utiliza el programa monothread
 *  para leer el archivo, procesar los datos y guardarlos en otro archivo.
 *
 *  @author Facundo Maero
 */
#include "../include/single_threaded.h"

int
leer_numero_pulsos_archivo(char file_name[], int* num_pulso, int* size_bytes){
	FILE *ptr;
	uint16_t valid_samples = 0;
	*num_pulso = 0;

	ptr=fopen(file_name,"rb");
	if (!ptr) {
		printf("Unable to open file!\n");
		return 1;
	}

	if(fseek(ptr,0,SEEK_END) != 0){
		printf("Error seeking file\n");
		fclose(ptr);
		return 1;
	}

	*size_bytes = ftell(ptr);
	printf("Tamaño del archivo %s en bytes: %d\n",file_name, *size_bytes);

	if(fseek(ptr,0,SEEK_SET) != 0){
		printf("Error seeking file\n");
		fclose(ptr);
		return 1;
	}
	
	while(ftell(ptr) != *size_bytes){
		if(fread(&valid_samples, sizeof(uint16_t), 1, ptr) != 1){
			printf("Error fread\n");
		}
		if(fseek(ptr,valid_samples*4*sizeof(float),SEEK_CUR) != 0){
			printf("Error seeking file\n");
			fclose(ptr);
			return 1;
		}
		(*num_pulso)++;
	}
	
	fclose(ptr);
	printf("El archivo contiene informacion de %d pulsos.\n", *num_pulso);
	return 0;
}


/**
* @brief Lee el archivo binario "pulsos.iq" y guarda su contenido en una estructura.
*
* Lee el archivo binario con la salida del ADC del radar, interpreta su contenido y
* lo guarda en un arreglo de pulsos. Cada pulso es una estructura que aloja las lecturas
* de la componente horizontal y vertidal del pulso, y el número de lecturas realizadas.
*
* @param file_name[] El nombre del archivo a leer
* @param pulsos[] Arreglo de estructuras de tipo pulso, donde guardar la información leida.
* @param cant_pulsos Puntero a variable para devolver la cantidad de pulsos leida.
* @return 1 si hubo un error, 0 caso contrario.
*/
int
leer_archivo(char file_name[], struct Pulso pulsos[], int len_file){
	FILE *ptr;
	uint16_t valid_samples = 0;
	float lectura[4*MAX_DATOS_LECTURA];
	int num_pulso=0;

	printf("Leyendo archivo '%s'...\n", file_name);

	ptr=fopen(file_name,"rb");
	if (!ptr) {
		printf("Unable to open file!\n");
		return 1;
	}
	
	while(ftell(ptr) != len_file){

		if(fread(&valid_samples, sizeof(uint16_t), 1, ptr) != 1){
			printf("Error fread\n");
		}

		pulsos[num_pulso].valid_samples = valid_samples;
		int j=0;
		//lee 1 pulso (1 tabla)
		if(fread(&lectura, sizeof(float), 4*valid_samples, ptr) != 4*valid_samples){
			printf("Error fread\n");
		}
		for (int i = 0; i < 4*valid_samples; ++i)
		{
			if(i<2*valid_samples){
				pulsos[num_pulso].dato_v[j].lectura_i = lectura[i];
				pulsos[num_pulso].dato_v[j].lectura_q = lectura[++i];
			}
			else{
				pulsos[num_pulso].dato_h[j].lectura_i = lectura[i];
				pulsos[num_pulso].dato_h[j].lectura_q = lectura[++i];
			}
			j = (j+1)%valid_samples;
		}
		num_pulso++;

	}
	
	fclose(ptr);
	printf("Lectura terminada.\n");
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
* @param pulsos[] Arreglo de estructuras de tipo pulso, donde guardar la información leida.
* @param gates[] Arreglo de estructuras de tipo gate, donde guardar los promedios y modulos calculados.
* @param num_pulsos Numero de pulsos en la estructura pulsos.
*/
void
promedio_y_valor_absoluto(struct Pulso pulsos[], struct Gate gates[], int num_pulsos){
	float valor_abs_v = 0, valor_abs_h = 0;
	int medicion = 0, limite;

	printf("Calculando valor absoluto de las mediciones...\n");
	printf("Promediando valores absolutos para cada gate...\n");

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

			for (int k = 0; k < limite; k++, medicion++)
			//calcula el valor absoluto de las muestras para ese gate
			//la variable medicion no se limpia para usarla en proximas iteraciones
			//y recorrer todas las muestras del pulso
			{
				valor_abs_v += valor_absoluto(pulsos[i].dato_v[medicion].lectura_i, pulsos[i].dato_v[medicion].lectura_q);
				valor_abs_h += valor_absoluto(pulsos[i].dato_h[medicion].lectura_i, pulsos[i].dato_h[medicion].lectura_q);
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
* @param gates[] Arreglo de estructuras de tipo gate, de donde saca el vector de modulos, y donde 
* guarda la correlacion calculada.
* @param num_pulsos Numero de pulsos en cada gate.
*/
void
calcular_autocorrelacion(struct Gate gates[], int num_pulsos){
	printf("Calculando autocorrelacion de cada gate...\n");
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
		printf("Error abriendo archivo para escritura\n");
		return 1;
	}
	uint16_t nro_pulsos = num_pulsos;
	uint16_t nro_gate = 0;
	if(fwrite(&nro_pulsos, sizeof(uint16_t), 1, f) < 0){
		printf("Error fwrite\n");
		fclose(f);
		return 1;
	}

	for (int i = 0; i < NUM_GATES; ++i, nro_gate++)
	{
		if(fwrite(&nro_gate, sizeof(uint16_t), 1, f) < 0){
			printf("Error fwrite\n");
			fclose(f);
			return 1;
		}
		if(fwrite(gates[i].vector_autocorr_v, sizeof(float), num_pulsos, f) < 0){
			printf("Error fwrite\n");
			fclose(f);
			return 1;
		}
		if(fwrite(gates[i].vector_autocorr_h, sizeof(float), num_pulsos, f) < 0){
			printf("Error fwrite\n");
			fclose(f);
			return 1;
		}
	}

	fclose(f);
	return 0;
}