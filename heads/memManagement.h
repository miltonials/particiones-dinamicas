#ifndef MEMMANAGEMENT_H
#define MEMMANAGEMENT_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>

int* getMemory(char* pMemoryName, int pKey, int pMemSize, int pPermissions) {
  key_t key = ftok(pMemoryName, key);
  int shmid = shmget(key, pMemSize, pPermissions);
  if (shmid == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  // Adjuntar la memoria compartida
  int *memory = (int *)shmat(shmid, NULL, 0); // https://www.ibm.com/docs/es/zos/3.1.0?topic=functions-shmat-attach-shared-memory-segment
  // sirve para adjuntar un segmento de memoria compartida a un proceso
  if (memory == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  return memory;
}

#endif