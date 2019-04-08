#ifndef GLOBALS_H
#define GLOBALS_H

#include "hdf5.h"
#include <stdlib.h>

#define SHM_SIZE   134217728 // 128 MB
#define NUM_CHUNKS 1024

#define TRUE  1
#define FALSE 0

#define DEBUG 1

#define SUCCESS 0
#define FAILURE 1
#define CHUNK_NO_CHUNKS    0100
#define CHUNK_SIZE_FAILED  0101

#define DATA_SIZE_FAILED   0200
#define DATA_BOUNDS_FAILED 0201

#define SHM_FILE_NOT_FOUND 0300

#define PRINT_AND_QUIT(text, rc) do{ \
        printf(text);                 \
        return rc;                     \
      } while(0);


typedef struct H5Hyperslab_t{
  hsize_t *start;
  hsize_t *end;
}H5Hyperslab_t;

typedef struct H5Chunk_t{
  bool     chunked;
  hsize_t *size;
  int     *start;
  int     *end;
  int      nchunks;
  char    *chunks;
}H5Chunk_t;

typedef struct H5Data_t{
  hid_t dataset_id;
  hid_t dataSetSpace;
  hid_t dcpl_id;

  H5D_layout_t  layout;
  H5Chunk_t     chunk;
  H5Hyperslab_t hyperslab;

  int      dims;
  hsize_t *dimSize;
  hsize_t *maxDimSize;
}H5Data_t;


typedef struct H5Shm_t{
  void  *data;
//  void  *lruIndex;
//  void  *chunkList;
  void *idIndex;
  int   freeSpaceOffset;
}H5Shm_t;
typedef struct H5Mem_t{
  hid_t   mem_type_id;
  hid_t   mem_space_id;
  void    *buf;
  H5Shm_t shm; 
}H5Mem_t;



typedef struct H5File_t{
  hid_t file_space_id;
}H5File_t;


typedef struct H5Prop_t{
  hid_t xfer_plist_id;
}H5Prop_t;



typedef struct H5Meta_t{
  H5Data_t data;
  H5Prop_t prop;
  H5File_t file;
  H5Mem_t  mem;
}H5Meta_t;







#endif
