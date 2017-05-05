/** @file main_single_thread.c
 *  @brief Archivo principal del programa monothread.
 *
 *
 *  @author Facundo Maero
 */

#include "../include/multithreaded.h"

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
	gettimeofday(&tv1, NULL);
	
	if(argc > 1){
		int aux = atoi(argv[1]);
		if((aux > 0) && (aux < MAX_NUM_THREADS)){
			omp_set_num_threads(aux);
		}
	}
	else {
		omp_set_num_threads(4);
	}

	printf("Ejecutando el codigo con %d hilos.\n", omp_get_max_threads());

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
		exit(EXIT_FAILURE);
	}

	promedio_y_valor_absoluto(pulsos, gates, cant_pulsos_archivo);
	calcular_autocorrelacion(gates, cant_pulsos_archivo);

	free_absolute_values_gates(gates);
	if(guardar_archivo(gates, "out_mt.txt", cant_pulsos_archivo) != 0){
		printf("Error guardando archivo\n");
		exit(EXIT_FAILURE);
	}

		
	printf("Datos guardados en 'out_mt.txt'\n");
	gettimeofday(&tv2, NULL);
	printf ("Tiempo total = %f segundos\n",
	         ( (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec) ));
	return 0;
}