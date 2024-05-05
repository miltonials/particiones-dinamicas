#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "./heads/cons.h"

int main() {
    // Obtener la memoria compartida
    key_t key = ftok("memoria_compartida", 65);
    key_t key_states = ftok("memoria_compartida_estados", 65);
    int shmid = shmget(key, MEM_SIZE, 0666);
    int shmid_states = shmget(key_states, MEM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Adjuntar la memoria compartida
    int *memory = (int *)shmat(shmid, NULL, 0);
    int *memory_states = (int *)shmat(shmid, NULL, 0);
    if (memory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Liberar recursos
    if (shmctl(shmid, IPC_RMID, NULL) == -1 || shmctl(shmid_states, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    printf("Recursos liberados\n");

    // Matar todos los procesos en ejecución
    // ...

    return 0;
}
