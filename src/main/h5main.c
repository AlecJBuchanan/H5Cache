#include "hdf5.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (argc != 3) return -1;
  char * fileName = argv[1];
  char * dsName   = argv[2];

  printf("Filename: %s\n", fileName);
  printf("Dataset:  %s\n", dsName);
  printf("Initialization\n");

  herr_t status;
  hid_t       datatype, dataspace;
  hid_t       memspace;
  H5T_class_t t_class;                 /* data type class */
  H5T_order_t order;                 /* data order */
  size_t      size;                  /*
                                        * size of the data element
                                        *                                         *                                         *                                       * stored in file
                                        *                                                                                 *                                                                                 *                                                                               */
  hsize_t     dimsm[3];              /* memory space dimensions */
  hsize_t     dims_out[3];           /* dataset dimensions */

  hsize_t      count[3];              /* size of the hyperslab in the file */
  hsize_t      offset[3];             /* hyperslab offset in the file */
  hsize_t      count_out[3];          /* size of the hyperslab in memory */
  hsize_t      offset_out[3];         /* hyperslab offset in memory */
  int          i, j, k, status_n, rank;
  hid_t        dcpl_id;
  hsize_t      chunk_size[3];

  printf("Opening file and dataset\n");

  hid_t file    = H5Fopen(fileName, H5F_ACC_RDONLY, H5P_DEFAULT);
  hid_t dataset = H5Dopen(file, dsName, H5P_DEFAULT);

  printf("Getting info\n");

  datatype = H5Dget_type(dataset);     /* datatype handle */
  t_class  = H5Tget_class(datatype);
  order    = H5Tget_order(datatype);
  size     = H5Tget_size(datatype);

  printf(" Data size is %d \n", (int)size);
  if (order == H5T_ORDER_LE) printf("Little endian order \n");
  if (t_class == H5T_INTEGER) printf("Data set has INTEGER type \n");

  dataspace = H5Dget_space(dataset);    /* dataspace handle */
  rank      = H5Sget_simple_extent_ndims(dataspace);
  status_n  = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
  printf("rank %d, dimensions %lu x %lu \n", rank, (unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]));
  dcpl_id   = H5Dget_create_plist(dataset);
  status    = H5Pget_chunk(dcpl_id, rank, chunk_size);
  int X, Y, Z;
  X = chunk_size[0];
  Y = chunk_size[1];
  Z = chunk_size[2];

  if (0){
    printf("Found chunk size: (%d, %d, %d)\n", X, Y, Z);
    return 1;
  }

  offset[0] = 0;
  offset[1] = 0;
  offset[2] = 0;
  count[0]  = X-1;
  count[1]  = Y-1;
  count[2]  = Z-1;
  status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);

  dimsm[0] = X;
  dimsm[1] = Y;
  dimsm[2] = Z;
  memspace = H5Screate_simple(3,dimsm,NULL);

  offset_out[0] = 0;
  offset_out[1] = 0;
  offset_out[2] = 0;
  count_out[0]  = X-1;
  count_out[1]  = Y-1;
  count_out[2]  = Z-1;

  status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL, count_out, NULL);
  printf("Reading\n");

  int *data_out = malloc(sizeof(int) * X * Y * Z);
  status        = H5DreadNew(dataset, H5T_STD_I32BE, memspace, dataspace, H5P_DEFAULT, data_out);
  printf("STATUS: %d\n", status);



  printf("Closing\n");
  H5Tclose(datatype);
  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(memspace);
  H5Fclose(file);

}
