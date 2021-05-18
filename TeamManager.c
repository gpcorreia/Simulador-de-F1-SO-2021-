#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void *Carro();
void leitura();

void TeamManager(int indice)
{
    // printf("A minha Equipa é %d\n", getpid());
    // printf("O meu pai é %d\n", getppid());

    //Criar threads dos carros

    inx aux;
    pthread_t tid[NumCars]; //thread id
    pthread_mutex_init(&mutex, NULL);
    aux.team = indice;

    for (int i = 0; i < EquipasSHM[aux.team].Numcars; i++)
    {
        aux.car = i;
        if (pthread_create(&tid[i], NULL, &Carro, (void *)&aux) != 0)
        {
            perror("Erro a criar thread.\n");
            exit(1);
        }
    }

    //sleep(5); //simular codigo a correr
    //leitura();
    // while (teamsAx.FinishCars != teamsAx.Numcars)
    // {
    //     if (teamsAx.cars[i])
    // }
    //Esperar que todas as threads terminem
    for (int i = 0; i < NumCars; i++)
    {
        if (pthread_join(tid[i], NULL) != 0)
        {
            perror("Erro a terminar thread\n");
            exit(1);
        }
    }
}

//Car thread
void *Carro(inx aux)
{
    // pthread_mutex_lock(&mutex);
    // printf("Carro da equipa %d\n", getpid());
    // pthread_mutex_unlock(&mutex);
    message my_msg;

    int TotalDistance = lap * dv;
    int combustivel = oilcap;
    int fourLaps = 4 * dv;
    int twoLaps = 2 * dv;
    printf("Carro : %d ----- Team %s\n", EquipasSHM[aux.team].cars[aux.car].model, EquipasSHM[aux.team].cars[aux.car].team);
    printf("O meu Gestor %d\n", getpid());

    while (TotalDistance > 0)
    {
        msgrcv(msqid, &my_msg, sizeof(my_msg) - sizeof(long), EquipasSHM[aux.team].cars[aux.car].model, IPC_NOWAIT);

        //printf("my_msg -> %ld, %d\n", my_msg.msgtype, my_msg.avaria);
        //printf("carro -> %d, avaria ? -> %d\n", EquipasSHM[aux.team].cars[aux.car].model, my_msg.avaria);
        if (my_msg.avaria == 1)
        {

            EquipasSHM[aux.team].cars[aux.car].state = 1;
        }

        else if (EquipasSHM[aux.team].cars[aux.car].consumption == 0)
        {
            sem_wait(mutex_sh);
            SharedMemory->desistencias++;
            EquipasSHM[aux.team].cars[aux.car].state = 3;
            sem_post(mutex_sh);
            printf("Car %d Team %s RUN OF GAS!", EquipasSHM[aux.team].cars[aux.car].model, EquipasSHM[aux.team].cars[aux.car].team);
            break;
        }

        else if (((combustivel - (fourLaps * EquipasSHM[aux.team].cars[aux.car].consumption) / EquipasSHM[aux.team].cars[aux.car].speed) <= 0) && EquipasSHM[aux.team].cars[aux.car].state == 0)
        {
            //começar a entrar na box
            if (EquipasSHM[aux.team].pitbox.state == 0)
            {
                EquipasSHM[aux.team].pitbox.car = EquipasSHM[aux.team].cars[aux.car].model;
                EquipasSHM[aux.team].pitbox.state = 2;
            }
            else if (EquipasSHM[aux.team].pitbox.state == 1)
            {
                //entrar na box
            }
        }

        else if (((combustivel - (twoLaps * EquipasSHM[aux.team].cars[aux.car].consumption) / EquipasSHM[aux.team].cars[aux.car].speed) <= 0) && EquipasSHM[aux.team].cars[aux.car].state == 0)
        {
            EquipasSHM[aux.team].cars[aux.car].state = 1;
        }

        if (EquipasSHM[aux.team].cars[aux.car].state == 1)
        {
            TotalDistance -= 0.3 * EquipasSHM[aux.team].cars[aux.car].speed;
            combustivel -= 0.4 * EquipasSHM[aux.team].cars[aux.car].consumption;
        }

        if (EquipasSHM[aux.team].cars[aux.car].state)
        {
            TotalDistance -= EquipasSHM[aux.team].cars[aux.car].speed;
            combustivel -= EquipasSHM[aux.team].cars[aux.car].consumption;
        }
        //printf("Total Avarias: %d\n", SharedMemory->totalAvarias);
        sleep(1);
    }

    if (SharedMemory->FinishCars == 0)
    {
        printf("Carro : %d ----- Team %s -> Ganhou a Corrida!!\n", EquipasSHM[aux.team].cars[aux.car].model, EquipasSHM[aux.team].cars[aux.car].team);
        SharedMemory->FinishCars++;
    }
    else
    {
        printf("Carro : %d ----- Team %s -> Terminou a Corrida!!\n", EquipasSHM[aux.team].cars[aux.car].model, EquipasSHM[aux.team].cars[aux.car].team);
        SharedMemory->FinishCars++;
    }

    pthread_exit(NULL);
    return NULL;
}

void leitura()
{
    char stateTM[MAX_CHAR_NAMES];
    sem_wait(mutex_up);
    read(p[0], &stateTM, sizeof(stateTM));
    sem_post(mutex_up);
}
