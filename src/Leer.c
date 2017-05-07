/**
* @mainpage main
* PROYECTO RADAR
* ===================
*
* @author Facundo Maero (facundojmaero@gmail.com)
* @date Mayo, 2017
* @version V1.0
*
* @brief Esta es una aplicacion que realiza cálculos de correlación sobre mediciones de un radar. Lenguaje: C.
* 
* @par INTRODUCCIÓN:
* El objetivo de este proyecto es mostrar el poder de la programación distribuida, 
* con el uso de la librería OpenMP, aplicado a un problema en particular.\n
* Se calculan valores de autocorrelación a partir de mediciones de un radar,
* primero de manera procedural (monothread), y explotando el paralelismo del problema.\n
* Se proveen además estadísticas y gráficos que comparan el rendimiento de ejecución del programa.\n
*
*
* @par INSTALACIÓN:
* Los siguientes son los comandos útiles para el uso de la aplicación:\n
*
* 	"make"					--> Compila el proyecto y genera ejecutable.
* 	"make clean"			--> Limpia para una nueva compilación.
* 	"make cppcheck" 		--> Ejecuta el programa CppCheck sobre el proyecto. (Debe estar instalado).
* 	"./single_threaded"		--> Ejecuta el programa en su versión single threaded.
* 	"./multithreaded"		--> Ejecuta el programa en su versión multithreaded.
*
* @par EJECUCIÓN:
* Para ejecutar el programa monothread, utilizar el comando "./single_threaded".\n
* Para ejecutar el programa ejecutado en paralelo, utilizar el comando "./multithreaded".\n
* Opciones que aceptan los binarios: \n
* --> -t Muestra por la salida standard el tiempo de ejecución del código.\n
* --> -s Guarda en un archivo de texto el tiempo de ejecución del código.\n
* --> <numero_de_hilos> En el caso del programa distribuído, lo ejecuta con el número de hilos ingresado.\n
* En el informe del trabajo se incluyen gráficos y estadísticas obtenidas de la ejecución \n
* del software en la notebook del alumno, y el clúster de la Facultad.\n
*
*
*/