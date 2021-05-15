#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void MalfunctionManager()
{
    // pid_t pid;
    printf("Gestor de Avarias Iniciado\n");
    printf("O meu é %d\n", getpid());
    printf("O meu pai é %d\n", getppid());

    sleep(5); //simular codigo a correr
}