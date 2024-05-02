#include "../include/commands.h"

int sizeofComands()
{
    int tamanho = sizeof(Comandos);
    return tamanho;
}

int compareCommands(const void *a, const void *b)
{
    int compare = ((Comandos *)b)->estimated_time - ((Comandos *)a)->estimated_time;
    return compare;
}

int atualizaStatus(Comandos comando, int logs)
{
    char status[1024];
    snprintf(status, sizeof(status), "PID: %d;Status: %s;Time: %f ;Prog: %s\n", comando.pid, comando.status, comando.exec_time, comando.prog_name);

    ssize_t bytes_escritos = write(logs, status, strlen(status));
    if (bytes_escritos <= 0)
    {
        write(2, "Erro ao escrever no FIFO\n", 25);
        exit(EXIT_FAILURE);
    }

    return 0;
}