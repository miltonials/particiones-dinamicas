#ifndef STRUCS_H
#define STRUCS_H

#include <semaphore.h>

typedef struct {
    int tid; // ID del hilo
    int size; // Tamaño del hilo
    int sleep; // Tiempo de espera del hilo
} ThreadInfo;

typedef struct {
    int *memory;      // Puntero a la memoria compartida
    int num_lines;    // Número de líneas de la memoria compartida
    int algorithm;    // Algoritmo de asignación de memoria
    pthread_mutex_t mutex; // Mutex para la exclusión mutua
    sem_t *memory_sem; // Semáforo para gestionar el acceso a la memoria
    int *memory_states; // Puntero a la memoria compartida de estados
} ThreadArgs;

#endif