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

    // Mostrar estado de la memoria
    printf("Estado de la memoria:\n");
    int i;
    for (i = 0; i < MEM_SIZE / sizeof(int); i++) {
        if (memory[i] == 0) {
            printf("Línea %d: Vacía\n", i);
        } else {
            printf("Línea %d: Proceso %d\n", i, memory[i]);
        }
    }

    // Desasociar la memoria compartida
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}
