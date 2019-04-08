#include "shmMain.h"

int openShm(char * name, int size, void * shm){
  int fd;
  fd = shm_open(name, O_RDWR, 0644);
  if (errno == ENOENT){
    if (DEBUG) printf("INFO  shmMain-openShm: Creating shared memory\n");
    fd = shm_open(name, O_CREAT, 0644);
    ftruncate(fd, size);
    shm = mmap(NULL, shm, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return SHM_FILE_NOT_FOUND;
  }
  else if (fd <= 0){
    // Failure
    perror("Failure 1\n");
    exit(-1);
  }
  if (DEBUG) printf("INFO  shmMain-openShm: Opening shared memory\n");
  shm = mmap(NULL, shm, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  // Check for failure
  if (shm == MAP_FAILED){
    perror("FAILURE 2");
    exit(-1);
  }
  return SUCCESS;  
}


unsigned int hashfunction(unsigned char *str)
{
    unsigned int hash = 0;
    int c;

    while (c = *str++)
      hash += c;

    return hash;
}
int key_eq_fn(void *a, void *b){
    return strcmp(a,b);
}
int initShm(H5Shm_t *shmInfo)
{
  // Initialization
  int fd;
  int rc = 0;
  rc = openShm(SHM_FILE,   SHM_SIZE, shmInfo->data);
  //rc = openShm(LRU_INDEX,  4096, shmInfo->lruIndex);
  //rc = openShm(CHUNK_LIST, 4096, shmInfo->chunkList); 

  shmInfo->idIndex = create_shmht(ID_INDEX, NUM_CHUNKS, 129, hashfunction, key_eq_fn);
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
