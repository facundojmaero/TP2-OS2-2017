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
* con el uso de la librería OpenMP, aplicado a un problema en particular.
* Se calculan valores de autocorrelación a partir de mediciones de un radar, 
* primero de manera procedural (monothread), y explotando el paralelismo del problema.
* Se proveen además estadísticas y gráficos que comparan el rendimiento de ejecución del programa.
*
*
* @par INSTALACIÓN:
* Los siguientes son los comandos útiles para el uso de la aplicación:\n
*
* 	"make"					--> Compila el proyecto y genera ejecutable.
* 	"make clean"			--> Limpia para una nueva compilación.
* 	"make cppcheck" 		--> Ejecuta el programa CppCheck sobre el proyecto. (Debe estar instalado).
* 	"./main_single_thread"	--> Ejecuta el programa en su versión single threaded.
* 	"./main_multithread"	--> Ejecuta el programa en su versión multithreaded.
*
* @par EJECUCIÓN:
* Para ejecutar el programa monothread, utilizar el comando "./main_single_thread"
* Para ejecutar el programa ejecutado en paralelo, utilizar el comando "./main_multithread"
* En el informe del trabajo se incluyen gráficos y estadísticas obtenidas de la ejecución 
* del software en la notebook del alumno, y el clúster de la Facultad.
*
*
*/