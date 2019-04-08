#include "h5chunks.h"

int initChunkInfo(H5Data_t *data)
{
  if (H5D_CHUNKED != data->layout)
  {
    if (DEBUG) printf("NOTE  H5Cache.c - initChunkInfo: This file is not chunky\n");
    data->chunk.chunked = FALSE;
    return CHUNK_NO_CHUNKS;
  }

  int rc = SUCCESS;
  data->chunk.chunked = TRUE;
  rc = H5Pget_chunk(data->dcpl_id, data->dims, data->chunk.size);
  if (rc < 0) PRINT_AND_QUIT("ERROR H5Cache.c - initChunkInfo: Failed to get chunk size\n",CHUNK_SIZE_FAILED);
  rc = H5Sget_simple_extent_dims(data->dataSetSpace, data->dimSize, data->maxDimSize);
  if (rc < 0) PRINT_AND_QUIT("ERROR H5Cache.c - initChunkInfo: Failed to get dataset size\n",DATA_SIZE_FAILED);

  return SUCCESS;
}



void getReadBounds(H5Data_t *data)
{
  if (DEBUG) printf("INFO  H5Chunks.c - getReadBounds: Finding chunks to read\n");
  int rank = data->dims;
  for (int dim = 0; dim < rank; dim++){
    data->chunk.start[dim] = data->hyperslab.start[dim]/ data->chunk.size[dim];
    data->chunk.end[dim]   = data->hyperslab.end[dim]  / data->chunk.size[dim];
  }
}


int getNumberChunks(int *start, int *end,  int dims)
{
  int nchunks  = 1;
  for (int dim = 0; dim < dims; dim++) 
    nchunks = nchunks * (end[dim] - start[dim] + 1);
  return nchunks;
}


int getChunkIDs(H5Data_t *data)
{
  if (DEBUG) printf("INFO  H5Chunks.c - getChunkIDs: Initialization\n");
  int *start  = data->chunk.start;
  int *end    = data->chunk.end;
  int *size   = data->chunk.size;
  int dims    = data->dims;
  int nchunks = getNumberChunks(start, end, dims);

  data->chunk.chunks  = malloc((sizeof(char) * (CHUNK_ID_LEN + 1)) * nchunks);
  data->chunk.nchunks = nchunks;

  if (DEBUG) printf("INFO  H5Chunks.c - getChunkIDs: Finding chunk IDs\n");
  for (int c = 0; c < nchunks; c++){
    int chunkID[dims];
    char cid[128];
    for(int dim = 0; dim < dims; dim++){
      int diff        = end[dim] - start[dim] + 1;
      double divider  = 1;
      for (int div = 0; div < dim; div++)
        divider = divider * (end[div] - start[div] + 1);
      int oneDimChunkID = ((int)floor(((double)c)/divider)%diff) + start[dim];
      if (DEBUG) printf("INFO  H5Chunks.c - getChunkIDs: chunk %d dimension %d has ID = %d\n", c, dim, oneDimChunkID);
      // TODO add filename and dataset name
      //sprintf(chunkID, "%s-%d", chunkID, oneDimChunkID);
      chunkID[dim] = oneDimChunkID;

    }

    char curId[32];
    sprintf(curId, "-%d", chunkID[0]); 
    strcpy(cid, curId);
    for (int d = 1; d < dims; d++){
      sprintf(curId, "-%d", chunkID[d]);
      strcat(cid, curId);
    }
    printf("INFO  H5Chunks.c - getChunkIDs: Identified chunk %s%s at index %d\n", "filename-datasetname", cid, c);
    //printf("data->chunk.chunks = %p\n", data->chunk.chunks);
    //printf("data->chunk.chunks[%d * (1 + %d)] = %p\n", c, CHUNK_ID_LEN, data->chunk.chunks + (c * (1 + CHUNK_ID_LEN)));
    sprintf(data->chunk.chunks + (c * (1 + CHUNK_ID_LEN)), "filename-datasetname%s", cid);
  }
}



int readChunks(H5Meta_t *H5Meta)
{
  // TODO: read more than just one chunk
  readChunk(H5Meta, H5Meta->data.chunk.start);
}




void *readChunk(H5Meta_t *H5Meta, int *chunk)
{
  void *shm = getShmLoc(chunk);
  if (shm != NULL) return shm;
 
  int rank = H5Meta->data.dims;
  hsize_t offset[rank];
  getOffset(offset, H5Meta->data.chunk.size, chunk, rank);

  shm           = allocChunk(&H5Meta->mem.shm, H5Meta->data.chunk.size, rank);
  hid_t dspace  = H5Dget_space(H5Meta->data.dataSetSpace);
  hid_t hyperid = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, H5Meta->data.chunk.size, NULL); 	// TODO: test for correctness
  hid_t mspace  = H5Screate_simple(rank, H5Meta->data.dimSize, H5Meta->data.maxDimSize);			// TODO: test for correctness
  herr_t status = H5Dread( H5Meta->data.dataSetSpace, H5Meta->mem.mem_type_id, H5Meta->mem.mem_space_id, H5Meta->file.file_space_id, H5Meta->prop.xfer_plist_id, shm );
 
  return shm;
}

void *getShmLoc(H5shm_t *H5shm, int *chunk, int rank)
{
  int size = 1;
  for (int d = 0; d < rank; d++)
    size = size * chunk[d];

  // return pointer to location of chunk in shm
  if (SHM_SIZE - H5shm->freeSpaceOffset < size) return NULL;
  
  int offset = H5shm->freeSpaceOffset;
  H5shm->freeSpaceOffset = H5shm->freeSpaceOffset + size;
  return H5shm + offset; // TODO verify that void* addition is same as cha* addition 
}

void getOffset(hsize_t offset[], hsize_t * size, int * chunk, int rank){
  for (int dim = 0; dim < rank; dim++)
    offset[dim] = chunk[dim] * size[dim];
}



void *allocChunk(hsize_t *size)
{
  // Check if shm is full
  
 
  return NULL;
}
