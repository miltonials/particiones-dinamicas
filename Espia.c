#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>

#define MEM_SIZE 1024 // Tamaño de la memoria compartida en bytes

// Estructura para representar el estado de un proceso
typedef struct {
    int pid;     // ID del proceso
    int status;  // Estado del proceso: 0 -> bloqueado, 1 -> ejecutando, 2 -> accediendo a memoria
} ProcessStatus;

// Función para imprimir el estado de la memoria
#include <stdio.h>
#include <stdlib.h>

char* print_memory_status(int *memory, int num_lines) {
    char* report = (char*)malloc(1024);
    if (report == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }

    printf("Estado de la memoria:\n");
    for (int i = 0; i < num_lines; i++) {
        if (memory[i] != 0) {
            printf("Línea %d: Ocupada por el proceso %d\n", i, memory[i]);
            strcat(report, "Línea ");
            char line[10];
            sprintf(line, "%d", i);
            strcat(report, line);
            strcat(report, ": Ocupada por el proceso ");
            char process[10];
            sprintf(process, "%d", memory[i]);
            strcat(report, process);
            strcat(report, "\n");
        } else {
            printf("Línea %d: Libre\n", i);
            strcat(report, "Línea ");
            char line[10];
            sprintf(line, "%d", i);
            strcat(report, line);
            strcat(report, ": Libre\n");
        }
    }

    return report;
}


// Función para imprimir el estado de los procesos
void print_process_status(ProcessStatus *processes, int num_processes) {
    printf("Estado de los procesos:\n");
    printf("PID\tEstado\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%d\t", processes[i].pid);
        switch (processes[i].status) {
            case 0:
                printf("Bloqueado\n");
                break;
            case 1:
                printf("Ejecutando\n");
                break;
            case 2:
                printf("Accediendo a memoria\n");
                break;
            default:
                printf("Estado desconocido\n");
                break;
        }
    }
    printf("\n");
}

int main() {
    // Obtener la memoria compartida
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

    // Crear una lista de procesos de ejemplo
    int num_processes = 3;
    ProcessStatus processes[num_processes];
    processes[0].pid = 123;
    processes[0].status = 0; // Bloqueado
    processes[1].pid = 456;
    processes[1].status = 1; // Ejecutando
    processes[2].pid = 789;
    processes[2].status = 2; // Accediendo a memoria

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
                while(1) {
                    // print_memory_status(memory, MEM_SIZE / sizeof(int));
                    char *report = print_memory_status(memory, MEM_SIZE / sizeof(int));
                    // guardar en Bitacora.txt
                    FILE *bitacora = fopen("Bitacora.txt", "a");
                    if (bitacora == NULL) {
                        perror("fopen");
                        exit(EXIT_FAILURE);
                    }

                    //vaciar el contenido de la bitacora
                    fprintf(bitacora, "%s", "");
                    fprintf(bitacora, "%s", report);
                    fclose(bitacora);
                    free(report);

                    sleep(1);
                }
                break;
            case 2:
                print_process_status(processes, num_processes);
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
