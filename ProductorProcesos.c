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
#include "./heads/algorithms/bestFit.h"

/*
Nombre: write_log
Descripción: Escribe en la bitácora las acciones realizadas por los hilos
    * @param tid: ID del hilo
    * @param action_type: Tipo de acción realizada
    * @param index: Índice de la memoria
    * @param size: Tamaño de la memoria
*/
void write_log(int tid, int action_type, int index, int size) {
    FILE *log_file = fopen("bitacora.txt", "a");
    time_t now;
    time(&now);

    if (log_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    switch (action_type) {
        case 0:
            fprintf(log_file, "* Proceso %d: No hay suficiente espacio en la memoria", tid);
            break;
        case 1:
            fprintf(log_file, "+ Proceso %d: Asignado a partir de la línea %d con tamaño %d", tid, index, size);
            break;
        case -1:
            fprintf(log_file, "- Proceso %d: Liberado a partir de la línea %d con tamaño %d", tid, index, size);
            break;
        default:
            fprintf(log_file, "? Proceso %d: Acción desconocida", tid);
            break;
    }

    //hora
    fprintf(log_file, " - %s", ctime(&now));

    // print_memory_status(memory, 65);
    fclose(log_file);
}

/*
Nombre: getIndex
Descripción: Obtiene el índice de la memoria para un hilo
    * @param algorithm: Algoritmo de asignación de memoria
    * @param memory: Puntero a la memoria
    * @param num_lines: Número de líneas de la memoria
    * @param size: Tamaño del hilo
    * @return: Índice de la memoria
*/
int getIndex(int algorithm, int *memory, int num_lines, int size) {
    int index;
    switch (algorithm) {
        case 1:
            index = worstFit(memory, num_lines, size);
            break;
        case 2:
            index = bestFit(memory, num_lines, size);
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

/*
Nombre: changeProcessStatus
Descripción: Cambia el estado de un proceso
    * @param statesMemory: Puntero a la memoria de estados
    * @param index: Índice de la memoria
    * @param status: Estado del proceso
*/
void changeProcessStatus(int *statesMemory, int index, int status) {
    //states: 0 -> libre, 1 -> accediendo a memoria, 2 -> ejecutando, 3 -> bloqueado
    if (index >= 0) {
        statesMemory[index] = status;
    }
    else {
        printf("Verificar el estado enviado\n\tIndex: %d\n\tStatus: %d", index, status);
    }
}

/*
Nombre: threadFunction
Descripción: Función que ejecuta un hilo
    * @param args: Argumentos del hilo
*/
void *threadFunction(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *) args;
    pthread_mutex_t *mutex = &(thread_args->mutex);
    sem_t *memory_sem = thread_args->memory_sem;
    int tid = pthread_self();
    int* processMem = attach_memory_block("./ProductorProcesos.c", 0, 65);
    int* statesMem = attach_memory_block("./ProductorProcesos.c", 0, 66);
    int pSize = thread_args->num_lines;

    printf("Creando proceso con tamaño: %d\n", pSize);

    sem_wait(memory_sem); // Pedir semáforo de memoria
    pthread_mutex_lock(mutex); // Bloquear el mutex antes de acceder a la memoria

    if (processMem == NULL || statesMem == NULL) {
        printf("No se logró obtener el segmento de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    int index = getIndex(thread_args->algorithm, processMem, thread_args->mem_size / sizeof(int), 1);

    if (index != -1) {
        changeProcessStatus(statesMem, index, 1); // Estado = accediendo a memoria
        write_log(tid, 1, index, pSize); // Escribir en Bitácora (asignación)
        printf("Proceso %d asignado a partir de la línea %d con tamaño %d\n", tid, index, pSize);
        // Se asigna la memoria para el proceso
        for (int i = index; i < index + pSize; i++) {
            processMem[i] = tid;
        }
        sleep(5); //sleep de 5s para que se pueda ver en el programa espía cuando esté accediendo a memoria :)
    } else {
        write_log(tid, 0, -1, -1); // Escribir en Bitácora (no hay suficiente espacio)
        printf("No hay suficiente espacio en la memoria para el hilo %d\n", tid);
        pthread_mutex_unlock(mutex);
        sem_post(memory_sem); // Devolver semáforo de memoria
        pthread_exit(NULL);
    }
    changeProcessStatus(statesMem, index, 2); // Estado = ejecutando
    pthread_mutex_unlock(mutex);
    sem_post(memory_sem);
    sleep(thread_args->sleep_time); // simula ejecución

    // changeProcessStatus(statesMem, index, 3); // Estado = bloqueado, porque si alguien tiene el semáforo, entonces no puede continuar.
    sem_wait(memory_sem);
    pthread_mutex_lock(mutex);
    changeProcessStatus(statesMem, index, 1); // Estado = accediendo a memoria
    for (int i = index; i < index + pSize; i++) {
        processMem[i] = 0;
    }
    sleep(5);

    write_log(tid, -1, index, pSize); // Escribir en Bitácora (liberación)
    changeProcessStatus(statesMem, index, 3);
    pthread_mutex_unlock(mutex);
    sem_post(memory_sem); // Devolver semáforo de memoria
    
    // changeProcessStatus(statesMem, index, 0);
    pthread_exit(NULL);
}

/*
Nombre: chooseAlgorithm
Descripción: Muestra un menú para seleccionar el algoritmo de asignación de memoria
    * @return: Algoritmo seleccionado
*/
int chooseAlgorithm() {
    int algorithm_choice;
    
    while(algorithm_choice < 1 || algorithm_choice > 4) {
        printf("Seleccione el algoritmo de asignación de memoria:\n");
        printf("1. Worst-Fit\n");
        printf("2. Best-Fit\n");
        printf("3. First-Fit\n");
        printf("4. Salir\n");
        printf("Opción: ");
        scanf("%d", &algorithm_choice);
        
        if (algorithm_choice < 1 || algorithm_choice > 4) {
            printf("********************\n");
            printf("* Opción no válida *\n");
            printf("********************\n");
        }
    }

    if (algorithm_choice == 4) {
        printf("Saliendo...\n");
        exit(EXIT_SUCCESS);
    }
    
    return algorithm_choice;
}

/*
Nombre: main
Descripción: Función principal del programa
@return: 0 si el programa se ejecuta correctamente
*/
int main() {
    srand(time(NULL));
    fclose(fopen("bitacora.txt", "w")); // para limpiar la bitácora
    int memSize = getMemSize(65, getShmId(65));

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_t *memory_sem = (sem_t *)malloc(sizeof(sem_t));
    sem_init(memory_sem, 0, 1); // Se inicializa el semáforo de memoria en 1
    
    int algorithm = chooseAlgorithm();
    int sleepTime;
    while (true) {
        printf("Creando un nuevo hilo...\n");
        pthread_t thread;
        ThreadArgs thread_args;

        thread_args.num_lines = ((rand() % (MAX_SIZE -  MIN_SIZE + 1)) + MIN_SIZE);
        thread_args.sleep_time = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;
        thread_args.algorithm = algorithm;
        thread_args.mutex = mutex;
        thread_args.memory_sem = memory_sem;
        thread_args.mem_size = memSize;
        
        if (pthread_create(&thread, NULL, threadFunction, (void *)&thread_args) != 0) {
            perror("pthread_create");
            sleep(2);
        }

        
        sleepTime = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;
        printf("Esperando %d segundos para crear otro hilo...\n", sleepTime);
        sleep(sleepTime);
    }

    // Destruir el semáforo de memoria
    sem_destroy(memory_sem);
    free(memory_sem);

    return 0;
}
