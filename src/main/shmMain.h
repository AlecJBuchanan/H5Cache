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
//#include <glib.h>
#include <errno.h>
#include "shmht.h"

#define NUMBER_CHUNKS 64
#define CHUNK_ID_LEN  128

#define SHM_FILE   "h5cacheData"
#define ID_INDEX   "h5cacheIdIndex"
#define LRU_INDEX  "h5cacheLruIndex"
#define CHUNK_LIST "h5cacheChunkInfo"

typedef struct chunkMetadata{
  int chunkID;
  int lastAccess;
  int pin;
  int offset;
}chunkMetadata;
/*typedef struct mainMetaData{
  chunkMetadata *chunks;
  GHashTable*    index; 
}mainMetaData;
*/

int   initShm(H5Shm_t *shmInfo);
void *shmalloc(char *shm_file, int *fd, int chunkSize);
void  shmrelease(void * buf, int chunkSize);
void  shmdestroy(char *shm_file);

#endif
