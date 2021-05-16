#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void printLista();

void MalfunctionManager()
{
    //create shared memory
    shmid = shmget(KEY, sizeof(SHARED_MEMORY), 0);

    //attach shared memory
    if ((SharedMemory = (SHARED_MEMORY *)shmat(shmid, NULL, 0)) == (SHARED_MEMORY *)-1)
    {
        perror("Shmat error!\n");
        exit(1);
    }

    // pid_t pid;
    printf("Gestor de Avarias Iniciado\n");
    printf("O meu é %d\n", getpid());
    printf("O meu pai é %d\n", getppid());
}