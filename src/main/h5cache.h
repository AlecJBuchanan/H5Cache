#ifndef H5CACHE_H
#define H5CACHE_H

#include "hdf5.h"
#include <stdlib.h>
#include "globals.h"
#include "h5chunks.h"
#include "shmMain.h"

herr_t H5DreadNew( hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void * buf );
int H5Meta_init(H5Meta_t * H5Meta);

#endif
