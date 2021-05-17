#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void *Carro();
void leitura();

void TeamManager(Team teamsAx)
{
    // printf("A minha Equipa é %d\n", getpid());
    // printf("O meu pai é %d\n", getppid());

    //Criar threads dos carros
    pthread_t tid[NumCars]; //thread id
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < teamsAx.Numcars; i++)
    {
        if (pthread_create(&tid[i], NULL, &Carro, (void *)&teamsAx.cars[i]) != 0)
        {
            perror("Erro a criar thread.\n");
            exit(1);
        }
    }

    //sleep(5); //simular codigo a correr
    //leitura();
    while (teamsAx.FinishCars != teamsAx.Numcars)
    {
        if (teamsAx.cars[i])
    }
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
void *Carro(Car *car)
{
    // pthread_mutex_lock(&mutex);
    // printf("Carro da equipa %d\n", getpid());
    // pthread_mutex_unlock(&mutex);
    message my_msg;

    int TotalDistance = lap * dv;
    int combustivel = oilcap;
    char message[1000];
    int fourLaps = 4 * dv;
    int twoLaps = 2 * dv;
    int time;
    printf("Carro : %d ----- Team %s\n", car->model, car->team);
    printf("O meu Gestor %d\n", getpid());

    while (TotalDistance > 0)
    {
        printf("%d\n", TotalDistance);

        if (msgrcv(msqid, &my_msg, sizeof(my_msg) - sizeof(long), car->model, IPC_NOWAIT) == -1)
        {
            //perror("Error: msgrcv()\n");
            //exit(1);
        }
        //printf("my_msg -> %ld, %d\n", my_msg.msgtype, my_msg.avaria);
        //printf("carro -> %d, avaria ? -> %d\n", car->model, my_msg.avaria);
        if (my_msg->avaria == 1)
        {
            for (int i = 0; i < SharedMemory->NumTeams; i++)
            {
                for (int j = 0; j < EquipasSHM[i].Numcars; j++)
                {
                    if (strcmp(EquipasSHM[i].cars[j].model, car->model) == 0)
                    {
                        EquipasSHM[i].cars[j].state = 1;
                        car->state = 1;
                    }
                }
            }
        }
        else if (car->consumption == 0)
        {
            sem_wait(mutex_sh);
            SharedMemory->desistencias++;
            car->state = 3;
            sem_post(mutex_sh);
            printf("Car %d Team %s RUN OF GAS!", car->model, car->team);
            break;
        }
        else if (((combustivel - (fourLaps * car->consumption) / car->speed) <= 0) && car->state == 0)
        {
            //começar a entrar na box
            for (int i = 0; i < SharedMemory->NumTeams; i++)
            {
                if (strcmp(EquipasSHM[i].name, car->team) == 0)
                {
                    if (EquipasSHM[i].pitbox.state == 0)
                    {
                        EquipasSHM[i].pitbox.car = car->model;
                        EquipasSHM[i].pitbox.state = 2;
                    }
                    else if (EquipasSHM[i].pitbox.state == 1)
                    {
                        //entrar na box
                    }
                }
            }
        }
        else if (((combustivel - (twoLaps * car->consumption) / car->speed) <= 0) && car->state == 0)
        {
            for (int i = 0; i < SharedMemory->NumTeams; i++)
            {
                for (int j = 0; j < EquipasSHM[i].Numcars; j++)
                {
                    if (strcmp(EquipasSHM[i].cars[j].model, car->model) == 0)
                    {
                        EquipasSHM[i].cars[j].state = 1;
                        car->state = 1;
                    }
                }
            }
        }
        //AVaria = modo seguranca
        //Se tiver em modo segurança
        if (car->state == 1)
        {
            TotalDistance -= 0.3 * car->speed;
            combustivel -= 0.4 * car->consumption;
        }
        if (car->state == 0)
        {
            TotalDistance -= car->speed;
            combustivel -= car->consumption;
        }
        sleep(1);
    }

    if (SharedMemory->FinishCars == 0)
    {
        printf("Carro : %d ----- Team %s -> Ganhou a Corrida!!\n", car->model, car->team);
        SharedMemory->FinishCars++;
    }
    else
    {
        printf("Carro : %d ----- Team %s -> Terminou a Corrida!!\n", car->model, car->team);
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
