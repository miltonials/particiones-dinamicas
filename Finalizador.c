#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEM_SIZE 1024 // Tamaño de la memoria compartida en bytes

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

    // Liberar recursos
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    printf("Recursos liberados\n");

    // Matar todos los procesos en ejecución
    // ...

    return 0;
}
