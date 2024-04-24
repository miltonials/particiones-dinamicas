#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>

#include "./heads/worstFit.h"

#define MEM_SIZE 1024 // Tamaño de la memoria compartida en bytes
#define MIN_SIZE 1     // Tamaño mínimo de un hilo en líneas
#define MAX_SIZE 10    // Tamaño máximo de un hilo en líneas
#define MIN_SLEEP 20   // Tiempo de espera mínimo en segundos
#define MAX_SLEEP 60   // Tiempo de espera máximo en segundos

typedef struct {
    int tid;
    int size;
    int sleep;
} ThreadInfo;

typedef struct {
    int *memory;      // Puntero a la memoria compartida
    int num_lines;    // Número de líneas de la memoria compartida
    int algorithm;    // Algoritmo de asignación de memoria
    pthread_mutex_t mutex; // Mutex para la exclusión mutua
    sem_t *memory_sem; // Semáforo para gestionar el acceso a la memoria
} ThreadArgs;

void write_log(int tid, int action_type, int index, int size) {
    printf("Escribiendo en la bitácora...\n");
}

void *threadFunction(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int *memory = thread_args->memory;
    int num_lines = thread_args->num_lines;
    pthread_mutex_t *mutex = &(thread_args->mutex);
    sem_t *memory_sem = thread_args->memory_sem;
    int tid = pthread_self();
    int size = (rand() % (MAX_SIZE - MIN_SIZE + 1)) + MIN_SIZE;
    int sleep_time = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;

    sem_wait(memory_sem); // Pedir semáforo de memoria

    // Algoritmo de asignación de memoria
    int index;
    pthread_mutex_lock(mutex); // Bloquear el mutex antes de acceder a la memoria
    switch (thread_args->algorithm) {
        case 1:
            index = worstFit(memory, num_lines, size);
            break;
        case 2:
            printf("Algoritmo no implementado\n");
            index = -1;
            break;
        case 3:
            printf("Algoritmo no implementado\n");
            index = -1;
            break;
        default:
            printf("Algoritmo no implementado\n");
            index = -1;
            break;
    }

    if (index != -1) {
        write_log(tid, 1, index, size); // Escribir en Bitácora (asignación)
        printf("Hilo %d asignado a partir de la línea %d con tamaño %d\n", tid, index, size);
        // Asignar memoria para el hilo
        int i;
        for (i = index; i < index + size; i++) {
            memory[i] = tid;
        }
    } else {
        write_log(tid, 0, -1, -1); // Escribir en Bitácora (no hay suficiente espacio)
        printf("No hay suficiente espacio en la memoria para el hilo %d\n", tid);
    }
    pthread_mutex_unlock(mutex); // Desbloquear el mutex después de acceder a la memoria
    sem_post(memory_sem); // Devolver semáforo de memoria
    // Dormir durante el tiempo aleatorio especificado
    sleep(sleep_time);
    sem_wait(memory_sem); // Pedir semáforo de memoria

    // Liberar memoria
    pthread_mutex_lock(mutex);
    for (int i = index; i < index + size; i++) {
        memory[i] = 0;
    }
    pthread_mutex_unlock(mutex);
    write_log(tid, -1, index, size); // Escribir en Bitácora (liberación)
    sem_post(memory_sem); // Devolver semáforo de memoria
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

    // Crear e inicializar el semáforo de memoria
    sem_t *memory_sem = (sem_t *)malloc(sizeof(sem_t));
    sem_init(memory_sem, 0, 1); // Inicializar el semáforo con valor 1

    int algorithm_choice;
    printf("Seleccione el algoritmo de asignación de memoria:\n");
    printf("1. Worst-Fit\n");
    printf("2. Best-Fit (Aún no implementado)\n");
    printf("3. First-Fit (Aún no implementado)\n");
    printf("4. Salir\n");
    printf("Opción: ");
    scanf("%d", &algorithm_choice);

    if (algorithm_choice < 1 || algorithm_choice > 4) {
        printf("Opción no válida\n");
        exit(EXIT_FAILURE);
    }

    if(algorithm_choice == 4) {
        printf("Saliendo...\n");
        exit(EXIT_SUCCESS);
    }

    while (1) {
        pthread_t thread;
        ThreadArgs thread_args;
        thread_args.memory = memory;
        thread_args.num_lines = MEM_SIZE / sizeof(int);
        thread_args.algorithm = algorithm_choice;
        thread_args.mutex = mutex;
        thread_args.memory_sem = memory_sem;
        if (pthread_create(&thread, NULL, threadFunction, (void *)&thread_args) != 0) {
            perror("pthread_create");
            break;
        }
        
        sleep((rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP);
    }

    // Desasociar la memoria compartida
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Destruir el semáforo de memoria
    sem_destroy(memory_sem);
    free(memory_sem);

    return 0;
}
