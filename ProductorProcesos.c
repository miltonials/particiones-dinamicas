// #include <stdio.h>
// #include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>
#include <time.h>
#include <string.h>

#include "./heads/cons.h"
#include "./heads/strucs.h"
#include "./heads/memManagement.h"
#include "./heads/algorithms/worstFit.h"
#include "./heads/algorithms/firstFit.h"

void write_log(int tid, int action_type, int index, int size) {
    FILE *log_file = fopen("bitacora.txt", "a");
    if (log_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    switch (action_type) {
        case 0:
            fprintf(log_file, "Hilo %d: No hay suficiente espacio en la memoria\n", tid);
            break;
        case 1:
            fprintf(log_file, "Hilo %d: Asignado a partir de la línea %d con tamaño %d\n", tid, index, size);
            break;
        case -1:
            fprintf(log_file, "Hilo %d: Liberado a partir de la línea %d con tamaño %d\n", tid, index, size);
            break;
        default:
            fprintf(log_file, "Hilo %d: Acción desconocida\n", tid);
            break;
    }

    // print_memory_status(memory, 65);
    fclose(log_file);
}

int getIndex(int algorithm, int *memory, int num_lines, int size) {
    int index;
    switch (algorithm) {
        case 1:
            index = worstFit(memory, num_lines, size);
            break;
        case 2:
            printf("Algoritmo no implementado\n");
            index = -1;
            break;
        case 3:
            index = firstFit(memory, num_lines, size);
            break;
        default:
            printf("Algoritmo no implementado\n");
            index = -1;
            break;
    }

    return index;
}

void changeProcessStatus(int *statesMemory, int index, int status) {
    //states: 1 -> accediendo a memoria, 2 -> ejecutando, 0 -> bloqueado
    if (index >= 0) {
        statesMemory[index] = status;
    }
}

void *threadFunction(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *) args;
    int *memory = thread_args->memory;
    int *memory_states = thread_args->memory_states;
    int num_lines = thread_args->num_lines;
    pthread_mutex_t *mutex = &(thread_args->mutex);
    sem_t *memory_sem = thread_args->memory_sem;
    int tid = pthread_self();
    int size = (rand() % (MAX_SIZE - MIN_SIZE + 1)) + MIN_SIZE;
    int sleep_time = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;
    // ProcessStatus process = thread_args->process;
    sem_wait(memory_sem); // Pedir semáforo de memoria
    pthread_mutex_lock(mutex); // Bloquear el mutex antes de acceder a la memoria
    int index = getIndex(thread_args->algorithm, memory, num_lines, size);


    if (index != -1) {
        changeProcessStatus(memory_states, index, 1); // Cambiar estado a Accediendo a memoria
        write_log(tid, 1, index, size); // Escribir en Bitácora (asignación)
        // printf("Hilo %d asignado a partir de la línea %d con tamaño %d (proceso %d)\n", tid, index, size, process.pid);
        // Asignar memoria para el hilo
        for (int i = index; i < index + size; i++) {
            memory[i] = tid;
        }
        sleep(2);
        changeProcessStatus(memory_states, index, 2); // Cambiar estado a Ejecutando
    } else {
        write_log(tid, 0, -1, -1); // Escribir en Bitácora (no hay suficiente espacio)
        printf("No hay suficiente espacio en la memoria para el hilo %d\n", tid);
    }
    pthread_mutex_unlock(mutex);
    sem_post(memory_sem);
    sleep(sleep_time);

    sem_wait(memory_sem);
    pthread_mutex_lock(mutex);
    // Liberar memoria
    for (int i = index; i < index + size && index >= 0; i++) {
        memory[i] = 0;
    }

    changeProcessStatus(memory_states, index, 3); // Cambiar estado a Bloqueado
    sleep(sleep_time);
    pthread_mutex_unlock(mutex);
    write_log(tid, -1, index, size); // Escribir en Bitácora (liberación)
    sem_post(memory_sem); // Devolver semáforo de memoria
    // changeProcessStatus(memory_states, index, 0); // Cambiar estado a Bloqueado
    pthread_exit(NULL);
}

int chooseAlgorithm() {
    int algorithm_choice;
    printf("Seleccione el algoritmo de asignación de memoria:\n");
    printf("1. Worst-Fit\n");
    printf("2. Best-Fit (Aún no implementado)\n");
    printf("3. First-Fit \n");
    printf("4. Salir\n");
    printf("Opción: ");
    scanf("%d", &algorithm_choice);

    if (algorithm_choice < 1 || algorithm_choice > 4) {
        printf("Opción no válida\n");
        exit(EXIT_FAILURE);
    } else if (algorithm_choice == 4) {
        printf("Saliendo...\n");
        exit(EXIT_SUCCESS);
    }
    
    return algorithm_choice;
}

int main() {
    srand(time(NULL));
    int* memory = attach_memory_block("./ProductorProcesos.c", MEM_SIZE, 65);
    int* statesMemory = attach_memory_block("./ProductorProcesos.c", MEM_SIZE, 66);
    
    if (memory == NULL || statesMemory == NULL) {
        printf("Error al adjuntar la memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_t *memory_sem = (sem_t *)malloc(sizeof(sem_t));
    sem_init(memory_sem, 0, 1); // Se inicializa el semáforo de memoria en 1
    
    int algorithm = chooseAlgorithm();
    int sleepTime;
    while (true) {
        printf("Creando un nuevo hilo...\n");
        pthread_t thread;
        ThreadArgs thread_args;

        thread_args.memory = memory;
        thread_args.num_lines = (rand() % (MEM_SIZE / sizeof(int) - 1)) + 1;
        thread_args.algorithm = algorithm;
        thread_args.mutex = mutex;
        thread_args.memory_sem = memory_sem;
        thread_args.memory_states = statesMemory;
        
        if (pthread_create(&thread, NULL, threadFunction, (void *)&thread_args) != 0) {
            perror("pthread_create");
            sleep(2);
        }

        
        sleepTime = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;
        printf("Esperando %d segundos para crear otro hilo...\n", sleepTime);
        sleep(1);
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
