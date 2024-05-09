#include "./heads/cons.h"
#include "./heads/strucs.h"
#include "./heads/memManagement.h"


int main() {
    int num_lineas;
    printf("Ingrese la cantidad de líneas o espacios de memoria: ");
    scanf("%d", &num_lineas);

    int mem_size = num_lineas * sizeof(int);

    key_t processMemKey = ftok("./ProductorProcesos.c", 65); // https://www.ibm.com/docs/es/zos/3.1.0?topic=functions-ftok-generate-interprocess-communication-ipc-key
    int processMemId = shmget(processMemKey, mem_size, IPC_CREAT | 0666); // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-shmget-get-shared-memory-segment
    
    key_t statesMemKey = ftok("./ProductorProcesos.c", 66);
    int statesMemId = shmget(statesMemKey, mem_size, IPC_CREAT | 0666);

    if (processMemId == -1 || statesMemId == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    //imprimir los datos de las memorias
    printf("---------------------- Process memory ----------------------\n");
    printf("Key: %d\n", processMemKey);
    printf("ID: %d\n", processMemId);
    printf("Size: %d bytes\n", mem_size);
    printf("-----------------------------------------------------------\n");
    printf("---------------------- States memory -----------------------\n");
    printf("Key: %d\n", statesMemKey);
    printf("ID: %d\n", statesMemId);
    printf("Size: %d bytes\n", mem_size);
    printf("-----------------------------------------------------------\n");
    
    printf("---------------------- Memory parameters -----------------------\n");
    printf("Number of lines: %d\n", num_lineas);
    printf("Line size: %ld bytes\n", sizeof(int));
    printf("Memory size: %d bytes\n", mem_size);
    printf("-----------------------------------------------------------\n");


    printf("Testing memory access...\n%d\n", getMemSize(65, processMemId));


    return 0;
}
