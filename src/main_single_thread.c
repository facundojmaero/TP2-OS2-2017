/** @file main_single_thread.c
 *  @brief Archivo principal del programa monothread.
 *
 *
 *  @author Facundo Maero
 */

#include "../include/single_threaded.h"

 /**
* @brief Función main monothread.
*
*Función principal del programa que procesa los valores de manera procedural.
*/
int main()
{
	struct Pulso pulsos[NUM_PULSOS_ARCHIVO];
	struct Gate gates[NUM_GATES];

	if(leer_archivo("pulsos.iq", pulsos) != 0){
		printf("Error leer_archivo\n");
		return 1;
	}

	promedio_y_valor_absoluto(pulsos, gates);
	calcular_autocorrelacion(gates);
	if(guardar_archivo(gates, "out.txt") != 0){
		printf("Error guardando archivo\n");
		return 1;
	}
	printf("Datos guardados en 'out.txt'\n");
	return 0;
}