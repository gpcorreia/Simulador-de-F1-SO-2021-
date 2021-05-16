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
void PrintStats();
void endRace();

void F1simulator() //gestor de corrida("RaceManager") + gestor de Avarias("...")
{

    init();

    printf("F1 Simulator %d \n", getpid());

    pid_t pid, pid2;

    signal(SIGTSTP, PrintStats);
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
            if (SharedMemory->infoRace != 0)
            {
                MalfunctionManager(); // Gestor de Avarias
                exit(0);
                break;
            }
        }
    }
    else
    {

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
    shmid = shmget(KEY, sizeof(SHARED_MEMORY), IPC_CREAT | 0700);

    //attach shared memory
    if ((SharedMemory = (SHARED_MEMORY *)shmat(shmid, NULL, 0)) == (SHARED_MEMORY *)-1)
    {
        perror("Shmat error!\n");
        exit(1);
    }

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
}

void endRace()
{
    closeSimulator();
    writeLog("SIMULATOR CLOSING\n");
}

//eliminar todos  os recursos requisitados pelos processos (semaforos, memorias partilhadas...)
void closeSimulator()
{
    //desattach da memória partilhada
    if (shmdt(SharedMemory) == -1)
    {
        perror("Erro shmdt.\n");
        exit(1);
    }

    //Destruição da memoria partilhada
    if (shmctl(shmid, IPC_RMID, NULL) != 0)
    {
        perror("Erro ao destruir a memoria partilhada.\n");
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
}
