#ifndef MEMMANAGEMENT_H
#define MEMMANAGEMENT_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static int get_shared_block(char *filename, int size, int id) {
  key_t key;
  int shmid;

  if ((key = ftok(filename, id)) == -1) {
    perror("ftok");
    return -1;
  }

  // return shmget(key, size, IPC_CREAT | 0666);
  return shmget(key, size, 0);
}

int* attach_memory_block(char *filename, int size, int id) {
  int shmid = get_shared_block(filename, size, id);
  int *result;

  if (shmid == -1) {
    return NULL;
  }

  result = shmat(shmid, NULL, 0);
  if (result == (void *)-1) {
    perror("shmat");
    return NULL;
  }

  return result;
}

bool detach_memory_block(int *block) {
  return shmdt(block);
}

bool destroy_memory_block(char *filename, int size, int id) {
  int shmid = get_shared_block(filename, size, id);
  if (shmid == -1) {
    return 0;
  }

  return (shmctl(shmid, IPC_RMID, NULL) != -1);
}

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