#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void writeTime(int flag, char *frase, FILE *f)
{
    printf("Na funcao writeTime()\n");
    time_t t = time(NULL);
    time_t segundos;
    time(&segundos);
    printf("1\n");

    struct tm *info;

    info = localtime(&segundos);
    printf("1.1\n");
    char buffer[200];
    info = localtime(&t);
    printf("2\n");

    strftime(buffer, 30, "%H:%M:%S ", info);
    printf("strftime()  \n");

    strcat(buffer, frase);
    printf("strcat()\n");

    if (flag == 1) //se flag for igual a 1 escreve na consola
    {
        printf("%d/%d/%d %s\n", info->tm_mday, info->tm_mon, info->tm_year + 1900, buffer);
    }
    else if (flag == 0)
    {
        fprintf(f, "%d/%d/%d %s\n", info->tm_mday, info->tm_mon, info->tm_year + 1900, buffer); // se for igual a 0 escreve no ficheiro

        printf("escrevi algo no Log.txt\n");
    }
}

void writeLog(char *info)
{
    printf("entrei no writeLog()\n");
    FILE *fp;
    fp = fopen(LOG_FILE, "a+");

    if (fp == NULL)
    {
        perror("Can not open file");
        exit(0);
    }

    printf("Log.txt aberto\n");

    writeTime(1, info, fp);

    writeTime(0, info, fp);

    fclose(fp);

    printf("escrevi no log\n");
}