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
    int i, j, min = num_lines, index = -1, count = 0;
    for (i = 0; i < num_lines; i++) {
        if (memory[i] == 0) {
            count++;
            if (count == size) {
                if (count < min) {
                    min = count;
                    index = i - count + 1;
                }
            }
        } else {
            count = 0;
        }
    }
    return index;

}
#endif