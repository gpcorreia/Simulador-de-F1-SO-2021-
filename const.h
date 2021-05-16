#ifndef CONSTS_H
#define CONSTS_H

#include "structs.h"
#include "includes.h"

double count_time; //conta tempo de agora

int ut;      // unidade de tempo (ms)
int dv;      // distancia por volta(m)
int lap;     // numero de voltas
int NumTeam; // numero de equipas
int NumCars; // numero maximo de carros por equipa
int PitLane; // Número de unidades de tempo entre novo cálculo de avaria (T_Avaria)
int min;     // tempo minimo de reparacao
int max;     // tempo maximo de reparacao
int oilcap;  // Capacidade do depósito de combustível (em litros)
int NumCarsRace;

int fdPipe; // file descriptor do named pipe
int p[2];   // file descriptor do unnamed pipe

int msqid; //message queue id

int shmid;                   //shared memory id
SHARED_MEMORY *SharedMemory; //shared memory

sem_t *mutex_write_log; //mutex write in log/cmd
sem_t *mutex_up;        //mutex write in unnamed pipe
sem_t *mutex_sh;        //mutex for shared memory

pthread_mutex_t mutex; //Mutex para threads

#endif