/** @file multithreaded.c
 *  @brief Archivo principal del programa multihilo.
 *
 *
 *  @author Facundo Maero
 */

#include "../include/multithreaded.h"

 /**
* @brief Función main multithread.
*
* Función principal del programa que procesa los valores explotando el paralelismo
* del problema. 
* Acepta parametros opcionales: 
* -t Para medir el tiempo total de ejecucion y mostrarlo por salida standard.
* -s Para guardar en un archivo la medición realizada, y el número de hilos utilizado.
* <nro_hilos> Para configurar el número de hilos a utilizar.
*/
int 
main(int argc, char *argv[])
{
	struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);
	int num_threads = 1, time_flag = 0, save_flag = 0;
	int cant_pulsos_archivo, tamano_archivo_bytes;

	process_arguments(argc, argv, &time_flag, &save_flag, &num_threads);
	omp_set_num_threads(num_threads);

	printf("Ejecutando el codigo con "BOLDGREEN"%d"RESET" hilos.\n", omp_get_max_threads());

	if(leer_numero_pulsos_archivo("pulsos.iq", &cant_pulsos_archivo, &tamano_archivo_bytes) != 0){
			printf(BOLDRED"Error leyendo numero de pulsos en archivo\n"RESET);
			exit(EXIT_FAILURE);
		}
		
	struct Pulso pulsos[cant_pulsos_archivo];
	struct Gate gates[NUM_GATES];

	initialize_gates(gates, cant_pulsos_archivo);

	if(leer_archivo("pulsos.iq", pulsos, tamano_archivo_bytes) != 0){
		printf(BOLDRED"Error leer_archivo\n"RESET);
		exit(EXIT_FAILURE);
	}

	promedio_y_valor_absoluto(pulsos, gates, cant_pulsos_archivo);
	calcular_autocorrelacion(gates, cant_pulsos_archivo);

	free_absolute_values_gates(gates);
	if(guardar_archivo(gates, "out_mt.txt", cant_pulsos_archivo) != 0){
		printf(BOLDRED"Error guardando archivo\n"RESET);
		exit(EXIT_FAILURE);
	}

	printf("Datos guardados en "BOLDGREEN"'out_mt.txt'\n"RESET);
	gettimeofday(&tv2, NULL);
	double execution_time = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec) ;
	
	if(time_flag){
		printf ("Tiempo total = "BOLDGREEN"%f"RESET" segundos\n",execution_time);
	}
	
	if(save_flag){
		if(save_time_to_file(execution_time, num_threads,"times_mt.txt") != 0){
			printf(BOLDRED"Error guardando tiempo de ejecucion en archivo\n"RESET);
			exit(EXIT_FAILURE);
		}
		printf("Tiempo guardado en "BOLDGREEN"'times_mt.txt'\n"RESET);
	}

	return 0;
}

