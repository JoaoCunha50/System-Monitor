#include "../include/info.h"

char **parse_args(char *args, int *n_args)
{
    (*n_args) = 0;
    char **lista_argumentos = malloc(sizeof(char *));
    if (lista_argumentos == NULL)
        perror("Falha ao alocar memória\n");

    char *nome_prog = strtok(args, " ");
    char *args2 = strtok(NULL, "");
    lista_argumentos[0] = nome_prog;
    (*n_args)++;

    for (; args2 != NULL; args2 = strtok(NULL, ""))
    {
        (*n_args)++;
        lista_argumentos = realloc(lista_argumentos, sizeof(char *) * (*n_args));
        lista_argumentos[*n_args - 1] = strdup(args);
    }
    lista_argumentos = realloc(lista_argumentos, sizeof(char *) * ((*n_args) + 1)); // Null no ultimo slot do array para satisfazer execvp
    lista_argumentos[(*n_args)] = NULL;

    return lista_argumentos;
}

void freeargs(char **lista_argumentos, int n_args)
{
    for (int i = 1; i < n_args; i++)
    {
        free(lista_argumentos[i]);
    }
    free(lista_argumentos);
}

// executa com a flag -u
void executa_u(char *args, int fifo)
{
    int n_args;
    char **lista_argumentos = parse_args(args, &n_args);

    pid_t pid = 0;
    int fd[2];
    pipe(fd);

    if (fork() != 0)
    {
        write(2, "Erro ao criar processo-filho\n", 31);
        close(fd[1]);
        read(fd[0], &pid, sizeof(pid_t));
        _exit(ERROR);
    }
    else
    {
        pid_t pid = getpid();
        close(fd[0]);
        write(fd[1], &pid, sizeof(pid_t));
        char pid_message[32];
        sprintf(pid_message, "PID: %d\n", pid);
        write(1, pid_message, strlen(pid_message));

        char exec[1024];
        sprintf(exec, "EM EXECUÇÃO \"%s\"\n", args);
        write(1, exec, strlen(exec));

        struct timeval time;
        gettimeofday(&time, NULL);
        char message_to_fifo[1024];
        sprintf(message_to_fifo, "EM EXECUÇÃO;PID: %d;%s;%ld;%ld", getpid(), lista_argumentos[0], time.tv_sec, time.tv_usec);
        write(fifo, message_to_fifo, 1024);

        if (execvp(lista_argumentos[0], lista_argumentos) < 0)
        {
            write(1, "Entrada Inválida.\n", 20);
            gettimeofday(&time, NULL);
            char message_to_fifo2[1024];
            sprintf(message_to_fifo2, "EXECUTADO;PID: %d;%s;%ld;%ld", getpid(), lista_argumentos[0], time.tv_sec, time.tv_usec);
            write(fifo, message_to_fifo2, 1024);
            _exit(ERROR);
        }
    }
    struct timeval time2;
    gettimeofday(&time2, NULL);
    char message_to_fifo2[1024];
    sprintf(message_to_fifo2, "EXECUTADO;PID: %d;%s;%ld;%ld", getpid(), lista_argumentos[0], time2.tv_sec, time2.tv_usec);
    write(fifo, message_to_fifo2, 1024);
    _exit(0);

    freeargs(lista_argumentos, n_args);
}

int main(int argc, char *argv[])
{
    if (access(CLIENTE, F_OK) != -1) // usamos a função "acess" para tentar aceder ao FIFO, ou seja, verificar se ele existe
    {
        write(2, "Client FIFO has already been created\n", 38);
    }
    else
    {
        if (mkfifo(CLIENTE, 0644) < 0)
        {
            write(2, "Error making Client FIFO\n", 25);
        }
    }

    if (access(SERVIDOR, F_OK) != -1) // tal como no caso anterior, utilizamos a função "acess"
    {
        write(2, "Server FIFO has already been created\n", 38);
    }
    else
    {
        if (mkfifo(SERVIDOR, 0644) < 0) // caso o retorno da função seja <0 (-1), houve um erro na criação do fifo
        {
            write(2, "Error making Server FIFO\n", 25);
        }
    }

    int fifo_leitura = open(CLIENTE, O_RDONLY);
    if (fifo_leitura < 0)
    {
        write(2, "Error opening reading fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    int fifo_escrita = open(SERVIDOR, O_WRONLY);
    if (fifo_escrita < 0)
    {
        write(2, "Error opening writing fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    if (argc < 2)
    {
        write(2, "Invalid number of arguments.\n", 30);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];

    while (read(fifo_leitura, buffer, 1024) == 0)
    {
        char *input = strtok(NULL, " ");

        if (strcmp(input, "-u") == 0)
        {
            input = strtok(NULL, " ");
            executa_u(input, fifo_escrita);
        }
    }
    return 0;
}
