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

#define NUM_PULSOS_ARCHIVO 72
/*!< Numero de pulsos en el archivo a leer. */
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
	// struct Lectura promedio_v[NUM_PULSOS_ARCHIVO];
	// struct Lectura promedio_h[NUM_PULSOS_ARCHIVO];
	float absol_v[NUM_PULSOS_ARCHIVO];
	float absol_h[NUM_PULSOS_ARCHIVO];
	float vector_autocorr_v[NUM_PULSOS_ARCHIVO];
	float vector_autocorr_h[NUM_PULSOS_ARCHIVO];
};
/*!< Estructura de un gate. Contiene valores absolutos de las componentes vertical
y horizontal de las mediciones, de todos los pulsos, y los valores de autocorrelacion
de los mismos.*/

int leer_archivo(char file_name[], struct Pulso pulsos[NUM_PULSOS_ARCHIVO]);
float valor_absoluto(float u, float v);
void promedio_y_valor_absoluto(struct Pulso pulsos[], struct Gate gates[]);
void autocorrelacion(float vector[],int len, float resultado[]);
void calcular_autocorrelacion(struct Gate gates[]);
int guardar_archivo(struct Gate gates[], char filename[]);