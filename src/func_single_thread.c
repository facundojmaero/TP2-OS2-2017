/** @file func_single_thread.c
 *  @brief Funciones de la ejecución single thread.
 *
 *  Contiene las funciones que utiliza el programa monothread
 *  para leer el archivo, procesar los datos y guardarlos en otro archivo.
 *
 *  @author Facundo Maero
 */
#include "../include/single_threaded.h"

/**
* @brief Lee el archivo binario "pulsos.iq" y guarda su contenido en una estructura.
*
* Lee el archivo binario con la salida del ADC del radar, interpreta su contenido y
* lo guarda en un arreglo de pulsos. Cada pulso es una estructura que aloja las lecturas
* de la componente horizontal y vertidal del pulso, y el número de lecturas realizadas.
*
* @param file_name[] El nombre del archivo a leer
* @param pulsos[] Arreglo de estructuras de tipo pulso, donde guardar la información leida.
* @return 1 si hubo un error, 0 caso contrario.
*/
int
leer_archivo(char file_name[], struct Pulso pulsos[NUM_PULSOS_ARCHIVO]){
	FILE *ptr;
	uint16_t valid_samples;
	float muestra[2];
	int num_pulso=0;

	printf("Leyendo archivo '%s'...\n", file_name);

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

	int len_file = ftell(ptr);
	// printf("Tamaño en bytes: %d\n", len_file);

	if(fseek(ptr,0,SEEK_SET) != 0){
		printf("Error seeking file\n");
		fclose(ptr);
		return 1;
	}

	while(ftell(ptr) != len_file){

		if(fread(&valid_samples, sizeof(uint16_t), 1, ptr) != 1){
			printf("Error fread\n");
		}
		// printf("%d - Tamaño: %d\n",num_pulso, valid_samples);
		pulsos[num_pulso].valid_samples = valid_samples;
		int j = 0;
		for (int i = 0; i < 2*valid_samples; ++i)
		{
			if(fread(&muestra, sizeof(float), 2, ptr) != 2){
				printf("Error fread\n");
			}
			if(i<valid_samples){
				pulsos[num_pulso].dato_v[j].lectura_i = muestra[0];
				pulsos[num_pulso].dato_v[j].lectura_q = muestra[1];
			}
			else{
				pulsos[num_pulso].dato_h[j].lectura_i = muestra[0];
				pulsos[num_pulso].dato_h[j].lectura_q = muestra[1];	
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
	float result = sqrt(pow(u,2) + pow(v,2));
	return result;
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
*/
void
promedio_y_valor_absoluto(struct Pulso pulsos[], struct Gate gates[]){
	float mean_v_q = 0, mean_v_i = 0, mean_h_q = 0, mean_h_i = 0;
	int medicion = 0, limite;

	printf("Calculando valores promedio para cada gate...\n");
	printf("Calculando valor absoluto de las mediciones promedio...\n");

	for (int i = 0; i < NUM_PULSOS_ARCHIVO; ++i)
	{
		int resto = pulsos[i].valid_samples % NUM_GATES;

		for (int j = 0; j < NUM_GATES; j++)
		//itera sobre un pulso llenando todas las gates
		{
			if (j >= resto) limite = pulsos[i].valid_samples / NUM_GATES;
			else 			limite = (pulsos[i].valid_samples / NUM_GATES) + 1;

			for (int k = 0; k < limite; k++, medicion++)
			{
				mean_v_i += pulsos[i].dato_v[medicion].lectura_i;
				mean_v_q += pulsos[i].dato_v[medicion].lectura_q;
				mean_h_i += pulsos[i].dato_h[medicion].lectura_i;
				mean_h_q += pulsos[i].dato_h[medicion].lectura_q;
			}

			gates[j].absol_v[i] = valor_absoluto(mean_v_i/limite, mean_v_q/limite);
			gates[j].absol_h[i] = valor_absoluto(mean_h_i/limite, mean_h_q/limite);

			mean_v_i = 0;
			mean_v_q = 0;
			mean_h_i = 0;
			mean_h_q = 0;
		}
		medicion = 0;
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
*/
void
calcular_autocorrelacion(struct Gate gates[]){
	printf("Calculando autocorrelacion de cada gate...\n");
	for (int i = 0; i < NUM_GATES; ++i)
	{
		autocorrelacion(gates[i].absol_v, NUM_PULSOS_ARCHIVO, gates[i].vector_autocorr_v);
		autocorrelacion(gates[i].absol_h, NUM_PULSOS_ARCHIVO,gates[i].vector_autocorr_h);
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
* @return 1 si hubo un error, 0 caso contrario.
*/
int
guardar_archivo(struct Gate gates[], char filename[]){
	printf("Guardando resultados...\n");
	FILE* f = fopen(filename,"wb");
	if(!f){
		printf("Error abriendo archivo para escritura\n");
		return 1;
	}
	uint16_t nro_pulsos = NUM_PULSOS_ARCHIVO;
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
		if(fwrite(gates[i].vector_autocorr_v, sizeof(float), NUM_PULSOS_ARCHIVO, f) < 0){
			printf("Error fwrite\n");
			fclose(f);
			return 1;
		}
		if(fwrite(gates[i].vector_autocorr_h, sizeof(float), NUM_PULSOS_ARCHIVO, f) < 0){
			printf("Error fwrite\n");
			fclose(f);
			return 1;
		}
	}

	fclose(f);
	return 0;
}