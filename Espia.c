#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./heads/cons.h"
#include "./heads/strucs.h"

char* print_memory_status(int *memory, int *statesMemory, int num_lines) {
    char* report = (char*)malloc(1024);
    if (report == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    int prevProcess;
    printf("Estado de la memoria:\n");
    for (int i = 0; i < num_lines; i++) {
        if (memory[i] != 0) {
            printf("Línea %d: Ocupada por el proceso %d ", i, memory[i]);
            if (memory[i] != prevProcess) {
                prevProcess = memory[i];
                switch (statesMemory[i]) {
                    case 0:
                        printf("(Bloqueado)");
                        break;
                    case 1:
                        printf("(Ejecutando)");
                        break;
                    case 2:
                        printf("(Accediendo a memoria)");
                        break;
                    default:
                        printf("(Estado desconocido)");
                        break;
                }
            }
            printf("\n");
        } else {
            printf("Línea %d: Libre\n", i);
        }
    }

    return report;
}


// Función para imprimir el estado de los procesos
void print_process_status(int *memory, int *statesMemory, int num_lines) {
    printf("Estado de los procesos:\n");
    printf("PID\tEstado\n");
    int prevProcess;
    for (int i = 0; i < num_lines; i++) {
        if (memory[i] != prevProcess) {
            prevProcess = memory[i];
            printf("%d\t", memory[i]);
            switch (statesMemory[i]) {
                case 0:
                    printf("No entra aca\n");
                    break;
                case 1:
                    printf("Accediendo a memoria\n");
                    break;
                case 2:
                    printf("Ejecutando\n");
                    break;
                default:
                    printf("Bloqueado\n");
                    break;
            }
        }
    }
}

int main() {
    // Obtener la memoria compartida
    key_t key = ftok("memoria_compartida", 65);
    key_t statesSharedMemoryKey = ftok("estados_memoria_compartida", 65);
    int shmid = shmget(key, MEM_SIZE, 0666);
    int statesSharedMemoryId = shmget(statesSharedMemoryKey, MEM_SIZE, 0666);
    if (shmid == -1 || statesSharedMemoryId == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Adjuntar la memoria compartida
    int *memory = (int *)shmat(shmid, NULL, 0);
    int *statesMemory = (int *)shmat(shmid, NULL, 0);

    if (memory == (void *)-1 || statesMemory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }


    // Interacción con el usuario
    int option;
    do {
        printf("Seleccione una opción:\n");
        printf("1. Estado de la memoria\n");
        printf("2. Estado de los procesos\n");
        printf("3. Salir\n");
        printf("Opción: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                print_memory_status(memory, statesMemory, MEM_SIZE / sizeof(int));
                break;
            case 2:
                print_process_status(memory, statesMemory, MEM_SIZE / sizeof(int));
                break;
            case 3:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opción inválida\n");
                break;
        }
    } while (option != 3);

    // Desasociar la memoria compartida
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}
