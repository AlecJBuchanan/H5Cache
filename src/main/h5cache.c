//#include "h5cache.h"
#include "h5cache.h"

herr_t H5DreadNew( hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void * buf )
{
  //
  //  Initialization
  //
  if (DEBUG) printf("INFO  H5Cache.c - H5Dread: Starting H5Dread\n");
  int rc = SUCCESS;
  H5Meta_t H5Meta; 
  H5Meta.data.dataset_id    = dataset_id;
  H5Meta.prop.xfer_plist_id = xfer_plist_id;
  H5Meta.file.file_space_id = file_space_id;
  H5Meta.mem.mem_type_id    = mem_type_id;
  H5Meta.mem.mem_space_id   = mem_space_id;
  H5Meta.mem.buf            = buf;
  rc = H5Meta_init(&H5Meta);
  if (rc != SUCCESS) PRINT_AND_QUIT("ERROR H5Cache.c - H5Dread: Failed to get H5 layout\n",rc);

  //
  // Chunk Evalutaion
  //
  if (DEBUG) printf("INFO  H5Cache.c - H5Dread: Initializing chunk info\n");
  rc = initChunkInfo(&H5Meta.data);
  if (rc != SUCCESS) PRINT_AND_QUIT("ERROR H5Cache.c - H5Dread: Failed to get chunk information\n",rc);
  if (!H5Meta.data.chunk.chunked) return H5Dread(dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id, buf);
  // TODO make above statement compatible with ld preload

  if (DEBUG) printf("INFO  H5Cache.c - H5Dread: Initializing shared memory and selecting bounds\n");
  initShm(&H5Meta.mem.shm);

  rc = H5Sget_select_bounds(file_space_id, H5Meta.data.hyperslab.start, H5Meta.data.hyperslab.end);
  if (rc < 0) PRINT_AND_QUIT("ERROR H5Cache.c - H5Dread: Failed to get hyperslab bounds\n", DATA_BOUNDS_FAILED);


  if (DEBUG) printf("INFO  H5Cache.c - H5Dread: Getting chunk IDs\n");
  getReadBounds(&H5Meta.data); 
  printf("chunk start: %d\n", H5Meta.data.chunk.start[0]);
  printf("chunk end: %d\n", H5Meta.data.chunk.end[0]);

  //getChunkIDs(&H5Meta.data);
  readChunks(&H5Meta); 
  printf("\n");
  // TODO read chunks and re evaluate readChunk func
  // TODO test getchunkIDS

  return SUCCESS;
}

int H5Meta_init(H5Meta_t * H5Meta)
{
  H5Meta->data.dataSetSpace = H5Dget_space(H5Meta->data.dataset_id);			// TODO: check for failure
  H5Meta->data.dims         = H5Sget_simple_extent_ndims(H5Meta->data.dataSetSpace);	// TODO: check for failure
  H5Meta->data.dcpl_id      = H5Dget_create_plist(H5Meta->data.dataset_id);		// TODO: check for failure
  H5Meta->data.layout       = H5Pget_layout(H5Meta->data.dcpl_id);			// TODO: check for failure

  H5Meta->data.chunk.size = malloc(sizeof(hsize_t) * H5Meta->data.dims);
  H5Meta->data.dimSize    = malloc(sizeof(hsize_t) * H5Meta->data.dims);
  H5Meta->data.maxDimSize = malloc(sizeof(hsize_t) * H5Meta->data.dims);

  H5Meta->data.hyperslab.start = malloc(sizeof(hsize_t) * H5Meta->data.dims);
  H5Meta->data.hyperslab.end   = malloc(sizeof(hsize_t) * H5Meta->data.dims);  

  H5Meta->data.chunk.start = malloc(sizeof(hsize_t) * H5Meta->data.dims);
  H5Meta->data.chunk.end   = malloc(sizeof(hsize_t) * H5Meta->data.dims);

  return SUCCESS;
}
