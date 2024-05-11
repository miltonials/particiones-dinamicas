#ifndef BESTFIT_H
#define BESTFIT_H

// Implementación del algoritmo Best Fit
/*
    * @param memory: Puntero a la memoria
    * @param num_lines: Número de líneas de la memoria
    * @param size: Tamaño del bloque de memoria a asignar
    * @return: Índice de la primera línea del bloque asignado, o -1 si no se pudo asignar
*/
int bestFit(int *memory, int num_lines, int size) {
    int start = -1;       // Índice de inicio del bloque libre
    int len = 0;          // Longitud del bloque libre
    int min_len = num_lines;  // Longitud mínima encontrada que sea mayor o igual al tamaño solicitado
    int index = -1;       // Índice del bloque asignado
    int i;

    for (i = 0; i < num_lines; i++) {
        if (memory[i] == 0) { // Si la línea está vacía
            if (start == -1) { // Comienza un nuevo bloque libre
                start = i;
            }
            len++; // Incrementar la longitud del bloque libre
        } else { // Si la línea está ocupada
            if (len >= size && len < min_len) { // Si el bloque libre es lo suficientemente grande y menor que la longitud mínima actual
                min_len = len;
                index = start; // Asignar el bloque
            }
            start = -1; // Restablecer el índice de inicio
            len = 0;    // Restablecer la longitud
        }
    }
    if (len >= size && len < min_len) { // Comprobar el último bloque libre
        index = start;
    }

    return index; // Devolver el índice del bloque asignado

}
#endif