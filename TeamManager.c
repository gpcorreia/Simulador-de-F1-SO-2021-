#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void *Carro();
void leitura();
void FixCar(Car *car);
void printLista();

void TeamManager(int indice)
{
    // printf("A minha Equipa é %d\n", getpid());
    // printf("O meu pai é %d\n", getppid());

    //Criar threads dos carros

    inx aux;
    pthread_t tid[NumCars]; //thread id
    pthread_mutex_init(&mutex, NULL);
    aux.team = indice;
    // printLista();
    for (int i = 0; i < EquipasSHM[aux.team].Numcars; i++)
    {
        aux.car = i;
        printf("aux.car = %d\n", aux.car);
        if (pthread_create(&tid[i], NULL, &Carro, &aux) != 0)
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
                sem_wait(mutex_sh);
                EquipasSHM[aux.team].pitbox.state = 1;
                FixCar(&EquipasSHM[aux.team].cars[i]);
                EquipasSHM[aux.team].pitbox.state = 0;
                EquipasSHM[aux.team].pitbox.car = 0;
                EquipasSHM[aux.team].cars[aux.car].checkMal = 0;
                EquipasSHM[aux.team].cars[aux.car].state = 0;
                EquipasSHM[aux.team].cars[aux.car].checkBox = 0;
                sem_post(mutex_sh);
                printf("Carro [%d] sai da box\n", EquipasSHM[aux.team].cars[i].model);
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

void FixCar(Car *car)
{
    int time;

    //abastecer
    if (car->checkMal == 1 || car->checkMal == 3)
    {
        //car->state = 0;
        time = 2 * ut;
        car->totalBox++;
        car->oilcap = oilcap;
        //car->checkMal = 0;
        SharedMemory->totalAbastecimentos++;
        sleep(time);
        printf("Carro [%d] abasteceu!\n", car->model);
    }

    //Avaria
    else if(car->checkMal == 4)
    {
        time = rand() % max + min;
        //car->state = 0;
        car->totalBox++;
        car->oilcap = oilcap;
        //car->checkMal = 0;
        SharedMemory->totalAvarias++;
        sleep(time);
        printf("Carro [%d] tratou de uma avaria!\n", car->model);
    }

    //Abastecer e tratar avaria
    else if (car->checkMal == 2)
    {
        time = rand() % max + min;
        //car->state = 0;
        car->totalBox++;
        car->oilcap = oilcap;
        //car->checkMal = 0;
        SharedMemory->totalAbastecimentos++;
        sleep(time + 2 * ut);
        printf("Carro [%d] abasteceu e tratou de uma avaria!\n", car->model);
    }
}

//Car thread
void *Carro(inx *aux)
{
    //printf("%d\n", aux->team);
    // pthread_mutex_lock(&mutex);
    // printf("Carro da equipa %d\n", getpid());
    // pthread_mutex_unlock(&mutex);
    message my_msg;
    printf("aux -> %d %d\n", aux->team, aux->car);

    //int TotalDistance = lap * dv;
    int lapsCompleted = 0;
    int distancePerLap = 0; 
    int fourLaps = 4 * dv;
    int twoLaps = 2 * dv;
    // printf("Carro : %d ----- Team %s\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
    // printf("O meu Gestor %d\n", getpid());

    while (lapsCompleted < lap)
    {
        //posicao 0 -> meta (onde se encontra a box)
    if(EquipasSHM[aux->team].cars[aux->car].checkBox == 0)
    {
        printf("Chegada a meta, %d volta(s) concluida(s)! -> Carro [%d]\n", lapsCompleted, EquipasSHM[aux->team].cars[aux->car].model);
        lapsCompleted++;
        distancePerLap = 0;

        //if checkbox == 0 tenta entra na box 
        //verificar se o carro precisa de ir a box
        if(EquipasSHM[aux->team].cars[aux->car].checkMal != 0)
        {
            //verifica se a box esta livre
            if(EquipasSHM[aux->team].pitbox.state == 0)
            {
                sem_wait(mutex_sh);
                EquipasSHM[aux->team].pitbox.car = EquipasSHM[aux->team].cars[aux->car].model;
                EquipasSHM[aux->team].cars[aux->car].state = 2;
                EquipasSHM[aux->team].pitbox.state = 1;
                EquipasSHM[aux->team].cars[aux->car].checkBox = 1;
                sem_post(mutex_sh);
                printf("Carro [%d] entrou na box!\n", EquipasSHM[aux->team].cars[aux->car].model);
            }

            //verifica se a box esta reservada e sou o carro q a reservou
            else if( (EquipasSHM[aux->team].pitbox.state == 2) && (EquipasSHM[aux->team].cars[aux->car].state == 5) )
            {
                sem_wait(mutex_sh);
                EquipasSHM[aux->team].pitbox.car = EquipasSHM[aux->team].cars[aux->car].model;
                EquipasSHM[aux->team].cars[aux->car].state = 2;
                EquipasSHM[aux->team].pitbox.state = 1;
                EquipasSHM[aux->team].cars[aux->car].checkBox = 1;
                sem_post(mutex_sh);
                printf("Carro [%d] entrou na box!\n", EquipasSHM[aux->team].cars[aux->car].model);
            }

            else
            {
                printf("Carro[%d] tenta entrar na box! Box ocupada!\n", EquipasSHM[aux->team].cars[aux->car].model);
            }

        }

        my_msg.avaria = 0;
        
        //corrida, 1 while = 1 volta
        while(distancePerLap < dv && EquipasSHM[aux->team].cars[aux->car].checkBox != 1)
        {

            //recebe mensagem da message queue, deteta se ha avarias ou nao
            //so aceita a mensagem se ainda nao tiver detetado uma avaria ou seja se my_msg.msgavaria = 0
            if(my_msg.avaria == 0)
            {

                msgrcv(msqid, &my_msg, sizeof(my_msg), EquipasSHM[aux->team].cars[aux->car].model, IPC_NOWAIT);

                //printf("my_msg -> %ld, %d\n", my_msg.msgtype, my_msg.avaria);

                //ativa modo de segurnça
                if(my_msg.avaria == 1)
                {
                    EquipasSHM[aux->team].cars[aux->car].state = 1;
                    EquipasSHM[aux->team].cars[aux->car].checkMal = 4;
                    SharedMemory->totalAvarias++;
                    printf("Carro [%d] HAD MALFUNCTION, MODE SAFE ON\n", EquipasSHM[aux->team].cars[aux->car].model);
                
                }
            }

            //corrida em estado de segurança
            if(EquipasSHM[aux->team].cars[aux->car].state == 1)
            {
                distancePerLap += 0.3 * EquipasSHM[aux->team].cars[aux->car].speed;
                EquipasSHM[aux->team].cars[aux->car].oilcap -= 0.4 * EquipasSHM[aux->team].cars[aux->car].consumption;
            }

            //corrida em estado normal
            if(EquipasSHM[aux->team].cars[aux->car].state == 0)
            {
                distancePerLap += EquipasSHM[aux->team].cars[aux->car].speed;
                EquipasSHM[aux->team].cars[aux->car].oilcap -= EquipasSHM[aux->team].cars[aux->car].consumption;
                
            }

            //vericar o combustivel
            if(EquipasSHM[aux->team].cars[aux->car].oilcap <= 0)
            {
                EquipasSHM[aux->team].cars[aux->car].state = 3;
                break;
            }

            //se nao tiver combustivel para 4 voltas tenta um lugar na box da equipa
            if( ( (fourLaps / EquipasSHM[aux->team].cars[aux->car].speed) * EquipasSHM[aux->team].cars[aux->car].consumption >= EquipasSHM[aux->team].cars[aux->car].oilcap) && (EquipasSHM[aux->team].cars[aux->car].state != 0) )
            {
                if(EquipasSHM[aux->team].cars[aux->car].checkMal == 4)
                {
                    EquipasSHM[aux->team].cars[aux->car].checkMal = 2;
                }
                else
                {
                    EquipasSHM[aux->team].cars[aux->car].checkMal = 3;

                }
                printf("Carro [%d] sem combustivel para 4 voltas, precisa de ir a box abastecer!\n", EquipasSHM[aux->team].cars[aux->car].model);
            }

            //se nao tiver combustivel para 2 voltas entra em modo de seguranca
            if(( (twoLaps / EquipasSHM[aux->team].cars[aux->car].speed) * EquipasSHM[aux->team].cars[aux->car].consumption >= EquipasSHM[aux->team].cars[aux->car].oilcap) /*&& (EquipasSHM[aux->team].cars[aux->car].state == 0)*/ )
            {
                EquipasSHM[aux->team].cars[aux->car].state = 1;
                if(EquipasSHM[aux->team].cars[aux->car].checkMal == 4)
                {
                    EquipasSHM[aux->team].cars[aux->car].checkMal = 2;
                }
                else
                {
                    EquipasSHM[aux->team].cars[aux->car].checkMal = 1;
                }
                printf("Carro [%d] sem combustivel para 2 voltas, MODO SAFE ON\n", EquipasSHM[aux->team].cars[aux->car].model);
            }
            
            sleep(ut);
            //printf("Carro [%d] percorre %d da %d volta!\n", EquipasSHM[aux->team].cars[aux->car].model, distancePerLap, lapsCompleted);
            //printf("Carro [%d] deposito -> %fS\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].oilcap);
        }
        
        printf("Chegada a meta, %d volta(s) concluida(s)! -> Carro [%d]\n", lapsCompleted, EquipasSHM[aux->team].cars[aux->car].model);
        

        if(EquipasSHM[aux->team].cars[aux->car].state == 3)
        {
            printf("Carro [%d] ficou sem combustivel!\n", EquipasSHM[aux->team].cars[aux->car].model);
            break;
        }

        }//fecha if

        else 
        {
            printf("sleep 2 (s)\n");
            sleep(2);    
        }

        //sleep(ut);

    }

    if (SharedMemory->FinishCars == 0 && EquipasSHM[aux->team].cars[aux->car].state != 3)
    {
        sem_wait(mutex_sh);
        SharedMemory->FinishCars++;
        sem_post(mutex_sh);
        printf("Carro : %d ----- Team %s -> Ganhou a Corrida!!\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
    }
    else if (SharedMemory->FinishCars != 0 && EquipasSHM[aux->team].cars[aux->car].state != 3)
    {
        sem_wait(mutex_sh);
        EquipasSHM[aux->team].cars[aux->car].state = 4;
        SharedMemory->FinishCars++;
        sem_post(mutex_sh);
        printf("Carro : %d ----- Team %s -> Terminou a Corrida!!\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
    } 
    else
    {
        sem_wait(mutex_sh);
        SharedMemory->desistencias++;
        sem_post(mutex_sh);
        printf("Carro : %d ----- Team %s -> Desistencia!!\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].team);
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
