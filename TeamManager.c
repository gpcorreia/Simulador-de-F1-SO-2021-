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
        lapsCompleted++;
        distancePerLap = 0;

        //if checkbox == 0 tenta entra na box 
        //verificar se o carro precisa de ir a box
    if(EquipasSHM[aux->team].cars[aux->car].checkBox == 0)
    {
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
        while(distancePerLap < dv)
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
                    EquipasSHM[aux->team].cars[aux->car].checkMal = 2;
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
                EquipasSHM[aux->team].cars[aux->car].checkMal = 3;
                printf("Carro [%d] sem combustivel para 4 voltas, precisa de ir a box abastecer!\n", EquipasSHM[aux->team].cars[aux->car].model);
            }

            //se nao tiver combustivel para 2 voltas entra em modo de seguranca
            if(( (twoLaps / EquipasSHM[aux->team].cars[aux->car].speed) * EquipasSHM[aux->team].cars[aux->car].consumption >= EquipasSHM[aux->team].cars[aux->car].oilcap) /*&& (EquipasSHM[aux->team].cars[aux->car].state == 0)*/ )
            {
                EquipasSHM[aux->team].cars[aux->car].state = 1;
                EquipasSHM[aux->team].cars[aux->car].checkMal = 1;
                printf("Carro [%d] sem combustivel para 2 voltas, MODO SAFE ON\n", EquipasSHM[aux->team].cars[aux->car].model);
            }
            
            sleep(ut);
            printf("Carro [%d] percorre %d da %d volta!\n", EquipasSHM[aux->team].cars[aux->car].model, distancePerLap, lapsCompleted);
            printf("Carro [%d] deposito -> %fS\n", EquipasSHM[aux->team].cars[aux->car].model, EquipasSHM[aux->team].cars[aux->car].oilcap);
        }
        
        

        if(EquipasSHM[aux->team].cars[aux->car].state == 3)
        {
            printf("Carro [%d] ficou sem combustivel!\n", EquipasSHM[aux->team].cars[aux->car].model);
            break;
        }

        }//fecha if

        else 
        {
            printf("sleep 3 s\n");
            sleep(3);    
        }
        //sleep(ut);
        printf("Chegada a meta, %d volta(s) concluida(s)! -> Carro [%d]\n", lapsCompleted, EquipasSHM[aux->team].cars[aux->car].model);

       /*  msgrcv(msqid, &my_msg, sizeof(my_msg), EquipasSHM[aux->team].cars[aux->car].model, IPC_NOWAIT);

        // printf("my_msg -> %ld, %d\n", my_msg.msgtype, my_msg.avaria);
        //printf("carro -> %d, avaria ? -> %d\n", EquipasSHM[aux->team].cars[aux->car].model, my_msg.avaria);

        if ( (my_msg.avaria == 1) && (EquipasSHM[aux->team].cars[aux->car].checkMal != 2) ) //Recebe msg de avaria caso o carro esteja em estado diferente de avaria
        {
            printf("Car %d HAD MALFUNCTION, MODO SAFE ON!!\n", EquipasSHM[aux->team].cars[aux->car].model);
            EquipasSHM[aux->team].cars[aux->car].state = 1;
            EquipasSHM[aux->team].cars[aux->car].checkMal = 2;
            my_msg.avaria = 0;

            sem_wait(mutex_sh);
            SharedMemory->totalAvarias++,
            sem_post(mutex_sh);

        }

        if (EquipasSHM[aux->team].cars[aux->car].checkMal == 2 || EquipasSHM[aux->team].cars[aux->car].checkMal == 1) //tenta ir a box se precisar de abastecer e tiver uma avaria
        {
            if (EquipasSHM[aux->team].pitbox.state == 0)
            {
                //entrou na box
                printf(">>>> Carro %d entrou na box!!\n", EquipasSHM[aux->team].cars[aux->car].model);
                sem_wait(mutex_sh);
                EquipasSHM[aux->team].pitbox.car = EquipasSHM[aux->team].cars[aux->car].model;
                EquipasSHM[aux->team].pitbox.state = 1;
                sem_post(mutex_sh);
            }
            else if (EquipasSHM[aux->team].pitbox.state == 1)
            {
                printf("Carro %d à espera de entrar na box!! Com urgencia!!\n", EquipasSHM[aux->team].cars[aux->car].model);
            }
        }

        else if (EquipasSHM[aux->team].cars[aux->car].oilcap == 0) //acaba a corrida porque nao tem combustivel
        {
            printf("Car %d RUN OF GAS!! Abort Race!!\n", EquipasSHM[aux->team].cars[aux->car].model);
            sem_wait(mutex_sh);
            SharedMemory->desistencias++;
            EquipasSHM[aux->team].FinishCars++;
            EquipasSHM[aux->team].cars[aux->car].state = 3;
            sem_post(mutex_sh);
            break;
        }

        //se nao tiver combustivel suficiente para completar 4 voltas e estiver em modo de corrida começa a tentar ir a box
        else if ((EquipasSHM[aux->team].cars[aux->car].oilcap - (fourLaps * EquipasSHM[aux->team].cars[aux->car].consumption / EquipasSHM[aux->team].cars[aux->car].speed)) <= 0 && EquipasSHM[aux->team].cars[aux->car].state == 0)
        {
            //começar a entrar na box
            // printf("Carro %d tentando entrar na box!!\n", EquipasSHM[aux->team].cars[aux->car].model);
            if (EquipasSHM[aux->team].pitbox.state == 0)
            {
                //entrou na box
                printf(">>>> Carro %d entrou na box!!\n", EquipasSHM[aux->team].cars[aux->car].model);
                sem_wait(mutex_sh);
                EquipasSHM[aux->team].pitbox.car = EquipasSHM[aux->team].cars[aux->car].model;
                EquipasSHM[aux->team].pitbox.state = 1;
                sem_post(mutex_sh);
            }
            else if (EquipasSHM[aux->team].pitbox.state == 1)
            {
                printf("Carro %d à espera de entrar na box!!", EquipasSHM[aux->team].cars[aux->car].model);
            }
        } 

        //passa para modo segurança caso n tenha combustivel suficente para completar 2 voltas
        else if ((EquipasSHM[aux->team].cars[aux->car].oilcap - (twoLaps * EquipasSHM[aux->team].cars[aux->car].consumption / EquipasSHM[aux->team].cars[aux->car].speed)) <= 0 && EquipasSHM[aux->team].cars[aux->car].state == 0)
        {
            sem_wait(mutex_sh);
            EquipasSHM[aux->team].cars[aux->car].state = 1;
            EquipasSHM[aux->team].cars[aux->car].checkMal = 1;
            sem_post(mutex_sh);
        }

    
        if (EquipasSHM[aux->team].cars[aux->car].state == 1) // Decrementacao de variaveis consoante estado (Segunrança)
        {
            TotalDistance -= 0.3 * EquipasSHM[aux->team].cars[aux->car].speed;
            EquipasSHM[aux->team].cars[aux->car].distance2finish = TotalDistance;
            EquipasSHM[aux->team].cars[aux->car].oilcap -= 0.4 * EquipasSHM[aux->team].cars[aux->car].consumption;
        }

        else if (EquipasSHM[aux->team].cars[aux->car].state == 0) // Decrementacao de variaveis consoante estado (Corrida)
        {
            TotalDistance -= EquipasSHM[aux->team].cars[aux->car].speed;
            EquipasSHM[aux->team].cars[aux->car].distance2finish = TotalDistance;
            EquipasSHM[aux->team].cars[aux->car].oilcap -= EquipasSHM[aux->team].cars[aux->car].consumption;
        }

        printf("Sou o carro %d e percorri %d metros (fiz %d voltas)\n", SharedMemory->teams[aux->team].cars[aux->car].model, TotalDistance, SharedMemory->teams[aux->team].cars[aux->car].laps);
        sleep(1);
        */
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
