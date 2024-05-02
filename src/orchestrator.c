#include "../include/info.h"

// executa com a flag -u
void executa_u(char *args, int fifo, Comandos *lista_comandos, int *size)
{
    char **lista_argumentos = NULL, *aux = strdup(args), *token = NULL;
    int num_args = 0, tempo;
    token = strsep(&aux, ";");
    tempo = atoi(token);
    char *aux1 = strdup(aux);

    while ((token = strsep(&aux, " ")) != NULL)
    {
        lista_argumentos = realloc(lista_argumentos, sizeof(char *) * (num_args + 1));
        lista_argumentos[num_args] = strdup(token);
        num_args++;
    }

    lista_argumentos = realloc(lista_argumentos, sizeof(char *) * (num_args + 1));
    lista_argumentos[num_args] = NULL;

    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == 0)
    {
        pid_t pid = getpid();
        close(fd[0]);
        write(fd[1], &pid, sizeof(pid_t));
        char pid_message[32];
        sprintf(pid_message, "PID: %d\n", pid);
        write(1, pid_message, strlen(pid_message));

        char exec[1024];
        sprintf(exec, "EM EXECUÇÃO \"%s\"\n", aux1);
        write(1, exec, strlen(exec));

        struct timeval time;
        gettimeofday(&time, NULL);
        char message_to_fifo[1024];
        sprintf(message_to_fifo, "EM EXECUÇÃO;PID: %d;%s", getpid(), lista_argumentos[0]);
        write(fifo, message_to_fifo, 1024);

        if (execvp(lista_argumentos[0], lista_argumentos) < 0)
        {
            write(1, "Entrada Inválida.\n", 20);
            gettimeofday(&time, NULL);
            char message_to_fifo2[1024];
            sprintf(message_to_fifo2, "EXECUTADO;PID: %d;%s;%ld", getpid(), lista_argumentos[0], time.tv_usec);
            write(fifo, message_to_fifo2, 1024);
            _exit(ERROR);
        }
    }
    else if (pid == -1)
    {
        write(2, "Erro ao criar processo-filho\n", 29);
        close(fd[1]);
        read(fd[0], &pid, sizeof(pid_t));
        return;
    }
    else
    {
        wait(NULL);
        close(fd[1]);
        read(fd[0], &pid, sizeof(pid_t));
    }

    struct timeval time2;
    gettimeofday(&time2, NULL);
    char message_to_fifo2[1024];
    sprintf(message_to_fifo2, "EXECUTADO;PID: %d;%s;%ld", getpid(), lista_argumentos[0], time2.tv_usec);
    write(fifo, message_to_fifo2, 1024);
    for (int i = 1; i < num_args; i++)
    {
        free(lista_argumentos[i]);
    }
    free(lista_argumentos);
}


int main(int argc, char *argv[])
{
    int sizeofcomands = sizeof(Comandos);
    int size = 0;
    Comandos *lista_comandos = malloc(sizeofcomands * 15);

    /*if (mkfifo(FIFO, 0666) < 0)
    {
        write(2, "Error making Server FIFO\n", 25);
        return 1;
    }
    if (access(FIFO, F_OK) != -1) // usamos a função "acess" para tentar aceder ao FIFO, ou seja, verificar se ele existe
    {
        write(2, "Client FIFO has already been created\n", 38);
    }*/

    if (access(FIFO, F_OK) != -1){ // usamos a função "acess" para tentar aceder ao FIFO, ou seja, verificar se ele existe
        write(2, "Client FIFO has already been created\n", 38);
    } else {
        if (mkfifo(FIFO, 0666) < 0){
            write(2, "Error making Server FIFO\n", 25);
            return 1;
        }
    }

    if (argc < 3)
    {
        write(2, "Invalid number of arguments.\n", 30);
        return 1;
    }

    char output[128];
    snprintf(output, sizeof(output), "%s/commands.log", argv[1]);

    printf("Servidor operacional ...\n");

    char buffer[1024];
    int logs = open(output, O_WRONLY | O_CREAT | O_APPEND, 0666);

    while (true)
    {
        int fifo_servidor = open(FIFO, O_RDONLY);
        if (fifo_servidor < 0)
        {
            write(2, "Error opening reading fifo.\n", 29);
            return 1;
        }
        if (read(fifo_servidor, buffer, 1024) <= 0)
        {
            write(2, "Erro ao ler input\n", 18);
            return 1;
        }
        close(fifo_servidor);
        if (strcmp(buffer, "exit") == 0)
        {
            printf("Terminando servidor...\n");
            break;
        }

        executa_u(buffer, fifo_servidor, lista_comandos, &size);
    }
    close(logs);
    return 0;
}
