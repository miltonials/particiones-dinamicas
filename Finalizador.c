#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    // Solicitar memoria compartida al sistema operativo
    int shmid = 40;

    // Eliminar la memoria compartida
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar memoria compartida");
        exit(EXIT_FAILURE);
    }

    printf("Finalización de memoria completa. Memoria compartida eliminada.\n");

    return 0;
}
