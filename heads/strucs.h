#ifndef STRUCS_H
#define STRUCS_H

#include <semaphore.h>



// Estructura para almacenar los argumentos de un hilo
typedef struct {
    int num_lines;    // Número de líneas de la memoria compartida
    int sleep_time;    //20 - 60 secs
    int algorithm;    // Algoritmo de asignación de memoria
    pthread_mutex_t mutex; // Mutex para la exclusión mutua
    sem_t *memory_sem; // Semáforo para gestionar el acceso a la memoria
    int mem_size;
} ThreadArgs;

#endif