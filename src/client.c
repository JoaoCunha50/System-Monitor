#include "../include/info.h"

int sizeofExecutados(Executados *executados)
{
    return sizeof(executados);
}

int sizeofExecutando(Executando *executando)
{
    return sizeof(executando);
}

int main(int argc, char *argv[])
{
    int fifo_servidor = open(FIFO, O_WRONLY);
    if (fifo_servidor < 0)
    {
        write(2, "Error opening reading fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    Executados *prog_executados = malloc(sizeofExecutados(prog_executados) * 15);
    int tamanho_executados = 15, nr_executados = 0;
    Executando *prog_executando = malloc(sizeofExecutados(prog_executando) * 15);
    int tamanho_executando = 15, nr_executando = 0;

    int flag = 0;

    while (!flag)
    {
        if (strcmp(argv[1], "status") == 0)
        {
            write(fifo_servidor, "Programas executados: \n", 23);
            for (int i = 0; i < nr_executados; i++)
            {
                struct timeval tv_now;
                gettimeofday(&tv_now, NULL);
                char status[100];
                sprintf(status, "PID: %d; Programa: %s; Tempo de Execução: %lld ms\n", prog_executados[i].pid, prog_executados[i].prog_name, prog_executados[i].exec_time);
                write(fifo_servidor, status, sizeof(status));
            }
            write(fifo_servidor, "Programas em execução: \n", 25);
            for (int i = 0; i < nr_executando; i++)
            {
                struct timeval tv_now;
                gettimeofday(&tv_now, NULL);
                char status[100];
                sprintf(status, "PID: %d; Programa: %s;\n", prog_executando[i].pid, prog_executando[i].prog_name);
                write(fifo_servidor, status, sizeof(status));
            }
            write(fifo_servidor, "Finished", 9);
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
    free(prog_executados);
    free(prog_executando);

    return 0;
}
