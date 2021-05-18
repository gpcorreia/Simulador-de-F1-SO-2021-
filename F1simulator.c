#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void init();
void ConfigRead();
void RaceManager();
void MalfunctionManager();
void writeLog(char *info);
void closeSimulator();
void closeSimulator();
void getTop5Cars(char *message);
void PrintStats();
void endRace();

void F1simulator() //gestor de corrida("RaceManager") + gestor de Avarias("...")
{

    init();

    printf("F1 Simulator %d \n", getpid());

    pid_t pid, pid2;

    signal(SIGINT, endRace);

    if ((pid = fork()) == 0)
    {
        RaceManager(); // Gestor de Corridas
        exit(0);
    }
    if ((pid2 = fork()) == 0)
    {
        while (1)
        {
            if (SharedMemory->infoRace == 1)
            {
                MalfunctionManager(); // Gestor de Avarias
                exit(0);
                break;
            }
        }
    }
    else
    {
        signal(SIGTSTP, PrintStats);
        printf("\nF1 Simulator %d \n", getpid());
    }

    for (int i = 0; i < 2; i++)
    {
        wait(NULL);
    }
}

void init()
{
    ConfigRead(); //ler ficheiro de config

    //create shared memory
    shmid = shmget(IPC_PRIVATE, sizeof(SHARED_MEMORY), IPC_CREAT | 0700);

    //attach shared memory
    if ((SharedMemory = (SHARED_MEMORY *)shmat(shmid, NULL, 0)) == (SHARED_MEMORY *)-1)
    {
        perror("Shmat error shmid!\n");
        exit(1);
    }

    shmidTeams = shmget(IPC_PRIVATE, sizeof(Team) * NumTeam, IPC_CREAT | 0700);
   
    //attach shared memory
    if ((SharedMemory->teams = (Team *)shmat(shmidTeams, NULL, 0)) == (Team *)-1)
    {
        perror("Shmat error shmidTeams!\n");
        exit(1);
    }
    //init of Shared Memory
    EquipasSHM = SharedMemory->teams;
    SharedMemory->infoRace = 0;
    SharedMemory->NumCars = 0;
    SharedMemory->FinishCars = 0;
    SharedMemory->NumTeams = 0;
    SharedMemory->desistencias = 0;
    SharedMemory->totalAvarias = 0;

    // criacao dos mutex / semaforos
    sem_unlink(MUTEX_SH); //mutex para semaforo
    mutex_sh = sem_open(MUTEX_SH, O_CREAT | O_EXCL, 0700, 1);
    if (mutex_sh == SEM_FAILED)
    {
        perror("Failure creating the semaphore MUTEX\n");
        exit(1);
    }

    sem_unlink(MUTEX_WR_LOG); //mutex para write log
    mutex_write_log = sem_open(MUTEX_WR_LOG, O_CREAT | O_EXCL, 0700, 1);
    if (mutex_write_log == SEM_FAILED)
    {
        perror("Failure creating the semaphore MUTEX\n");
        exit(1);
    }

    sem_unlink(MUTEX_UP); //mutex para write log
    mutex_up = sem_open(MUTEX_UP, O_CREAT | O_EXCL, 0700, 1);
    if (mutex_up == SEM_FAILED)
    {
        perror("Failure creating the semaphore MUTEX\n");
        exit(1);
    }

    if ((mkfifo(PIPE_NAME, O_CREAT | O_EXCL | 0600) < 0) && (errno != EEXIST)) //create named pipe
    {
        perror("Cannot open pipe: \n");
        exit(1);
    }

    if ((msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0777)) == -1)
    {
        perror("Error: msgget()\n");
        exit(1);
    }

    printf("----Program Started----\n");
    writeLog("SIMULATOR STARTING");
}

void ConfigRead()
{
    //leitura do ficheiro config.txt e organização dos dados coletados (variaveis globais)
    char config[MAX_LINES_CONFIG][MAX_COLUMS_CONFIG];
    FILE *fp;
    fp = fopen(CONFIG_FILE, "r");
    int i = 0;
    if (fp == NULL)
    {
        printf("Erro ao abrir o ficheiro de Configuração\n");
        exit(1);
    }

    while (!feof(fp))
    {
        fgets(config[i], MAX_COLUMS_CONFIG, fp);
        i++;
    }

    fclose(fp);

    ut = atoi(config[0]);
    dv = atoi(strtok(config[1], ", "));
    lap = atoi(strtok(NULL, ", "));
    NumTeam = atoi(config[3]);
    NumCars = atoi(config[4]);
    PitLane = atoi(config[5]);
    min = atoi(strtok(config[6], ", "));
    max = atoi(strtok(NULL, ", "));
    oilcap = atoi(config[7]);
    NumCarsRace = NumCars;
    NumCarsRace = NumCars * NumTeam;

    // printf("%d %d %d %d %d %d %d %d %d", ut, dv, lap, NumTeam, NumCars, PitLane, min, max, oilcap);
}

