/** @file single_threaded.c
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
* Acepta parametros opcionales: 
* -t Para medir el tiempo total de ejecucion y mostrarlo por salida standard.
* -s Para guardar en un archivo la medición realizada.
*/
int 
main(int argc, char *argv[])
{
	clock_t start, end;
	start = clock();
	double cpu_time_used;
	
	int time_flag = 0, save_flag = 0;
	int cant_pulsos_archivo, tamano_archivo_bytes;

	process_arguments(argc, argv, &time_flag, &save_flag);

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

	if(guardar_archivo(gates, "out_st.txt", cant_pulsos_archivo) != 0){
		printf("Error guardando archivo\n");
		return 1;
	}
	printf("Datos guardados en "BOLDGREEN"'out_st.txt'\n"RESET);
	
	end = clock();
	cpu_time_used = ((double) (end-start)) / CLOCKS_PER_SEC;
	
	if(time_flag){
		printf ("Tiempo total = "BOLDGREEN"%f"RESET" segundos\n",cpu_time_used);
	}
	
	if(save_flag){
		if(save_time_to_file(cpu_time_used,"times_st.txt") != 0){
			printf("Error guardando tiempo de ejecucion en archivo\n");
			exit(EXIT_FAILURE);
		}
		printf("Tiempo guardado en "BOLDGREEN"'times_st.txt'\n"RESET);
	}
	return 0;
}