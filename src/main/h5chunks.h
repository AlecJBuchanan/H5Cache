#ifndef H5CHUNKS_H
#define H5CHUNKS_H

#include "globals.h"
#include "hdf5.h"
#include "h5cache.h"
#include "shmht.h"

int   initChunkInfo(H5Data_t *data);
void  getOffset(hsize_t * offset, hsize_t * size, int * chunk, int rank);
void *readChunk(H5Meta_t *H5Meta,  int *chunkID);
int   readChunks(H5Meta_t *H5Meta);
void  getReadBounds(H5Data_t *data);
int findShmOffset(struct shmht *ht, char *chunk);
void *allocChunk(H5Shm_t *H5Shm, hsize_t *chunk, int rank, hid_t memType);
int   getChunkIDs(H5Data_t *data);

#endif
