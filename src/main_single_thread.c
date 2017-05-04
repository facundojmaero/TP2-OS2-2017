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
int 
main(int argc, char *argv[])
{
	struct timeval  tv1, tv2;
	clock_t begin = clock();
	int opt = 0;
	gettimeofday(&tv1, NULL);
	
	if(argc > 1){
		opt = atoi(argv[1]);
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
		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("Tiempo de ejecucion: %f\n", time_spent);
		gettimeofday(&tv2, NULL);
		printf ("Total time = %f seconds\n",
		         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
		         (double) (tv2.tv_sec - tv1.tv_sec));
	}
	return 0;
}

void
initialize_gates(struct Gate gates[], int cant_pulsos_archivo){
	for (int i = 0; i < NUM_GATES; ++i)
	{
		gates[i].absol_v = malloc(sizeof(float) * cant_pulsos_archivo);
		gates[i].absol_h = malloc(sizeof(float) * cant_pulsos_archivo);
		gates[i].vector_autocorr_v = malloc(sizeof(float) * cant_pulsos_archivo);
		gates[i].vector_autocorr_h = malloc(sizeof(float) * cant_pulsos_archivo);
	}
}

void
free_absolute_values_gates(struct Gate gates[]){
	for (int i = 0; i < NUM_GATES; ++i)
	{
		free(gates[i].absol_v);
		free(gates[i].absol_h);
	}
}