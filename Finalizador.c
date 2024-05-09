#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "./heads/cons.h"
#include "./heads/memManagement.h"


int main() {
    bool memoryDestroyed = destroy_memory_block("./ProductorProcesos.c", 0, 65);
    bool statesMemoryDestroyed = destroy_memory_block("./ProductorProcesos.c", 0, 66);

    if (!memoryDestroyed || !statesMemoryDestroyed) {
        printf("Error: No se pudo destruir la memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    printf("Memoria compartida destruida\n");
    return 0;
}
