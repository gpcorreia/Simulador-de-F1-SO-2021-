#include "includes.h"
#include "const.h"
#include "defines.h"
#include "structs.h"

void writeTime(int flag, char *frase, FILE *f)
{
    time_t t = time(NULL);
    time_t segundos;
    time(&segundos);

    struct tm *info;

    info = localtime(&segundos);
    char buffer[200];
    info = localtime(&t);

    strftime(buffer, 30, "%H:%M:%S ", info);

    strcat(buffer, frase);

    if (flag == 1) //se flag for igual a 1 escreve na consola
    {
        printf("%d/%d/%d %s\n", info->tm_mday, info->tm_mon, info->tm_year + 1900, buffer);
    }
    else if (flag == 0)
    {
        fprintf(f, "%d/%d/%d %s\n", info->tm_mday, info->tm_mon, info->tm_year + 1900, buffer); // se for igual a 0 escreve no ficheiro
    }
}

void writeLog(char *info)
{
    FILE *fp;
    fp = fopen(LOG_FILE, "a+");

    if (fp == NULL)
    {
        perror("Can not open file");
        exit(0);
    }

    writeTime(1, info, fp);

    writeTime(0, info, fp);

    fclose(fp);
}