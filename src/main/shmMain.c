#include "shmMain.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int openShm(char * name, int size, void ** shm, int flag){
  int fd;
  int status;
  
  if (DEBUG) printf("INFO  shmMain-openShm: Opening shared memory\n");
  fd = shm_open(name, flag, 0644);

  if (DEBUG) printf("INFO  shmMain-openShm: Truncating shared memory\n");
  status = ftruncate(fd, size);
  if (flag == O_CREAT)return SUCCESS;


  if (status == -1){
    printf("ERROR shmMain-openShm: ftruncate failed\n");
  }
  if (fd <= 0){
    // Failure
    perror("Failure 1\n");
    exit(-1);
  }


  if (DEBUG) printf("INFO  shmMain-openShm: Mapping shm file to memory\n");
  *shm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  // Check for failure
  if (*shm == MAP_FAILED){
    perror("FAILURE 2");
    exit(-1);
  }
  return SUCCESS;  
}


unsigned int hashfunction(void *str)
{
    unsigned int hash = 0;
    int c;

    while (c = *(unsigned char*)str++)
      hash += c;

    return hash;
}
int key_eq_fn(void *a, void *b){
    return strcmp(a,b);
}




int initShm(H5Shm_t *shmInfo)
{
  // Initialization
  int retVal  = 1;
  int initVal = 0; //CHUNK_ID_LEN + 1; // TODO: set to zero
  int rc      = 0;
  int fd;
  size_t retSize;


  if (DEBUG) printf("INFO  shmMain-initShm: Initializing shm for chunks\n");
  rc = openShm(SHM_FILE,   SHM_SIZE, &shmInfo->data, O_CREAT |O_RDWR);

  if (DEBUG) printf("INFO  shmMain-initShm: Initializing shm for hash table\n");
  rc = openShm(ID_INDEX,  32768, &(shmInfo->idIndex), O_CREAT |O_RDWR); // 32MB
  
  if (DEBUG) printf("INFO  shmMain-initShm: Creating hash table\n");
  shmInfo->idIndex = create_shmht("/dev/shm/h5cacheIdIndex", NUM_CHUNKS, CHUNK_ID_LEN, hashfunction, key_eq_fn); 
printf("hashtable at %p\n", shmInfo->idIndex); 

  if (DEBUG) printf("INFO  shmMain-initShm: Verifying freespace offset\n");
  void * status    = shmht_search (shmInfo->idIndex, FREE_SPACE_KEY, sizeof(FREE_SPACE_KEY), &retSize);
printf("verifying\n");

  if (status == NULL) retVal = shmht_insert (shmInfo->idIndex, FREE_SPACE_KEY, sizeof(FREE_SPACE_KEY), &initVal, sizeof(int));
  if (retVal < 1) printf("ERROR shmMain-initShm: Failed to insert initial values into shmht\n");
  if (DEBUG) printf("INFO  shmMain-initShm: Done initializing shm\n");

  //if (*(char *)shmInfo->data != 0) memset(shmInfo->data, 0, sizeof(char) * CHUNK_ID_LEN); // TODO: remove line
}




int destroySem(char * filename)
{
  return sem_unlink(filename);
  /*sem_t *sem;
  if ((sem = sem_open(filename, O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("Fail to initialize semaphore");
    exit(-1);
  }
  return sem_destroy(sem);*/
}

// TODO: the fd arguement is kind of useless
// TODO: creaete structure for this data
void *shmalloc(char *shm_file, int *fd, int chunkSize)
{ 
  if (DEBUG) printf("INFO  shmMain-shmalloc: Initializing\n");
  // Variable Initialization 
  void * buf;
  
  if (DEBUG) printf("INFO  shmMain-shmalloc: Opening shared memory file\n");
  // Open or create shared memory file
  *fd = shm_open(shm_file, O_RDWR | O_CREAT, 0644);
  if (*fd <= 0){
    perror("Failure 1\n");
    exit(-1);
  }

  if (DEBUG) printf("INFO  shmMain-shmalloc: Creating buffer for shared memory\n");
  // Set file size and create buffer
  ftruncate(*fd, chunkSize);
  buf = mmap(NULL, chunkSize, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
  if (buf == MAP_FAILED){
    perror("FAILURE 2");
    exit(-1);
  }

  return buf;
}

void  shmrelease(void * buf, int chunkSize)
{
  if (munmap(buf, chunkSize) == -1) {
    perror("Fail to unmap\n");
    exit(-1);
  }
}
void  shmdestroy(char *shm_file)
{
  if (shm_unlink(shm_file) != 0) {
    perror("Fail to unlink\n");
    exit(-1);
  }
}
