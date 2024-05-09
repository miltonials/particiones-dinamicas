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

// void print_memory_status(int *processMem, int *statesMemory, int num_lines);

void print_memory_status(int *processMem, int *statesMemory, int num_lines) {
    int prevProcess;
    int stateIndex = 0;
    printf("Estado de la memoria:\n");
    for (int i = 0; i < num_lines; i++) {
        if (processMem[i] != 0) {
            printf("Línea %d: Ocupada por el proceso %d ", i, processMem[i]);

            if (processMem[i] != prevProcess) {
                prevProcess = processMem[i];
                stateIndex = i;
            }
            
            switch (statesMemory[stateIndex]) {
                case 1:
                    printf("(Accediendo a memoria)\n");
                    break;
                case 2:
                    printf("(Ejecutando)\n");
                    break;
                case 3:
                    printf("(Bloqueado)\n");
                    break;
            }
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

int largo(int *processMem) {
    int x = sizeof(processMem) / sizeof(processMem[0]);
    return x;
}

void menu(int *processMem, int *statesMem) {
    int option;
    int memSize = largo(processMem);
    do {
        printf("Seleccione una opción:\n");
        printf("1. Estado de la memoria\n");
        printf("2. Estado de los procesos\n");
        printf("3. Salir\n");
        printf("Opción: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                print_memory_status(processMem, statesMem, memSize);
                break;
            case 2:
                print_process_status(processMem, statesMem, memSize);
                break;
            case 3:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opción inválida\n");
                break;
        }
    } while (option != 3);
}

int main() {
    int *processMem = attach_memory_block("./ProductorProcesos.c", 0, 65);
    int *statesMem = attach_memory_block("./ProductorProcesos.c", 0, 66);

    if (processMem == NULL || statesMem == NULL) {
        printf("Error al adjuntar la memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    menu(processMem, statesMem);

    return 0;
}
