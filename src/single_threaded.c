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
* Función principal del programa que procesa los valores de manera procedural.
* Acepta parametro opcional '-t' para medir el tiempo total de ejecucion.
*/
int 
main(int argc, char *argv[])
{
	struct timeval  tv1, tv2;
	// clock_t begin = clock();
	int opt = 0;
	gettimeofday(&tv1, NULL);
	
	if(argc == 2){
		if(strcmp(argv[1], "-t") == 0){
			opt = 1;
		}
	}

	int cant_pulsos_archivo, tamano_archivo_bytes;

	if(leer_numero_pulsos_archivo("pulsos.iq", &cant_pulsos_archivo, &tamano_archivo_bytes) != 0){
		printf("Error leyendo numero de pulsos en archivo\n");
		exit(EXIT_FAILURE);
	}

	struct Pulso pulsos[cant_pulsos_archivo];
	struct Gate gates[NUM_GATES];

	initialize_gates(gates, cant_pulsos_archivo);

	if(leer_archivo("pulsos.iq", pulsos, tamano_archivo_bytes) != 0){
		printf("Error leer_archivo\n");
		return 1;
	}

	promedio_y_valor_absoluto(pulsos, gates, cant_pulsos_archivo);
	calcular_autocorrelacion(gates, cant_pulsos_archivo);

	free_absolute_values_gates(gates);

	if(guardar_archivo(gates, "out.txt", cant_pulsos_archivo) != 0){
		printf("Error guardando archivo\n");
		return 1;
	}
	printf("Datos guardados en 'out.txt'\n");

	if(opt == 1){
		// clock_t end = clock();
		// double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		// printf("Tiempo de ejecucion: %f\n", time_spent);
		gettimeofday(&tv2, NULL);
		printf ("Tiempo total = %f segundos\n",
		         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
		         (double) (tv2.tv_sec - tv1.tv_sec));
	}
	return 0;
}