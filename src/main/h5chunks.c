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

void getChunkID(int *chunk, hsize_t *csize, int rank, char *chunkID)
{
  //char *chunkID = malloc(sizeof(char) * (CHUNK_ID_LEN));
  sprintf(chunkID, "%s", "filename-datasetname");
  for (int dim = 0; dim < rank; dim++){
    int dimID = chunk[dim];// * csize[dim];
    sprintf(chunkID, "%s-%d", chunkID, dimID);
  }
}

// TODO make getChunkIDs take advantage of getchunkID
int getChunkIDs(H5Data_t *data)
{
  if (DEBUG) printf("INFO  H5Chunks.c - getChunkIDs: Initialization\n");
  int *start    = data->chunk.start;
  int *end      = data->chunk.end;
  hsize_t *size = data->chunk.size;
  int dims      = data->dims;
  int nchunks   = getNumberChunks(start, end, dims);

  data->chunk.chunks  = malloc((sizeof(char) * (CHUNK_ID_LEN)) * nchunks);
  data->chunk.nchunks = nchunks;
  memset(data->chunk.chunks, 0, (sizeof(char) * (CHUNK_ID_LEN)) * nchunks);

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

    // TODO: delete above and below code because we made getChunkID

    sprintf(data->chunk.chunks + (c * (CHUNK_ID_LEN)), "filename-datasetname%s", cid);
  }

  for (int c = 0; c < nchunks; c++) printf("%d. %s\n", c, data->chunk.chunks + (c * (CHUNK_ID_LEN))); // TODO: remove
}


int getChunkSize(hsize_t *chunkSize, int rank)
{
  int size = 1;
  for (int dim = 0; dim < rank; dim++)
    size = size * chunkSize[dim];
  return size * sizeof(int);
}


int readChunks(H5Meta_t *H5Meta)
{
  // TODO: read more than just one chunk
  //readChunk(H5Meta, H5Meta->data.chunk.start);
  
  /*int nchunks = H5Meta->data.chunk.nchunks;
  for (int chunk = 0; chunk < nchunks; chunk++){
    readChunk(H5Meta, H5Meta->data.chunk.chunks + (chunk * CHUNK_ID_LEN));
    // 1. copy data into output buffer
  }*/


  int *start    = H5Meta->data.chunk.start;
  int *end      = H5Meta->data.chunk.end;
  int ndims     = H5Meta->data.dims;
  int nchunks   = getNumberChunks(start, end, ndims);
  int chunkSize = getChunkSize(H5Meta->data.chunk.size, ndims); // ASSUMES INT
  H5Meta->data.chunk.nchunks = nchunks;

  for(int chunk = 0; chunk < nchunks; chunk++){
    int chunkID[ndims];
    for(int dim = 0; dim < ndims; dim++){
      int diff        = end[dim] - start[dim] + 1;
      double divider  = 1;
      for (int div = 0; div < dim; div++)
        divider = divider * (end[div] - start[div] + 1);
      chunkID[dim] = ((int)floor(((double)chunk)/divider)%diff) + start[dim];
    }
//printf("about to read %d,%d,%d", chunkID[0], chunkID[1], chunkID[2]);
    void *shmChunk = readChunk(H5Meta, chunkID);
    void *bufLoc   = H5Meta->mem.buf + (chunk * chunkSize);
    memcpy(bufLoc, shmChunk, chunkSize);
  }
}




void *readChunk(H5Meta_t *H5Meta, int *chunk)
{
  void    *shm   = H5Meta->mem.shm.data;
  int      rank  = H5Meta->data.dims;
  hsize_t *csize = H5Meta->data.chunk.size;

  char    chunkID[CHUNK_ID_LEN] = {0};
  hsize_t offset [rank]; 
  getChunkID(chunk, csize, rank, chunkID);


  int shmOffset = findShmOffset(H5Meta->mem.shm.idIndex ,chunkID);
  if (shmOffset >= 0) return ((char *)shm) + shmOffset;
 
  if (DEBUG) printf("INFO  H5Chunks.c - readChunk: Could not find chunk in shm\n");
  getOffset(offset, H5Meta->data.chunk.size, chunk, rank);
  shm           = allocChunk(&H5Meta->mem.shm, H5Meta->data.chunk.size, rank, H5Meta->mem.mem_type_id);
  hid_t dspace  = H5Dget_space(H5Meta->data.dataset_id);
  hid_t hyperid = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, H5Meta->data.chunk.size, NULL); 	// TODO: test for correctness
  hid_t mspace  = H5Screate_simple(rank, H5Meta->data.chunk.size, NULL); //(rank, H5Meta->data.dimSize, H5Meta->data.maxDimSize);			// TODO: test for correctness


  herr_t status = H5Dread( H5Meta->data.dataset_id, H5Meta->mem.mem_type_id, H5Meta->mem.mem_space_id, H5Meta->file.file_space_id, H5Meta->prop.xfer_plist_id, shm ); // should the fourth arg be dspace or H5Meta->file.file_space_id
  if (status >= 0){
    int chunkLoc = shm - H5Meta->mem.shm.data;
    int result   = shmht_insert (H5Meta->mem.shm.idIndex, chunkID, CHUNK_ID_LEN, &chunkLoc, sizeof(chunkLoc));
    if (result <= 0) printf("ERROR H5Chunks.c - readChunk: Inserting into shmht failed\n");
    // concerned that when shmht was created, the record size wa set to chunk_id_len. 
    // does the record contain the key and value? The record size may be too small
  }else printf("ERROR H5Chunks.c - readChunk: H5Dread failed\n");


  H5Sclose(dspace);
  return shm;
}


