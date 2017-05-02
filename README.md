# TP2-OS2-2017 

### Trabajo Práctico 2 - Programación Distribuída
![Icono](http://icons.iconarchive.com/icons/martz90/circle-addon1/256/radar-icon.png)
___
|Autor                               |
|------------------------------------|
|Maero Facundo - 38479441              |

## 1. Descripción del trabajo
---
El objetivo de este proyecto es mostrar el poder de la programación distribuida, con el uso de la librería OpenMP, aplicado a un problema en particular.
Se calculan valores de autocorrelación a partir de mediciones de un radar, primero de manera procedural (monothread), y explotando el paralelismo del problema.
Se proveen además estadísticas y gráficos que comparan el rendimiento de ejecución del programa.

## 2. Instalación
---
Los siguientes son los comandos útiles para el uso de la aplicación:

```$ make```              --> Compila el proyecto y genera ejecutables.
 
```$ make clean ```       --> Limpia para una nueva compilación.

```$ make cppcheck```     --> Ejecuta el programa CppCheck sobre el proyecto. (Debe estar instalado).

```$ ./main_single_thread```   --> Ejecuta el programa en su versión single threaded.

```$ ./main_multithread```   --> Ejecuta el programa en su versión multithreaded.

## 3. Ejecución
--- 
Para ejecutar el programa monothread, utilizar el comando 
```$ ./main_single_thread```
 Para ejecutar el programa ejecutado en paralelo, utilizar el comando
 ```$ ./main_multithread```

 En el informe del trabajo se incluyen gráficos y estadísticas obtenidas de la ejecución del software en la notebook del alumno, y el clúster de la Facultad.

## 4. CppCheck
--- 
Al compilar y linkear, se genera un archivo donde se guardan los posibles errores y advertencias que encuentre el programa CppCheck al realizar el análisis estático del código. Este archivo se encuentra en:
```
TP2-OS2-2017/err.txt
```
Si desea más información, remítase a la documentación proporcionada, que se encuentra en la ruta ```doc/html/index.html```
