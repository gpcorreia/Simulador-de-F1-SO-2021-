#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void *Carro();
void leitura();

void TeamManager()
{
    // printf("A minha Equipa é %d\n", getpid());
    // printf("O meu pai é %d\n", getppid());

    //Criar threads dos carros
    pthread_t tid[NumCars]; //thread id
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NumCars; i++)
    {
        if (pthread_create(&tid[i], NULL, Carro, NULL) != 0)
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
void *Carro()
{
    // pthread_mutex_lock(&mutex);
    // printf("Carro da equipa %d\n", getpid());
    // pthread_mutex_unlock(&mutex);
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
