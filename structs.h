#ifndef STRUCTS_S
#define STRUCTS_S

#include "defines.h"
#include "const.h"

typedef struct Car
{ //"Thread" dos carros
    char team[MAX_CHAR_NAMES];
    int model;
    int state; //0-Corrida - 1-Segurança - 2-Box - 3-Desistencia - 4-Terminado
    int speed;
    int laps;
    float consumption;
    int reliability;
    int oilcap;
    int totalBox; //numero de entradas na box
} Car;

typedef struct indices
{
    int team;
    int car;
} inx;

typedef struct PitBox //Box das equipas
{
    int state; //0 - disponivel; 1 - ocupada; 2 - reservada;
    int car;
} box;

typedef struct Team
{
    char name[MAX_CHAR_NAMES];
    int Numcars;
    int FinishCars;
    Car cars[2]; //index
    box pitbox;
} Team;

typedef struct shared_memory
{
    int infoRace; // 1- if Started Race/resume     2 - if Pause
    int NumCars;
    int FinishCars;
    int desistencias;
    int NumTeams;
    int totalAbastecimentos;
    int totalAvarias;
    Team *teams; //lista ligada de equipas

} SHARED_MEMORY;

typedef struct mq_message
{
    long msgtype;
    int avaria; // 1 -> tem avaria; 0-> nao tem avaria
} message;

#endif
