#ifndef SHM_H
#define SHM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "globals.h"
#include <glib.h>

#define NUMBER_CHUNKS 16

typedef struct chunkMetadata{
  int chunkID;
  int lastAccess;
  int pin;
}chunkMetadata;
typedef struct dsMetaData{
  chunkMetadata *chunks;
  
}dsMetaData;


void *shmalloc(char *shm_file, int *fd, int chunkSize);
void  shmrelease(void * buf, int chunkSize);
void  shmdestroy(char *shm_file);

#endif
