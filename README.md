# OpenMP Examples (ompe)
El siguiente proyecto C++ intenta mostrar en una consola como trabaja OpenMP a traves de una visualización por conosla de los hilos generados y mesajes que pasa el usuario.

## Clonar proyecto
En una terminal vamos a la carpeta que deseamos que deseamos que esté el proyecto.
```
git clone https://github.com/anibalanto/openmp_examples
```
## Compilación
Dentro del proyecto escribimos:
```
cd build
cmake ..
cmake --build .
```

## Uso
Dentro de la carpeta `/build` se encuentra el comando ejecutable.
Para ver la ayuda del comando utilizamos la opción `-h` o `--help`:
```
./ompe -h
```
Se imprimirá:
```
uso:        ompe [<fnum>] [-l] [-h]
Estos son comandos de ompe (OpenMP Example)
       <fnum>  Número de función a ejecutar
       -l, --list
               Lista de funciones con su número de función

       -h, --help
               Ayuda
```
Con la opción `-l` o `--list` se listarán todas las funciones con ejemplos y su valor `fnum` con el que se puede ejecutar. Por ejemplo, para ejecutar la función número 3 simplemente escribimos:

```
./ompe 3
```
