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

  shm           = allocChunk(H5Meta->data.chunk.size);
  hid_t dspace  = H5Dget_space(H5Meta->data.dataSetSpace);
  hid_t hyperid = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, H5Meta->data.chunk.size, NULL); 	// TODO: test for correctness
  hid_t mspace  = H5Screate_simple(rank, H5Meta->data.dimSize, H5Meta->data.maxDimSize);			// TODO: test for correctness
  herr_t status = H5Dread( H5Meta->data.dataSetSpace, H5Meta->mem.mem_type_id, H5Meta->mem.mem_space_id, H5Meta->file.file_space_id, H5Meta->prop.xfer_plist_id, shm );
 
  return shm;
}



void *getShmLoc(int *chunk)
{
  // TODO
  // check metadata 
  return NULL;
}



void getOffset(hsize_t offset[], hsize_t * size, int * chunk, int rank){
  for (int dim = 0; dim < rank; dim++)
    offset[dim] = chunk[dim] * size[dim];
}



void *allocChunk(hsize_t *size)
{
  // TODO
  // 1. check for empty chunks
  // 2. if empty chunk is there then claim it
  // 3. otherwise, evict chunk
  // 4. return the memaddress
  return NULL;
}
