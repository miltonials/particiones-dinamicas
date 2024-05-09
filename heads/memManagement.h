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

static int getMemId(key_t key, int shmid) {
  struct shmid_ds shmid_ds_buf;
  int result = shmctl(shmid, IPC_STAT, &shmid_ds_buf);
  if (result == -1) {
      perror("shmctl");
      exit(EXIT_FAILURE);
  }
  return shmid_ds_buf.shm_cpid;
}

static int getShmId(int key) {
  key_t processMemKey = ftok("./ProductorProcesos.c", key);
  int processMemId = shmget(processMemKey, 0, 0);
  return processMemId;
}

static int getMemSize(key_t key, int shmid) {
  struct shmid_ds shmid_ds_buf;
  int result = shmctl(shmid, IPC_STAT, &shmid_ds_buf);
  if (result == -1) {
      perror("shmctl");
      exit(EXIT_FAILURE);
  }
  return shmid_ds_buf.shm_segsz;
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

#endif