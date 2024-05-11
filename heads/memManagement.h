#ifndef MEMMANAGEMENT_H
#define MEMMANAGEMENT_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/*
Nombre: get_shared_block
Descripción: Obtiene un bloque de memoria compartida
    * @param filename: Nombre del archivo para generar la clave
    * @param size: Tamaño del bloque de memoria
    * @param id: Identificador del bloque de memoria
    * @return: ID del bloque de memoria compartida
*/
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

/*
Nombre: getMemId
Descripción: Obtiene el ID del proceso que creó el bloque de memoria compartida
    * @param key: Clave del bloque de memoria compartida
    * @param shmid: ID del bloque de memoria compartida
    * @return: ID del proceso que creó el bloque de memoria compartida
*/
static int getMemId(key_t key, int shmid) {
  struct shmid_ds shmid_ds_buf;
  int result = shmctl(shmid, IPC_STAT, &shmid_ds_buf);
  if (result == -1) {
      perror("shmctl");
      exit(EXIT_FAILURE);
  }
  return shmid_ds_buf.shm_cpid;
}

/*
Nombre: getShmId
Descripción: Obtiene el ID del bloque de memoria compartida
    * @param key: Clave del bloque de memoria compartida
    * @return: ID del bloque de memoria compartida
*/
static int getShmId(int key) {
  key_t processMemKey = ftok("./ProductorProcesos.c", key);
  int processMemId = shmget(processMemKey, 0, 0);
  return processMemId;
}

/*
Nombre: getMemSize
Descripción: Obtiene el tamaño del bloque de memoria compartida
    * @param key: Clave del bloque de memoria compartida
    * @param shmid: ID del bloque de memoria compartida
    * @return: Tamaño del bloque de memoria compartida
*/
static int getMemSize(key_t key, int shmid) {
  struct shmid_ds shmid_ds_buf;
  int result = shmctl(shmid, IPC_STAT, &shmid_ds_buf);
  if (result == -1) {
      perror("shmctl");
      exit(EXIT_FAILURE);
  }
  return shmid_ds_buf.shm_segsz;
}

/*
Nombre: attach_memory_block
Descripción: Adjunta un bloque de memoria compartida
    * @param filename: Nombre del archivo para generar la clave
    * @param size: Tamaño del bloque de memoria
    * @param id: Identificador del bloque de memoria
    * @return: Puntero al bloque de memoria compartida
*/
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

/*
Nombre: detach_memory_block
Descripción: Desadjunta un bloque de memoria compartida
    * @param block: Puntero al bloque de memoria compartida
    * @return: Verdadero si se desadjuntó correctamente, falso en caso contrario
*/
bool detach_memory_block(int *block) {
  return shmdt(block);
}

/*
Nombre: destroy_memory_block
Descripción: Destruye un bloque de memoria compartida
    * @param filename: Nombre del archivo para generar la clave
    * @param size: Tamaño del bloque de memoria
    * @param id: Identificador del bloque de memoria
    * @return: Verdadero si se destruyó correctamente, falso en caso contrario
*/
bool destroy_memory_block(char *filename, int size, int id) {
  int shmid = get_shared_block(filename, size, id);
  if (shmid == -1) {
    return 0;
  }

  return (shmctl(shmid, IPC_RMID, NULL) != -1);
}

#endif