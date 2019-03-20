#ifndef H5CHUNKS_H
#define H5CHUNKS_H

#include "globals.h"
#include "hdf5.h"
#include "h5cache.h"

int initChunkInfo(H5Data_t *data);
void getOffset(hsize_t * offset, hsize_t * size, int * chunk, int rank);
void *readChunk(H5Meta_t *H5Meta, int *chunk);
int readChunks(H5Meta_t *H5Meta);
void getReadBounds(H5Data_t *data);
void *getShmLoc(int *chunk);
void *allocChunk(hsize_t *size);

#endif
