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
## Compilación
Entramos al proyecto, una vez allí a la carpeta build:
```
cd build
cmake ..
cmake --build .
```

## Uso
Dentro de la carpeta `/build` se encontrá el archivo del comando ejecutable con el nombre `ompe`.
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