void PrintStats()
{
    printf("Escrevendo Estatisticas\n");

    //Top5 carros tendo em conta o numero de voltas completas; Carro em ultimo lugar
    char top5cars[1024];
    getTop5Cars(top5cars);

    //Carro em ultimo lugar

    //Total de avarias

    //Total de abastecimentos

    //Total de avarias

    //Numero de carros em pista   
}

void endRace()
{
    closeSimulator();
    writeLog("SIMULATOR CLOSING\n");
}

void getTop5Cars(char *message)
{
    //mete dentro do array todos os carros que participam na corrida
    Car allCars[NumTeam*NumCars];
    int t, i, j, k=0;
    Car aux;
    Car last = EquipasSHM[0].cars[0];
    char aux2[1024];
    int Maxdist = 0;

    for(i=0; i<SharedMemory->NumTeams; i++)
    {
        for (j = 0; j < EquipasSHM[i].Numcars; j++)
        {
            allCars[k] = EquipasSHM[i].cars[j];
            //printf("%d\n", EquipasSHM[i].cars[j].model);
            //printf("%d\n", array[k].model);
            if(EquipasSHM[i].cars[j].distance2finish > Maxdist)
            {
                Maxdist = EquipasSHM[i].cars[j].distance2finish;
                last = EquipasSHM[i].cars[j];
            }
            k++;
        }
    }

    // printf("k: %d\n", k);
    /* for(i=0; i<k; i++)
    {
        printf("%d %d\n", array[i].model, array[i].laps);
    }  */
    

    //ordenar e retirar os 5 primeiros
    for(i=0; i<k; i++)
    {
        for (j = 0; j < k; j++)
        {
            if (allCars[i].laps > allCars[j].laps)
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

    writeLog("TOP 5");

    for(int i=0; i<t; i++)
    {
        sprintf(aux2, "=>CAR: %d | TEAM: %s | LAPS: %d | BOX: %d", allCars[i].model, allCars[i].team, allCars[i].laps, allCars[i].totalBox);
        writeLog(aux2);
    } 

    writeLog("CARRO EM ULTIMO LUGAR");
    sprintf(aux2, "=>CAR: %d | TEAM: %s | LAPS: %d | BOX: %d", last.model, last.team, last.laps, last.totalBox);
    writeLog(aux2);

}

//eliminar todos  os recursos requisitados pelos processos (semaforos, memorias partilhadas...)
void closeSimulator()
{

    //desattach da memória partilhada
    if (shmdt(SharedMemory->teams) == -1)
    {
        perror("Erro shmdt.\n");
        exit(1);
    }

    //Destruição da memoria partilhada
    if (shmctl(shmidTeams, IPC_RMID, NULL) != 0)
    {
        perror("Erro ao destruir a memoria partilhadashmidTeams.\n");
        exit(1);
    }

    //desattach da memória partilhada
    if (shmdt(SharedMemory) == -1)
    {
        perror("Erro shmdt.\n");
        exit(1);
    }

    //Destruição da memoria partilhada
    if (shmctl(shmid, IPC_RMID, NULL) != 0)
    {
        perror("Erro ao destruir a memoria partilhada shmid.\n");
        exit(1);
    }

    //Destruição do (named) mutex usado para o controlo do acesso à memoria partilhada
    if (sem_close(mutex_sh) != 0)
    {
        perror("Erro sem_close(mutex_sh).\n");
        exit(1);
    }
    if (sem_unlink(MUTEX_SH))
    {
        perror("Erro ao apagar MUTEX_SH.\n");
        exit(1);
    }
    //Destruição do (named) mutex usado para o controlo da escrita no log.txt
    if (sem_close(mutex_write_log))
    {
        perror("Erro sem_close(mutex_write_log).\n");
        exit(1);
    }
    if (sem_unlink(MUTEX_WR_LOG))
    {
        perror("Erro ao apagar MUTEX_WR_LOG.\n");
        exit(1);
    }
    if (sem_close(mutex_up))
    {
        perror("Erro sem_close(mutex_up).\n");
        exit(1);
    }
    if (sem_unlink(MUTEX_UP))
    {
        perror("Erro ao apagar MUTEX_UP.\n");
        exit(1);
    }

    if (msgctl(msqid, IPC_RMID, 0) == -1)
    {
        perror("Error: msgctl()\n");
        exit(1);
    }

    kill(0, SIGKILL);
}