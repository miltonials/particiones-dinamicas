#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "./heads/worstFit.h"

#define MEM_SIZE 1024 // Tamaño de la memoria compartida en bytes
#define MIN_SIZE 1     // Tamaño mínimo de un hilo en líneas
#define MAX_SIZE 10    // Tamaño máximo de un hilo en líneas
#define MIN_SLEEP 20   // Tiempo de espera mínimo en segundos
#define MAX_SLEEP 60   // Tiempo de espera máximo en segundos

typedef struct {
    int tid;
    int size;
    int sleep
} ThreadInfo;

// Estructura para pasar múltiples argumentos a la función del hilo
typedef struct {
    int *memory;      // Puntero a la memoria compartida
    int num_lines;    // Número de líneas de la memoria compartida
    pthread_mutex_t mutex; // Mutex para la exclusión mutua
} ThreadArgs;


void *threadFunction(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int *memory = thread_args->memory;
    int num_lines = thread_args->num_lines;
    pthread_mutex_t *mutex = &(thread_args->mutex);
    int tid = pthread_self();
    int size = (rand() % (MAX_SIZE - MIN_SIZE + 1)) + MIN_SIZE;
    int sleep_time = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;

    // Algoritmo de worst fit para encontrar espacio en la memoria
    pthread_mutex_lock(mutex); // Bloquear el mutex antes de acceder a la memoria
    int index = worstFit(memory, num_lines, size);
    if (index != -1) { // Si se encontró espacio suficiente
        printf("Hilo %d asignado a partir de la línea %d con tamaño %d\n", tid, index, size);
        // Asignar memoria para el hilo
        int i;
        for (i = index; i < index + size; i++) {
            memory[i] = tid;
        }
    } else {
        printf("No hay suficiente espacio en la memoria para el hilo %d\n", tid);
    }
    pthread_mutex_unlock(mutex); // Desbloquear el mutex después de acceder a la memoria

    // Dormir durante el tiempo aleatorio especificado
    sleep(sleep_time);

    // Desasignar la memoria asignada para el hilo
    pthread_mutex_lock(mutex); // Bloquear el mutex antes de acceder a la memoria
    for (int i = index; i < index + size; i++) {
        memory[i] = 0;
    }
    pthread_mutex_unlock(mutex); // Desbloquear el mutex después de acceder a la memoria

    // Finalizar el hilo
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    key_t key = ftok("memoria_compartida", 65);
    int shmid = shmget(key, MEM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Adjuntar la memoria compartida
    int *memory = (int *)shmat(shmid, NULL, 0);
    if (memory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Inicializar el mutex para la exclusión mutua
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // Crear hilos hasta que ya no haya espacio en la memoria
    while (1) {
        // Crear hilo
        pthread_t thread;
        ThreadArgs thread_args;
        thread_args.memory = memory;
        thread_args.num_lines = MEM_SIZE / sizeof(int);
        thread_args.mutex = mutex;
        if (pthread_create(&thread, NULL, threadFunction, (void *)&thread_args) != 0) {
            perror("pthread_create");
            break;
        }
        // Esperar un tiempo aleatorio antes de crear otro hilo
        sleep((rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP);
    }

    // Desasociar la memoria compartida
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}
