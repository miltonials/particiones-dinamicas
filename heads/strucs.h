#ifndef STRUCS_H
#define STRUCS_H

typedef struct {
    int tid; // ID del hilo
    int size; // Tamaño del hilo
    int sleep; // Tiempo de espera del hilo
} ThreadInfo;

typedef struct {
    int pid;     // ID del proceso
    int status;  // Estado del proceso: 0 -> bloqueado, 1 -> ejecutando, 2 -> accediendo a memoria
} ProcessStatus;

typedef struct {
    int *memory;      // Puntero a la memoria compartida
    int num_lines;    // Número de líneas de la memoria compartida
    int algorithm;    // Algoritmo de asignación de memoria
    pthread_mutex_t mutex; // Mutex para la exclusión mutua
    sem_t *memory_sem; // Semáforo para gestionar el acceso a la memoria
    ProcessStatus process; // Estructura para almacenar el estado del proceso
} ThreadArgs;

#endif