#include "shmMain.h"


void *initShm(H5Meta_t *H5Meta)
{
  // TODO
  // 1. check if metadata already exists
  //      if exists then return pointer to shm
  // 2. create shm
  //      base it off of the file name and dataset name
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
