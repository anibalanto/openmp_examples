# OpenMP Examples (ompe)
El siguiente proyecto C++ intenta mostrar en una consola como trabajan los hilos de OpenMP visualmente a través del registro de mensajes que le pasa el usuario.

Puedes directamente ejecutar desde el navegador Web:

[![Run on Repl.it](https://repl.it/badge/github/anibalanto/openmp_examples)](https://repl.it/github/anibalanto/openmp_examples)

O sino... lo siguiente:
## Clonar proyecto
En una terminal vamos a la carpeta donde queremos que esté el proyecto.
```
git clone https://github.com/anibalanto/openmp_examples
```
## Compilación GCC
(Es ncesario tener instalado GCC)
Corremos `g++` para compilar
```
g++ -fopenmp main.cpp -o ompe
```
## Compilación CMake
(Es necesario tener instaldo CMake)
Creamos la carpeta build dentro del proyecto y nos colocamos dentro:
```
mkdir build
cd build
```
Desde allí corremos CMake para compilar:
```
cmake ..
cmake --build .
```

## Uso
Dentro de la carpeta `/build/apps` se encontrá el archivo del comando ejecutable con el nombre `ompe`.
Para ver la ayuda del comando utilizamos la opción `-h` o `--help`:
```
./ompe -h
```
Esto mostrará el uso general del comando con el detalle de cada una de las opciones:
```
uso:        ompe [<fnum>] [-l] [-h]
Estos son comandos de ompe (OpenMP Example)
       <fnum>  Número de función a ejecutar
       -l, --list
               Lista de funciones con su número de función

       -h, --help
               Ayuda
```
Con la opción `-l` o `--list` se listarán todas las funciones con ejemplos y su valor `fnum` con el cual se puede ejecutar.
```
./ompe -l
```
Tendrá como salida:
```
Número función con su respectivo nombre
fnum  Nombre              
0     pfor1               
1     pfor1bis            
2     phola_openmp        
3     pcritical_hola_openmp
4     phello_omp          
5     phello_nested       
6     p4for_single_master 
7     pnested_compare     
8     pbig_factorial_nested

```
Por ejemplo, para ejecutar la función número 3 simplemente escribimos:

```
./ompe 6
```
Tendrá como salida:
```
*>     0 ~begin
*───┬───┬───┬───┐   *>     0 ~fork
0   │   │   │   │   0>   0.0 ~msg: simple for 0
│   │   │   3   │   3>   0.3 ~msg: simple for 0
│   │   │   3   │   3>   0.3 ~msg: simple for 1
0   │   │   │   │   0>   0.0 ~msg: simple for 1
│   │   │   3   │   3>   0.3 ~msg: simple for 2
│   │   │   │   4   4>   0.4 ~msg: simple for 0
│   │   2   │   │   2>   0.2 ~msg: simple for 0
│   │   │   *─┐ │   *>   0.3 ~fork
│   1   │   │ │ │   1>   0.1 ~msg: simple for 0
0   │   │   │ │ │   0>   0.0 ~msg: simple for 2
│   1   │   │ │ │   1>   0.1 ~msg: simple for 1
│   │   │   0 │ │   0> 0.3.0 ~msg: NESTED SINGLE 0
│   │   │   │ │ 4   4>   0.4 ~msg: simple for 1
│   │   │   0 │ │   0> 0.3.0 ~msg: NESTED SINGLE 1
│   1   │   │ │ │   1>   0.1 ~msg: simple for 2
│   │   2   │ │ │   2>   0.2 ~msg: simple for 1
│   │   │   0 │ │   0> 0.3.0 ~msg: NESTED SINGLE 2
│   │   2   │ │ │   2>   0.2 ~msg: simple for 2
*─┐ │   │   │ │ │   *>   0.0 ~fork
│ │ *─┐ │   │ │ │   *>   0.1 ~fork
│ │ │ │ *─┐ │ │ │   *>   0.2 ~fork
│ │ │ │ 0 │ │ │ │   0> 0.2.0 ~msg: NESTED SINGLE 0
│ │ │ │ 0 │ │ │ │   0> 0.2.0 ~msg: NESTED SINGLE 1
│ │ │ │ 0 │ │ │ │   0> 0.2.0 ~msg: NESTED SINGLE 2
│ │ │ 1 │ │ │ │ │   1> 0.1.1 ~msg: NESTED SINGLE 0
│ │ │ 1 │ │ │ │ │   1> 0.1.1 ~msg: NESTED SINGLE 1
│ │ │ │ │ │ °─┘ │   °>   0.3 ~join
│ │ │ │ │ │ │   4   4>   0.4 ~msg: simple for 2
│ │ │ │ °─┘ │   │   °>   0.2 ~join
│ │ │ 1 │   │   │   1> 0.1.1 ~msg: NESTED SINGLE 2
│ 1 │ │ │   │   │   1> 0.0.1 ~msg: NESTED SINGLE 0
│ │ │ │ │   │   *─┐ *>   0.4 ~fork
│ │ │ │ │   3   │ │ 3>   0.3 ~msg: SINGLE 0
│ │ °─┘ │   │   │ │ °>   0.1 ~join
│ 1 │   │   │   │ │ 1> 0.0.1 ~msg: NESTED SINGLE 1
│ │ │   │   3   │ │ 3>   0.3 ~msg: SINGLE 1
│ 1 │   │   │   │ │ 1> 0.0.1 ~msg: NESTED SINGLE 2
│ │ │   │   │   │ 1 1> 0.4.1 ~msg: NESTED SINGLE 0
│ │ │   │   3   │ │ 3>   0.3 ~msg: SINGLE 2
│ │ │   │   │   │ 1 1> 0.4.1 ~msg: NESTED SINGLE 1
°─┘ │   │   │   │ │ °>   0.0 ~join
│   │   │   │   │ 1 1> 0.4.1 ~msg: NESTED SINGLE 2
0   │   │   │   │ │ 0>   0.0 ~msg: MASTER 0
0   │   │   │   │ │ 0>   0.0 ~msg: MASTER 1
│   │   │   │   °─┘ °>   0.4 ~join
0   │   │   │   │   0>   0.0 ~msg: MASTER 2
0   │   │   │   │   0>   0.0 ~msg: --barrier-----
0   │   │   │   │   0>   0.0 ~msg: parallel 4.0
│   │   │   3   │   3>   0.3 ~msg: parallel 4.0
│   │   2   │   │   2>   0.2 ~msg: parallel 4.0
│   │   │   3   │   3>   0.3 ~msg: parallel 4.1
│   │   2   │   │   2>   0.2 ~msg: parallel 4.1
│   │   │   3   │   3>   0.3 ~msg: parallel 4.2
│   │   2   │   │   2>   0.2 ~msg: parallel 4.2
0   │   │   │   │   0>   0.0 ~msg: parallel 4.1
0   │   │   │   │   0>   0.0 ~msg: parallel 4.2
│   │   │   │   4   4>   0.4 ~msg: parallel 4.0
│   │   │   │   4   4>   0.4 ~msg: parallel 4.1
│   1   │   │   │   1>   0.1 ~msg: parallel 4.0
│   │   │   │   4   4>   0.4 ~msg: parallel 4.2
│   1   │   │   │   1>   0.1 ~msg: parallel 4.1
│   1   │   │   │   1>   0.1 ~msg: parallel 4.2
°───┴───┴───┴───┘   °>     0 ~join
°>     0 ~end

```
