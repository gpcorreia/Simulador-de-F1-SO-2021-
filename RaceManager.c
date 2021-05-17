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
void createTM();

void RaceManager()
{
    // pid_t pid;
    printf("Gestor de Corrida Iniciado\n");
    printf("O meu é %d\n", getpid());
    printf("O meu pai é %d\n", getppid());

    int nread;
    char CommandsNP[1000];
    char infos[1024];
    pipe(p);

    if ((fdPipe = open(PIPE_NAME, O_RDONLY | O_NONBLOCK)) < 0)
    {
        perror("Cannot open Pipe:\n");
        exit(0);
    }
    while (1)
    {
        //verificaCorrida();
        // if (SharedMemory->FinishCars == NumCars)
        // {
        //     printf("Acabou a Corrida\n");
        //     break;
        // }
        if ((nread = read(fdPipe, &CommandsNP, sizeof(char) * 10000)) != 0) //named pipe
        {
            CommandsNP[nread] = '\0';

            if (strcmp(CommandsNP, "START RACE!\n") == 0 && SharedMemory->NumTeams == NumTeam)
            {
                SharedMemory->infoRace = 1;
                infos[0] = '\0';
                sprintf(infos, "NEW COMMAND RECEIVED: START RACE");
                writeLog(infos);
                createTM();
                // printLista();
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
                        printf("a entrar em insereCarro()\n");
                        insereCarro(team, carro, speed, consumption, reliability);
                        printf("inserido\n");
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
        // }s
    }

    for (int i = 0; i < NumTeam; i++)
    {
        wait(NULL);
    }
}

void createTM()
{
    for(int i=0; i<NumTeam; i++)
    {
        if(fork() == 0)
        {
            TeamManager(teams[i]);
            exit(0);
        }
    }
}

int insereCarro(char *team, int carro, int speed, float consumption, int reliability)
{
    printf("a entrar em checkTeam()\n");
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
            printf("a inserir\n");
            Team newTeam;

            strcpy(newTeam.name, team);
            strcpy(newTeam.cars[newTeam.Numcars].team, team);
            newTeam.cars[newTeam.Numcars].model = carro;
            newTeam.cars[newTeam.Numcars].speed = speed;
            newTeam.cars[newTeam.Numcars].consumption = consumption;
            newTeam.cars[newTeam.Numcars].reliability = reliability;
            newTeam.Numcars++;

            teams[SharedMemory->NumTeams] = newTeam;

            SharedMemory->NumTeams++;    
            //printf("acabei de inserir\n");        
        }
    }

    if (aux != 2)
    {
        printf("aqui\n");
        char info[1000];
        sprintf(info, "NEW CAR LOADED => TEAM: %s, CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d", team, carro, speed, consumption, reliability);
        printf("aqui2\n");
        //writeLog(info);
        printf("dps do aqui\n");
    }

    return 0;
}

void printLista()
{
    if(SharedMemory->NumTeams == 0)
    {
        printf("Lista de equipas vazia!\n");
    }

    for(int i=0; i<SharedMemory->NumTeams; i++)
    {
        printf("---------- TEAM %s ----------\n", teams[i].name);
        for (int i = 0; i < 2; i++)
        {
            printf("Carro[%d] = %d\n", i, teams[i].cars[i].model);
            printf("Carro[%d] = %.2f\n", i, teams[i].cars[i].consumption);
        }
    }

}

int checkCar(int car)
{
    if (SharedMemory->NumTeams == 0)
    {
        printf("Lista Vazia\n");
    }

    for(int i=0; i<SharedMemory->NumTeams; i++)
    {
        for (int i = 0; i < teams[i].Numcars; i++)
        {
            if (teams[i].cars[i].model == car)
            {
                return 0; //se o carro ja existir
            }
        }
    }

    return 1; //se o carro nao existir
}

int checkTeam(char *team, int carro, int speed, float consumption, int reliability)
{
    int k = 1;

    if (SharedMemory->NumTeams == 0)
    {
        return 1;
    }

    for (int i = 0 ; i<SharedMemory->NumTeams; i++)
    {
        if (strcmp(team, teams[i].name) == 0)
        {
            if (teams[i].Numcars < 2)
            {
                strcpy(teams[i].cars[teams[i].Numcars].team, team);
                teams[i].cars[teams[i].Numcars].model = carro;
                teams[i].cars[teams[i].Numcars].speed = speed;
                teams[i].cars[teams[i].Numcars].consumption = consumption;
                teams[i].cars[teams[i].Numcars].reliability = reliability;
                teams[i].Numcars++;
                k = 0;
                break;
            }
            else
            {
                printf("Equipa Cheia\n");
                k = 2;
                break;
            }
        }
    }

    return k;
}

int handleCommand(char *commands)
{
    regex_t regex;

    regcomp(&regex, "ADDCAR TEAM: [a-zA-Z], CAR: [0-9]+, SPEED: [+-]?([0-9]*[.])?[0-9]+, CONSUMPTION: [+-]?([0-9]*[.])?[0-9]+, RELIABILITY: [0-9]+", REG_ICASE | REG_EXTENDED);

    return regexec(&regex, commands, 0, NULL, 0);
}