#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> // Agregado para getpid()

#include "./heads/cons.h"
#include "./heads/strucs.h"

int main() {
    int num_lineas;
    printf("Ingrese la cantidad de líneas o espacios de memoria: ");
    scanf("%d", &num_lineas);

    // Redondear el tamaño de la memoria para que sea un múltiplo del tamaño de página
    // int mem_size = ((num_lineas * sizeof(int)) / PAGE_SIZE + 1) * PAGE_SIZE;
    int mem_size = ((num_lineas * sizeof(int)) / PAGE_SIZE + 1) * PAGE_SIZE;
    key_t key = ftok("memoria_compartida", 65); // https://www.ibm.com/docs/es/zos/3.1.0?topic=functions-ftok-generate-interprocess-communication-ipc-key
    int shmid = shmget(key, mem_size, IPC_CREAT | 0666); // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-shmget-get-shared-memory-segment
    
    key_t statesSharedMemoryKey = ftok("estados_memoria_compartida", 65);
    int statesSharedMemoryId = shmget(statesSharedMemoryKey, mem_size, IPC_CREAT | 0666);

    if (shmid == -1 || statesSharedMemoryId == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    printf("Memoria compartida creada con ID: %d\n", shmid);
    printf("PID: %d\n", getpid());
    printf("Shmid: %d\n", shmid);
    printf("Key: %d\n", key);
    printf("Tamaño de la memoria: %d\n", mem_size);
    printf("Número de líneas: %d\n", num_lineas);
    printf("Tamaño de página: %d\n", PAGE_SIZE);
    return 0;
}
