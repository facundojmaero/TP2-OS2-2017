/** @file single_threaded.h
 *  @brief Libreria de la ejecución monothread
 *
 *  Libreria del programa monothread, constantes, estructuras y prototipos de
 *  funciones.
 *
 *  @author Facundo Maero
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"

#define MAX_DATOS_LECTURA 5900
/*!< Numero maximo de datos por pulso en el archivo a leer. */
#define NUM_GATES 500
/*!< Numero de gates que discrimina el radar. */

struct Lectura{
	float lectura_i;
	float lectura_q;
};
/*!< Estructura de una lectura compleja del radar, compuesta de un valor
en fase y otro en cuadratura. */

struct Pulso{
	int valid_samples;
	struct Lectura dato_v[MAX_DATOS_LECTURA];
	struct Lectura dato_h[MAX_DATOS_LECTURA];
};
/*!< Estructura de una lectura un pulso del radar, compuesta de un numero de muestras,
y un conjunto de muestras (lecturas) de la componente vertical y horizontal. */

struct Gate{
	float *absol_v;
	float *absol_h;
	float *vector_autocorr_v;
	float *vector_autocorr_h;
};
/*!< Estructura de un gate. Contiene valores absolutos de las componentes vertical
y horizontal de las mediciones, de todos los pulsos, y los valores de autocorrelacion
de los mismos.*/

int leer_numero_pulsos_archivo(char file_name[], int* num_pulso, int* size_bytes);
int leer_archivo(char file_name[], struct Pulso pulsos[], int len_file);
float valor_absoluto(float u, float v);
void promedio_y_valor_absoluto(struct Pulso pulsos[], struct Gate gates[], int num_pulsos);
void autocorrelacion(float vector[],int len, float resultado[]);
void calcular_autocorrelacion(struct Gate gates[], int num_pulsos);
int guardar_archivo(struct Gate gates[], char filename[], int num_pulsos);
void initialize_gates(struct Gate gates[], int cant_pulsos_archivo);
void free_absolute_values_gates(struct Gate gates[]);
int save_time_to_file(double execution_time, char filename[]);
void process_arguments(int argc, char *argv[], int* time_flag, int* save_flag);