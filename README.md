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

```$ ./build/single_threaded```   --> Ejecuta el programa en su versión single threaded.

```$ ./build/multithreaded```   --> Ejecuta el programa en su versión multithreaded.

## 3. Ejecución
--- 
Para ejecutar el programa monothread, utilizar el comando 
```$ ./build/ingle_threaded```
 Para ejecutar el programa ejecutado en paralelo, utilizar el comando
 ```$ ./build/multithreaded```

Pueden ingresarse opciones al ejecutar los binarios. Estas opciones son:

 - ```-t``` Muestra por salida standard el tiempo de ejecución medido.
 - ```-s``` Guarda en un archivo de texto el tiempo anterior. En caso de ser el programa multihilo, guarda también el número de hilos utilizado.
 - ```<nro_hilos>``` Permite modificar el número de hilos a usar.

Ejemplos:

 - ```./build/single_threaded -t -s```
 - ```./build/multithreaded 8 -s```

En el informe del trabajo se incluyen gráficos y estadísticas obtenidas de la ejecución del software en la notebook del alumno, y el clúster de la Facultad.

Puede comprobarse que el procesamiento en paralelo es correcto, y que no se produjo corrupción en los datos, mediante el siguiente comando, luego de ejecutar ambos programas:

```$ cmp out_mt.txt out_st.txt```

Que compara ambos archivos de texto, output de los programas multihilo y monohilo respectivamente. En caso de ser iguales no imprime nada en consola. Si hay diferencias, avisa al usuario e indica la línea donde se encuentra.

## 4. CppCheck
--- 
Al compilar y linkear, se genera un archivo donde se guardan los posibles errores y advertencias que encuentre el programa CppCheck al realizar el análisis estático del código. Este archivo se encuentra en:
```
TP2-OS2-2017/err.txt
```
Para ver su contenido, puede utilizar:

```$ cat err.txt ```

Si desea más información, remítase a la documentación proporcionada, que se encuentra en la ruta ```doc/html/index.html```

## 4. Profilers
--- 
Para este proyecto se utilizó **Valgrind** para observar la ejecución single thread, y optimizarlo para una mejor performance multithread. En particular se utilizó la herramienta **Callgrind**, que muestra el historial de llamadas a funciones del programa, el número de veces que se llamó a cada función, quién lo hizo, y un costo relativo de cada una con respecto a la ejecución total.

Para usar el profiler ejecutar el comando:
```$ valgrind --tool=callgrind <binario> <argumentos> ```

Ejemplos:
```$ valgrind --tool=callgrind ./build/single_threaded```
```$ valgrind --tool=callgrind ./build/multithreaded 4```

Luego, para visualizar los resultados de la ejecución, puede usarse un programa como **KCachegrind**, con el comando:

```$ kcachegrind callgrind.out.PID ```

Ejemplo:
```$ kcachegrind callgrind.out.15500 ```