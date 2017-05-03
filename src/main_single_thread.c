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
	int cant_pulsos_archivo;

	if(leer_archivo("pulsos.iq", pulsos, &cant_pulsos_archivo) != 0){
		printf("Error leer_archivo\n");
		return 1;
	}

	promedio_y_valor_absoluto(pulsos, gates, cant_pulsos_archivo);
	calcular_autocorrelacion(gates, cant_pulsos_archivo);
	if(guardar_archivo(gates, "out.txt", cant_pulsos_archivo) != 0){
		printf("Error guardando archivo\n");
		return 1;
	}
	printf("Datos guardados en 'out.txt'\n");
	return 0;
}