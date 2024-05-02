#include "../include/orchestrator.h"

// executa com a flag -u
void executa_u(char *args, int fifo, Comandos comandoFifo, int logs)
{
    char *prog_name = strtok(args, " ");
    char *arg_str = strtok(NULL, "");
    char **lista_argumentos = malloc(sizeof(char *));
    lista_argumentos[0] = prog_name;
    int num_args = 1;

    while (arg_str != NULL)
    {
        num_args++;
        lista_argumentos = realloc(lista_argumentos, sizeof(char *) * num_args);
        lista_argumentos[num_args - 1] = strdup(arg_str);
        arg_str = strtok(NULL, "");
    }

    lista_argumentos = realloc(lista_argumentos, sizeof(char *) * (num_args + 1));
    lista_argumentos[num_args] = NULL;

    int status;
    int fd[2];
    pipe(fd);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    pid_t pid = fork();

    if (pid == 0)
    {
        char pid_message[32];
        pid_t pid_child = getpid();
        sprintf(pid_message, "PID: %d\n", pid_child);
        write(1, pid_message, strlen(pid_message));
        close(fd[0]);
        write(fd[1], &pid_child, sizeof(pid_child));

        char exec[1024];
        sprintf(exec, "EXECUTING \"%s\"\n", prog_name);
        strcpy(comandoFifo.status, "EXECUTING;");

        if (execvp(lista_argumentos[0], lista_argumentos) < 0)
        {
            write(1, "Entrada Inválida.\n", 20);
            _exit(ERROR);
        }
        exit(1);
    }
    else if (pid == -1)
    {
        write(2, "Erro ao criar processo-filho\n", 29);
        return;
    }

    waitpid(pid, &status, 0);
    pid_t pid_comando = 0;
    close(fd[1]);
    read(fd[0], &pid_comando, sizeof(pid_comando));
    comandoFifo.pid = pid_comando;

    gettimeofday(&end, NULL);
    long time_exec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

    strcpy(comandoFifo.status, "EXECUTED;");
    comandoFifo.exec_time = (float)time_exec;

    atualizaStatus(comandoFifo, logs);

    for (int i = 1; i < num_args; i++)
    {
        free(lista_argumentos[i]);
    }
    free(lista_argumentos);
}

int main(int argc, char *argv[])
{

    if (mkfifo(SERVIDOR, 0666) < 0)
    {
        write(2, "Error making Server FIFO\n", 25);
        return 1;
    }

    if (mkfifo(CLIENTE, 0666) < 0)
    {
        write(2, "Error making Server FIFO\n", 25);
        return 1;
    }

    if (argc < 3)
    {
        write(2, "Invalid number of arguments.\n", 30);
        return 1;
    }

    // Para obter o máximo de tarefas
    int tasks_parallel = atoi(argv[2]);
    char error[100]; // criar a mensagem de erro
    if (tasks_parallel == 0)
    {
        strcpy(error, "Please introduce a valid value");
        write(2, error, strlen(error));
        exit(EXIT_FAILURE);
    }

    char outputPath[128];
    strcpy(outputPath, argv[1]);

    char output_log_path[256];
    snprintf(output_log_path, sizeof(output_log_path), "%s/Commands.log", outputPath);

    write(1, "Servidor operacional ...\n", strlen("Servidor operacional ...\n"));

    int logs = open(output_log_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (logs == -1)
    {
        perror("Error creating/opening logs file");
        exit(EXIT_FAILURE);
    }

    char format[] = "PID;STATUS;EXEC TIME;PROGRAM NAME\n";
    if (write(logs, format, strlen(format)) <= 0)
    {
        perror("Error writing in logs file");
        exit(EXIT_FAILURE); // return caso aconteça algo, i.e caso nao consiga escrever nada no ficheiro das logs
    }

    int flag = 1;

    while (flag)
    {
        Comandos comandoFifo;
        int fifo_servidor = open(SERVIDOR, O_RDONLY);
        if (fifo_servidor < 0)
        {
            write(2, "Error opening reading fifo.\n", 29);
            return 1;
        }
        while ((read(fifo_servidor, &comandoFifo, sizeof(Comandos)) > 0))
        {
            if (strcmp(comandoFifo.status, "EXECUTED") == 0)
            {
                // acabou a tarefa
                continue;
            }
            else if (strcmp(comandoFifo.command, "execute") == 0)
            {
                executa_u(comandoFifo.prog_name, fifo_servidor, comandoFifo, logs);
                close(fifo_servidor);
            }

            else if (strcmp(comandoFifo.command, "status") == 0)
            {
                int fifo_cliente = open(CLIENTE, O_WRONLY);
                ssize_t bytes_escritos = write(fifo_cliente, output_log_path, strlen(output_log_path));
                if (bytes_escritos <= 0)
                {
                    perror("Erro ao escrever no Fifo\n");
                }
                close(fifo_servidor);
            }

            else if (strcmp(comandoFifo.command, "exit") == 0)
            {
                printf("Terminando servidor...\n");
                flag = 0;
                close(fifo_servidor);
                break;
            }
        }
    }
    close(logs);
    unlink(SERVIDOR);
    unlink(CLIENTE);
    return 0;
}