int findShmOffset(struct shmht *ht, char *chunk)
{
  if (DEBUG) printf("INFO  H5Chunks.c - getShmLoc: Searching for chunk %s\n", chunk);
  size_t vSize = 0;
  void *value  = shmht_search(ht, chunk, CHUNK_ID_LEN, &vSize);
  if (value == NULL) return -1;

  if (DEBUG) printf("INFO  H5Chunks.c - getShmLoc: Found chunk in shm at offset %d\n", *(int *)value);
  return *(int *)value;
}


void *getFreeSpace(H5Shm_t *H5Shm, hsize_t *chunk, int rank, hid_t memType)
{
  int size = 1;
  int freeSpaceOffset = 0;
  for (int d = 0; d < rank; d++)
    size = size * chunk[d];
  size = size * sizeof(int); // TODO: must multiply by data type size, currently subbed in 4. look into function getDatatypeSize()
  printf("TODO  H5Chunks.c - getFreeSpace: NEED TO FIND WAY TO CONVERT MEMTYPE TO A MEMORY SIZE\n");


  void * result;
  size_t retSize;
  result = shmht_search (H5Shm->idIndex, FREE_SPACE_KEY, sizeof(FREE_SPACE_KEY), &retSize);
  if (result == NULL) printf("TODO  H5Chunks.c - getFreeSpace: Faital error, could not find free space offset\n");
  freeSpaceOffset = *(int *)result;


  if (DEBUG) printf("INFO  H5Chunks.c - getFreeSpace: Looking for %d free space\n", size);
  if (SHM_SIZE - freeSpaceOffset < size) return NULL;
  

  if (DEBUG) printf("INFO  H5Chunks.c - getFreeSpace: Found free space at offset %d\n", freeSpaceOffset); 
  int offset      = freeSpaceOffset; 
  freeSpaceOffset = freeSpaceOffset + size;
  shmht_remove (H5Shm->idIndex, FREE_SPACE_KEY, sizeof(FREE_SPACE_KEY));
  shmht_insert (H5Shm->idIndex, FREE_SPACE_KEY, sizeof(FREE_SPACE_KEY), &freeSpaceOffset, sizeof(freeSpaceOffset));
  
  return H5Shm->data + offset; // TODO verify that void* addition is same as cha* addition 
}

void getOffset(hsize_t offset[], hsize_t * size, int * chunk, int rank){
  for (int dim = 0; dim < rank; dim++)
    offset[dim] = chunk[dim] * size[dim];
}


void shmhtFlush(H5Shm_t *shmInfo)
{
  if (DEBUG) printf("TODO  H5Chunks.c - shmhtFlus: Implement better eviction policy\n");
  if (DEBUG) printf("INFO  H5Chunks.c - shmhtFlus: Flushing hashtable\n");
  int initVal = 0;

  shmht_flush  (shmInfo->idIndex);
  shmht_insert (shmInfo->idIndex, FREE_SPACE_KEY, sizeof(FREE_SPACE_KEY), &initVal, sizeof(int));
}
void *allocChunk(H5Shm_t *H5Shm, hsize_t *chunk, int rank, hid_t memType)
{
  // Check if shm is full
  void * freeSpace = NULL;
  freeSpace = getFreeSpace(H5Shm, chunk, rank, memType);
  if (freeSpace != NULL) return freeSpace;
  if (DEBUG) printf("INFO  H5Chunks.c - allocChunk: Could not find free space for chunk\n"); 

  // TODO evict chunk
  shmhtFlush(H5Shm);
  return NULL;
}
