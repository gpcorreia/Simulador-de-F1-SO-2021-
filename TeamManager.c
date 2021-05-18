#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void *Carro();
void leitura();
void FixCar(Car car);

void TeamManager(int indice)
{
    printf("A minha Equipa é %d\n", getpid());
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
    while (EquipasSHM[aux.team].FinishCars != EquipasSHM[aux.team].Numcars)
    {

        for (int i = 0; i < EquipasSHM[aux.team].Numcars; i++)
        {
            if (EquipasSHM[aux.team].pitbox.car == EquipasSHM[aux.team].cars[i].model)
            {
                EquipasSHM[aux.team].pitbox.state = 1;
                FixCar(EquipasSHM[aux.team].cars[i]);
                printf("Carro %d | Team %s a sair da pitbox!! ");
                EquipasSHM[aux.team].pitbox.state = 0;
            }
        }
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

void FixCar(Car car)
{
    int time = min + (random() % max);
    //random sleep;
    car.state = 0;
    car.totalBox++;
    car.oilcap = oilcap;
    sleep(time);
}

//Car thread
void *Carro(inx *aux)
{
    //printf("%d\n", aux->team);
    // pthread_mutex_lock(&mutex);
    // printf("Carro da equipa %d\n", getpid());
    // pthread_mutex_unlock(&mutex);
    message my_msg;

    int TotalDistance = lap * dv;
    int fourLaps = 4 * dv;
    int twoLaps = 2 * dv;
    EquipasSHM[aux->team].cars[aux->car].state = 0;
    printf("Carro : %d ----- Team %s\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
    printf("O meu Gestor %d\n", getpid());

    while (TotalDistance > 0)
    {
        if (EquipasSHM[aux->team].cars[aux->car].checkBox == 1)
        {
            continue;
        }
        else
        {
            msgrcv(msqid, &my_msg, sizeof(my_msg) - sizeof(long), EquipasSHM[aux->team].cars[aux->car].model, IPC_NOWAIT);

            //printf("my_msg -> %ld, %d\n", my_msg.msgtype, my_msg.avaria);
            //printf("carro -> %d, avaria ? -> %d\n", EquipasSHM[aux->team].cars[aux->car].model, my_msg.avaria);
            if (my_msg.avaria == 1)
            {
                printf("Avaria no Carro %d MODO SAFE ON!!\n", EquipasSHM[aux->team].cars[aux->car].model);
                EquipasSHM[aux->team].cars[aux->car].state = 1;
            }

            else if (EquipasSHM[aux->team].cars[aux->car].consumption == 0)
            {
                sem_wait(mutex_sh);
                SharedMemory->desistencias++;
                EquipasSHM[aux->team].cars[aux->car].state = 3;
                sem_post(mutex_sh);
                printf("Car %d Team %s RUN OF GAS!\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
                break;
            }

            else if (((EquipasSHM[aux->team].cars[aux->car].oilcap - (fourLaps * EquipasSHM[aux->team].cars[aux->car].consumption) / EquipasSHM[aux->team].cars[aux->car].speed) <= 0) && EquipasSHM[aux->team].cars[aux->car].state == 0)
            {
                //começar a entrar na box
                printf("Carro %d tentando entrar na box!!\n", EquipasSHM[aux->team].cars[aux->car].model);

                if (EquipasSHM[aux->team].pitbox.state == 0)
                {
                    printf("Carro %d entrou na box!!\n", EquipasSHM[aux->team].cars[aux->car].model);
                    EquipasSHM[aux->team].pitbox.car = EquipasSHM[aux->team].cars[aux->car].model;
                    EquipasSHM[aux->team].pitbox.state = 2;
                }
            }

            if (EquipasSHM[aux->team].cars[aux->car].state == 1)
            {
                TotalDistance -= 0.3 * EquipasSHM[aux->team].cars[aux->car].speed;
                EquipasSHM[aux->team].cars[aux->car].distance2finish = TotalDistance;
                EquipasSHM[aux->team].cars[aux->car].oilcap -= 0.4 * EquipasSHM[aux->team].cars[aux->car].consumption;
            }

            if (EquipasSHM[aux->team].cars[aux->car].state == 0)
            {
                TotalDistance -= EquipasSHM[aux->team].cars[aux->car].speed;
                EquipasSHM[aux->team].cars[aux->car].distance2finish = TotalDistance;
                EquipasSHM[aux->team].cars[aux->car].oilcap -= EquipasSHM[aux->team].cars[aux->car].consumption;
            }
        }

        if (SharedMemory->FinishCars == 0)
        {
            printf("Carro : %d ----- Team %s -> Ganhou a Corrida!!\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
            SharedMemory->FinishCars++;
        }
        else
        {
            printf("Carro : %d ----- Team %s -> Terminou a Corrida!!\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
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
