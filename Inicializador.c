#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> // Agregado para getpid()

#define PAGE_SIZE 4096 // Tamaño de página del sistema

int main() {
    // Pedir al usuario la cantidad de líneas o espacios de memoria
    int num_lineas;
    printf("Ingrese la cantidad de líneas o espacios de memoria: ");
    scanf("%d", &num_lineas);

    // Redondear el tamaño de la memoria para que sea un múltiplo del tamaño de página
    int mem_size = ((num_lineas * sizeof(int)) / PAGE_SIZE + 1) * PAGE_SIZE;

    // Solicitar memoria compartida al sistema operativo
    key_t key = ftok("memoria_compartida", 65);
    int shmid = shmget(key, mem_size, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    printf("Memoria compartida creada con ID: %d\n", shmid);

    // Liberar recursos y salir
    // shmctl(shmid, IPC_RMID, NULL);
    // printf("Recursos liberados\n");

    return 0;
}
