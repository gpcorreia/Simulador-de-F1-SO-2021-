#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void TeamManager();
int handleCommand(char *commands);
int insereCarro(char *team, int carro, int speed, float consumption, int reliability);
int checkTeam(char *team, int carro, int speed, float consumption, int reliability);
int handleCommand(char *commands);
int checkCar(int car);
void printLista();
void writeLog(char *info);

void RaceManager()
{
    // pid_t pid;
    printf("Gestor de Corrida Iniciado\n");
    printf("O meu é %d\n", getpid());
    printf("O meu pai é %d\n", getppid());

    pid_t myid = getpid();
    int nread;
    char CommandsNP[1000];
    char infos[1024];
    pipe(p);

    for (int i = 0; i < NumTeam; i++)
    {
        if (getpid() == myid)
        {
            if (fork() == 0)
            {
                TeamManager();
                sleep(5);
                exit(0);
            }
        }
    }

    if ((fdPipe = open(PIPE_NAME, O_RDONLY | O_NONBLOCK)) < 0)
    {
        perror("Cannot open Pipe:\n");
        exit(0);
    }

    while (1)
    {
        if ((nread = read(fdPipe, &CommandsNP, sizeof(char) * 10000)) != 0) //named pipe
        {
            CommandsNP[nread] = '\0';

            if (strcmp(CommandsNP, "START RACE!\n") == 0 && SharedMemory->NumTeams == NumTeam)
            {
                SharedMemory->infoRace = 1;
                infos[0] = '\0';
                sprintf(infos, "NEW COMMAND RECEIVED: START RACE");
                writeLog(infos);
                printLista();
            }
            else if (strcmp(CommandsNP, "START RACE!\n") == 0 && SharedMemory->NumTeams < NumTeam)
            {
                infos[0] = '\0';
                sprintf(infos, "CANNOT START, NOT ENOUGH TEAMS");
                writeLog(infos);
            }

            else if (handleCommand(CommandsNP) == 0)
            {
                if (SharedMemory->infoRace != 1)
                {
                    char team[2];
                    int speed, reliability, carro;
                    float consumption;
                    sscanf(CommandsNP, "ADDCAR TEAM: %c, CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d", team, &carro, &speed, &consumption, &reliability);
                    if (checkCar(carro) != 0)
                    {
                        insereCarro(team, carro, speed, consumption, reliability);
                    }
                    else
                    {
                        writeLog("Car Already exist!");
                    }
                }
                else
                {
                    writeLog("Race Already Started!");
                }
            }
            else
            {
                infos[0] = '\0';
                sprintf(infos, "WRONG COMMAND => %s", CommandsNP);
                writeLog(infos);
            }
        }
        // if (write(p[1], &ola, sizeof(ola)) != 0)
        // {
        //     printf("%s\n", stateTM);
        // }
    }

    for (int i = 0; i < NumTeam; i++)
    {
        wait(NULL);
    }
}

int insereCarro(char *team, int carro, int speed, float consumption, int reliability)
{
    int aux = checkTeam(team, carro, speed, consumption, reliability);

    if (aux == 1) // nao existe equipa
    {

        if (SharedMemory->NumTeams >= NumTeam)
        {
            printf("Impossivel adicionar mais equipas\n");
            return 1;
        }
        else
        {

            Team *novoNo;

            novoNo = (Team *)malloc(sizeof(Team));

            if (novoNo == NULL)
            {
                printf("Memoria Cheia !\n");
                return 1;
            }
            novoNo->next = NULL;
            novoNo->Numcars = 0;

            strcpy(novoNo->name, team);
            strcpy(novoNo->cars[novoNo->Numcars].team, team);
            novoNo->cars[novoNo->Numcars].model = carro;
            novoNo->cars[novoNo->Numcars].speed = speed;
            novoNo->cars[novoNo->Numcars].consumption = consumption;
            novoNo->cars[novoNo->Numcars].reliability = reliability;
            novoNo->Numcars++;
            novoNo->next = SharedMemory->teams;
            SharedMemory->teams = novoNo;
            SharedMemory->NumTeams++;
        }
    }

    if (aux != 2)
    {
        char info[1000];
        sprintf(info, "NEW LOADED => TEAM: %s, CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d", team, carro, speed, consumption, reliability);
        writeLog(info);
    }

    return 0;
}

void printLista()
{
    Team *proximo = SharedMemory->teams;

    if (proximo == NULL)
    {
        printf("Lista Vazia\n");
    }

    while (proximo != NULL)
    {
        printf("---------- TEAM %s ----------\n", proximo->name);
        for (int i = 0; i < 2; i++)
        {
            printf("Carro[%d] = %d\n", i, proximo->cars[i].model);
            printf("Carro[%d] = %.2f\n", i, proximo->cars[i].consumption);
        }
        proximo = proximo->next;
    }
}

int checkCar(int car)
{
    Team *proximo = SharedMemory->teams;

    if (proximo == NULL)
    {
        printf("Lista Vazia\n");
    }

    while (proximo != NULL)
    {
        for (int i = 0; i < proximo->Numcars; i++)
        {
            if (proximo->cars[i].model == car)
            {
                return 0; //se o carro ja existir
            }
        }
        proximo = proximo->next;
    }

    return 1; //se o carro nao existir
}

int checkTeam(char *team, int carro, int speed, float consumption, int reliability)
{

    Team *proximo = SharedMemory->teams;
    int i = 1;

    if (proximo == NULL)
    {
        return 1;
    }

    while (proximo != NULL)
    {
        if (strcmp(team, proximo->name) == 0)
        {
            if (proximo->Numcars < 2)
            {
                strcpy(proximo->cars[proximo->Numcars].team, team);
                proximo->cars[proximo->Numcars].model = carro;
                proximo->cars[proximo->Numcars].speed = speed;
                proximo->cars[proximo->Numcars].consumption = consumption;
                proximo->cars[proximo->Numcars].reliability = reliability;
                proximo->Numcars++;
                i = 0;
                break;
            }
            else
            {
                printf("Equipa Cheia\n");
                i = 2;
                break;
            }
        }

        proximo = proximo->next;
    }

    return i;
}

int handleCommand(char *commands)
{
    regex_t regex;

    regcomp(&regex, "ADDCAR TEAM: [a-zA-Z], CAR: [0-9]+, SPEED: [+-]?([0-9]*[.])?[0-9]+, CONSUMPTION: [+-]?([0-9]*[.])?[0-9]+, RELIABILITY: [0-9]+", REG_ICASE | REG_EXTENDED);

    return regexec(&regex, commands, 0, NULL, 0);
}