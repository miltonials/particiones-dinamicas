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
#include "./heads/memManagement.h"

void print_memory_status(int *memory, int *statesMemory, int num_lines) {
    int prevProcess;
    int stateIndex = 0;
    printf("Estado de la memoria:\n");
    for (int i = 0; i < num_lines; i++) {
        if (memory[i] != 0) {
            printf("Línea %d: Ocupada por el proceso %d ", i, memory[i]);
            if (memory[i] != prevProcess) {
                prevProcess = memory[stateIndex];
                switch (statesMemory[stateIndex]) {
                    case 1:
                        printf("(Accediendo a memoria)");
                        break;
                    case 2:
                        printf("(Ejecutando)");
                        break;
                    default:
                        printf("(Bloqueado)");
                        break;
                }
            }
            else {
                stateIndex = i;
            }
            printf("\n");
        } else {
            printf("Línea %d: Libre\n", i);
        }
    }
}


// Función para imprimir el estado de los procesos
void print_process_status(int *memory, int *statesMemory, int num_lines) {
    printf("Estado de los procesos:\n");
    printf("PID\tEstado\n");
    int prevProcess;
    for (int i = 0; i < num_lines; i++) {
        if (memory[i] != prevProcess && memory[i] != 0) {
            prevProcess = memory[i];
            printf("%d\t", memory[i]);
            switch (statesMemory[i]) {
                case 1:
                    printf("Accediendo a memoria\n");
                    break;
                case 2:
                    printf("Ejecutando\n");
                    break;
                case 3:
                    printf("Bloqueado\n");
                    break;
                default:
                    printf("Desconocido\n");
                    break;
            }
        }
    }
}

int main() {
    // Obtener la memoria compartida
    int *memory = attach_memory_block("./ProductorProcesos.c", MEM_SIZE, 65);
    int *statesMemory = attach_memory_block("./ProductorProcesos.c", MEM_SIZE, 66);


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
