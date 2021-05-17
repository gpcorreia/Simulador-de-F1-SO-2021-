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

    //sleep(5); //simular codigo a correr

    while(1)
    {

        //sleep T_Avaria unidades de tempo, em segundos
        sleep(PitLane);

        //determina se ocorre uma avaria e notifica os carros atraves da message queue
        notificaCarros();

    }
}

//retorna 0 se nao tiver avaria e 1 se tiver
int gera_avarias(int reliability)
{
    int isAvaria = rand() % 100;
    if(isAvaria >= reliability)
    {
        return 1;
    }
    return 0;
}

int notificaCarros()
{
    int auxAvaria;
    message* msg;

    if (SharedMemory->NumTeams)
    {
        printf("Lista de equipas vazia\n");
    }

    for (int i=0; i<SharedMemory->NumTeams; i++)
    {
        for(int i=0; i<teams[i].Numcars; i++)
        {
            //gera a avaria (ou nao)
            auxAvaria = gera_avarias(teams[i].cars[i].reliability);
            msg->msgtype=teams[i].cars[i].model;
            msg->avaria=auxAvaria;
            
            //notifica o carro
            if(msgsnd(msqid, msg, sizeof(msg) - sizeof(long), 0) == -1)
            {
                perror("Error: msgsnd()\n");
                exit(1);
            }
            
        }
    }
    return 0;
}

