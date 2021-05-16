#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void *Carro();
void leitura();

void TeamManager(Team *teamsAx)
{
    // printf("A minha Equipa é %d\n", getpid());
    // printf("O meu pai é %d\n", getppid());

    //Criar threads dos carros
    pthread_t tid[NumCars]; //thread id
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < teamsAx->Numcars; i++)
    {
        if (pthread_create(&tid[i], NULL, &Carro, (void *)&teamsAx->cars[i]) != 0)
        {
            perror("Erro a criar thread.\n");
            exit(1);
        }
    }

    sleep(5); //simular codigo a correr
    // leitura();

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
    int TotalDistance = lap * dv;
    char message[1000];

    printf("Carro : %d ----- Team %s\n", car->model, car->team);
    printf("O meu Gestor %d\n", getpid());

    while (TotalDistance > 0)
    {
        printf("%d\n", TotalDistance);

        TotalDistance -= car->speed;
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
