#include "../include/info.h"

int sizeofComands(Comandos *comands)
{
    int tamanho = sizeof(comands);
    return tamanho;
}

int compareCommands(const void *a, const void *b)
{
    int compare = ((Comandos *)b)->pid - ((Comandos *)a)->pid;
    return compare;
}

int main(int argc, char *argv[])
{
    int fifo_servidor = open(FIFO, O_WRONLY);
    if (fifo_servidor < 0)
    {
        write(2, "Error opening reading fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    Comandos *lista_comandos = malloc(sizeofComands(lista_comandos) * 15);
    int tamanho_comandos = 15, nr_comandos = 0;

    int flag = 0;

    while (!flag)
    {
        if (strcmp(argv[1], "status") == 0)
        {
            for (int i = 0; i < nr_comandos; i++)
            {
                char status[100];
                if (strcmp(lista_comandos[i].status, "EXECUTED"))
                {
                    sprintf(status, "PID: %d; Programa: %s; Tempo de Execução: %lld ms\n", lista_comandos[i].pid, lista_comandos[i].prog_name, lista_comandos[i].exec_time);
                }
                else if (strcmp(lista_comandos[i].status, "EXECUTING"))
                {
                    sprintf(status, "PID: %d; Programa: %s;\n", lista_comandos[i].pid, lista_comandos[i].prog_name);
                }
                else if (strcmp(lista_comandos[i].status, "QUEUED"))
                {
                    sprintf(status, "PID: %d; Programa: %s; Tempo Estimado: %d;\n", lista_comandos[i].pid, lista_comandos[i].prog_name, lista_comandos[i].estimated_time);
                }
                write(fifo_servidor, status, sizeof(status));
            }
        }
        else if (strcmp(argv[1], "execute") == 0)
        {
            if (strcmp(argv[3], "-u") == 0)
            {
                write(fifo_servidor, argv[4], strlen(argv));
            }
        }
        else if (strcmp(argv[1], "exit") == 0)
        {
            flag = 1;
            break;
        }
    }
    free(lista_comandos);

    return 0;
}
