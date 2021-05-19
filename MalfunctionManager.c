#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void printLista();
int gera_avarias(int reliability);
int notificaCarros();

void MalfunctionManager()
{
    // pid_t pid;
    printf("Gestor de Avarias Iniciado\n");
    printf("O meu é %d\n", getpid());
    printf("O meu pai é %d\n", getppid());
    //sleep(5); //simular codigo a correr
    while (1)
    {
        //sleep T_Avaria unidades de tempo, em segundos
        //printf("Avarias a surgir em %d s\n", PitLane);
        sleep(PitLane);
        //printf("avarias a ocorrer\n");

        //determina se ocorre uma avaria e notifica os carros atraves da message queue
        notificaCarros();
    }
}

//retorna 0 se nao tiver avaria e 1 se tiver
int gera_avarias(int reliability)
{
    int isAvaria;
    isAvaria = rand() % 100;
    if (isAvaria >= reliability)
    {
        return 1;
    }
    return 0;
}

int notificaCarros()
{
    int auxAvaria;
    message msg;

    if (SharedMemory->NumTeams == 0)
    {
        printf("Lista de equipas Vazia\n");
    }

    for (int i = 0; i < SharedMemory->NumTeams; i++)
    {
        for (int j = 0; j < EquipasSHM[i].Numcars; j++)
        {
            //gera a avaria (ou nao)
            auxAvaria = gera_avarias(EquipasSHM[i].cars[j].reliability);
            msg.msgtype = EquipasSHM[i].cars[j].model;
            msg.avaria = auxAvaria;

            if (auxAvaria == 1)
            {
                sem_wait(mutex_sh);
                SharedMemory->totalAvarias++,
                sem_post(mutex_sh);
            }
            
            //notifica o carro
            if (msgsnd(msqid, &msg, sizeof(msg), 0) == -1)
            {
                perror("Error: msgsnd()\n");
                exit(1);
            }
            printf("Enviei para mq -> carro: %ld, avaria: %d\n", msg.msgtype, msg.avaria);
        }
    }
    return 0;
}