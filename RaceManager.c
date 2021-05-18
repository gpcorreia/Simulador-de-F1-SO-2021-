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
int getTop5Cars(Car array[]);

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
        perror("Cannot open Pipe!\n");
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
                // printLista();
                createTM();
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
                    if (checkCar(carro) == 0)
                    {
                        writeLog("Car Already exist!");
                    }
                    else
                    {
                        insereCarro(team, carro, speed, consumption, reliability);
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

void createTM()
{
    for (int i = 0; i < SharedMemory->NumTeams; i++)
    {
        if (fork() == 0)
        {
            TeamManager(i);
            exit(0);
        }
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
            strcpy(EquipasSHM[SharedMemory->NumTeams].name, team);
            strcpy(EquipasSHM[SharedMemory->NumTeams].cars[0].team, team);
            EquipasSHM[SharedMemory->NumTeams].cars[0].model = carro;
            EquipasSHM[SharedMemory->NumTeams].cars[0].speed = speed;
            EquipasSHM[SharedMemory->NumTeams].cars[0].consumption = consumption;
            EquipasSHM[SharedMemory->NumTeams].cars[0].reliability = reliability;
            EquipasSHM[SharedMemory->NumTeams].cars[0].laps = 0;
            EquipasSHM[SharedMemory->NumTeams].cars[0].state = 0;
            EquipasSHM[SharedMemory->NumTeams].Numcars = 1;
            SharedMemory->NumTeams++;
        }
    }
    if (aux != 2)
    {
        char info[1000];
        sprintf(info, "NEW CAR LOADED => TEAM: %s, CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d", team, carro, speed, consumption, reliability);
        writeLog(info);
    }

    return 0;
}

void printLista()
{

    for (int i = 0; i < SharedMemory->NumTeams; i++)
    {
        printf("---------- TEAM %s ----------\n", EquipasSHM[i].name);
        for (int j = 0; j < EquipasSHM[i].Numcars; j++)
        {
            printf("Carro[%d] = %d\n", j, EquipasSHM[i].cars[j].model);
        }
    }
}

int checkCar(int car)
{
    for (int i = 0; i < SharedMemory->NumTeams; i++)
    {
        for (int j = 0; j < EquipasSHM[i].Numcars; j++)
        {
            if (EquipasSHM[i].cars[j].model == car)
            {
                return 0; //se o carro ja existir
            }
        }
    }

    return 1; //se o carro nao existir
}

int checkTeam(char *team, int carro, int speed, float consumption, int reliability)
{
    int j = 1;
    if (SharedMemory->NumTeams == 0)
    {
        return 1;
    }

    for (int i = 0; i < SharedMemory->NumTeams; i++)
    {
        if (strcmp(team, EquipasSHM[i].name) == 0)
        {
            if (EquipasSHM[i].Numcars < 2)
            {
                strcpy(EquipasSHM[i].cars[EquipasSHM[i].Numcars].team, team);
                EquipasSHM[i].cars[EquipasSHM[i].Numcars].model = carro;
                EquipasSHM[i].cars[EquipasSHM[i].Numcars].speed = speed;
                EquipasSHM[i].cars[EquipasSHM[i].Numcars].consumption = consumption;
                EquipasSHM[i].cars[EquipasSHM[i].Numcars].reliability = reliability;
                EquipasSHM[i].cars[EquipasSHM[i].Numcars].laps = 0;
                EquipasSHM[i].cars[EquipasSHM[i].Numcars].state = 0;
                EquipasSHM[i].Numcars++;
                j = 0;
                break;
            }
            else
            {
                printf("Equipa Cheia\n");
                j = 2;
                break;
            }
        }
    }

    return j;
}

//retorna o numero de 
int getTop5Cars(Car array[])
{
    //mete dentro do array todos os carros que participam na corrida
    Car allCars[NumTeam*NumCars];
    int t, i, j, k=0;
    for(i=0; i<SharedMemory->NumTeams; i++)
    {
        for(j=0; j<EquipasSHM[i].Numcars; j++)
        {
            allCars[k] = EquipasSHM[i].cars[j];
            //printf("%d\n", EquipasSHM[i].cars[j].model);
            //printf("%d\n", array[k].model);
            k++;
        }
    }

    // printf("k: %d\n", k);
    /* for(i=0; i<k; i++)
    {
        printf("%d %d\n", array[i].model, array[i].laps);
    }  */
    
    //ordenar e retirar os 5 primeiros

    Car aux;
    for(i=0; i<k; i++)
    {
        for(j=0; j<k; j++)
        {
            if(allCars[i].model > allCars[j].model)
            {
                aux = allCars[j];
                allCars[j] = allCars[i];
                allCars[i] = aux;
            }
        }
    }

    if (k < TOP)
    {
        t = k;
    }
    else
    {
        t = TOP;
    }

    for (i=0; i<t; i++)
    {
        array[i] = allCars[i];
    }
    return t;
}

int handleCommand(char *commands)
{
    regex_t regex;

    regcomp(&regex, "ADDCAR TEAM: [a-zA-Z], CAR: [0-9]+, SPEED: [+-]?([0-9]*[.])?[0-9]+, CONSUMPTION: [+-]?([0-9]*[.])?[0-9]+, RELIABILITY: [0-9]+", REG_ICASE | REG_EXTENDED);

    return regexec(&regex, commands, 0, NULL, 0);
}