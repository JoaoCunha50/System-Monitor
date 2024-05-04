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

int atualizaLogs(Comandos comando, int logs)
{
    char status[1024];
    status[0] = '\0';
    snprintf(status, sizeof(status), "PID: %d;Status: %s;Time: %f ms;Prog: %s\n", comando.pid, comando.status, comando.exec_time, comando.prog_name);

    ssize_t bytes_escritos = write(logs, status, strlen(status));
    if (bytes_escritos <= 0)
    {
        write(2, "Erro ao escrever no FIFO\n", 25);
        exit(EXIT_FAILURE);
    }

    return 0;
}

int atualizaStatus(int fifo_cliente, Comandos *received, Comandos *queue)
{
    char status[4096];
    status[0] = '\0'; // Inicializa a string status

    snprintf(status, sizeof(status), "Executing\n\n");
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        if ((!is_command_empty(received[i])) && strcmp(received[i].status, "EXECUTING") == 0)
        {
            snprintf(status + strlen(status), sizeof(status), "ID: %d Prog: %s\n", received[i].id, received[i].prog_name);
        }
    }

    snprintf(status + strlen(status), sizeof(status), "\nScheduled\n\n");
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        if ((!is_command_empty(queue[i])) && strcmp(queue[i].status, "QUEUED") == 0)
        {
            snprintf(status + strlen(status), sizeof(status), "ID: %d Prog: %s\n", queue[i].id, queue[i].prog_name);
        }
    }

    snprintf(status + strlen(status), sizeof(status), "\nCompleted\n\n");
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        if ((!is_command_empty(received[i])) && strcmp(received[i].status, "EXECUTED") == 0)
        {
            snprintf(status + strlen(status), sizeof(status) - strlen(status), "ID: %d Prog: %s Time: %f ms\n", received[i].id, received[i].prog_name, received[i].exec_time);
        }
    }

    ssize_t bytes_escritos = write(fifo_cliente, status, strlen(status));
    if (bytes_escritos <= 0)
    {
        write(2, "Erro ao escrever no FIFO\n", 25);
        exit(EXIT_FAILURE);
    }

    return 0;
}
