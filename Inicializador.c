#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int num_lines;  // Variable para almacenar la cantidad de líneas o espacios de memoria

    // Solicitar al usuario la cantidad de líneas o espacios de memoria
    printf("Ingrese la cantidad de líneas o espacios de memoria: ");
    scanf("%d", &num_lines);

    // Solicitar memoria compartida al sistema operativo
    int shmid = shmget(IPC_PRIVATE, num_lines * sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error al solicitar memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria compartida a la memoria del proceso
    int *shared_memory = (int *)mmap(NULL, num_lines * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (shared_memory == MAP_FAILED) {
        perror("Error al mapear memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Inicializar la memoria compartida
    for (int i = 0; i < num_lines; i++) {
        shared_memory[i] = 0;  // Inicializar cada línea o espacio de memoria a 0
    }

    // Inicializar la memoria compartida
    // Aquí podrías agregar código adicional para inicializar la memoria si fuera necesario

    printf("Inicialización completa. Memoria compartida creada en %d.\n", shmid);

    // El proceso finaliza después de la inicialización
    exit(EXIT_SUCCESS);
}
