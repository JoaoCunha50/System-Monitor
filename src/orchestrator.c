#include "../include/orchestrator.h"

int task_id = 1;

int next_task_id()
{
    return task_id++;
}

void start_queue(Comandos *queue)
{
    queue = malloc(TAMANHO_LISTA * sizeof(Comandos));
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        strcpy(queue[i].status, "QUEUE");
        queue[i].exec_time = 0;
    }
}

bool is_queue_empty(Comandos *queue)
{
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        if (strcmp(queue[i].prog_name, "") == 0)
        {
            return false;
        }
    }
    return true;
}

int compareComandosTime(Comandos novo, Comandos queued)
{
    if (novo.estimated_time < queued.estimated_time)
        return 1; // Retorna 1 se o tempo estimado do novo for menor que o tempo estimado da primeira tarefa na fila
    else
        return 0; // Retorna 0 caso contrário
}
Comandos *shiftRight(Comandos *queue)
{
    for (int i = 0; i < TAMANHO_LISTA - 1; i++)
    {
        queue[i] = queue[i + 1];
    }
    return queue;
}

int get_correct_index(Comandos *queue, Comandos novo)
{
    int index = 0;
    for (index; index < TAMANHO_LISTA && strcmp(queue[index].prog_name, "") == 0 && !(compareComandosTime(novo, queue[index])); index++)
        ;
    return index;
}

Comandos *addTask(Comandos *queue, Comandos novo, int index)
{
    for (int i = TAMANHO_LISTA - 1; i > index; i--)
    {
        queue[i] = queue[i - 1];
    }
    queue[index] = novo;

    return queue;
}

void add_task_toQueue(Comandos *queue, Comandos novo)
{
    if (is_queue_empty(queue) || compareComandosTime(novo, queue[0])) // adicionamos à 1ª posição, caso esteja vazia ou o seu tempo seja menor
    {
        queue = shiftRight(queue); // saltamos uma posição para dar espaço para a primeira posição
        queue[0] = novo;
    }
    else // caso nao esteja na primeira posição, encontramos o indice correto para adicionar à queue
    {
        int index_novo = get_correct_index(queue, novo);
        queue = addTask(queue, novo, index_novo);
    }
}

Comandos *move_elements_back(Comandos *queue, int start_index)
{
    for (int j = start_index; j < TAMANHO_LISTA - 1; j++)
    {
        queue[j] = queue[j + 1];
    }
    queue[TAMANHO_LISTA - 1].prog_name[0] = '\0';

    return queue;
}

void remove_task_fromQueue(Comandos *queue, Comandos remove)
{
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        if (queue[i].command != NULL && queue[i].id == remove.id)
        {
            queue = move_elements_back(queue, i);
            break;
        }
    }
}

Comandos queueGetNextTask(Comandos *queue)
{
    return queue[0];
}

void deleteQueue(Comandos *queue)
{
    free(queue);
}

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
        write(1, &exec, strlen(exec));
        strcpy(comandoFifo.status, "EXECUTING");

        dup2(logs, STDOUT_FILENO);
        dup2(logs, STDERR_FILENO);

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

    strcpy(comandoFifo.status, "EXECUTED");
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
    Comandos *queue;
    Comandos *executed;
    Comandos *executing;

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
    close(logs);

    int flag = 1;

    while (flag)
    {
        Comandos comandoFifo;
        int fifo_servidor = open(SERVIDOR, O_RDONLY);
        logs = open(output_log_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
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
                close(logs);
                logs = open(output_log_path, O_RDONLY | O_CREAT | O_APPEND, 0666);
                char logs_output[4096];
                if (read(logs, logs_output, sizeof(logs_output)) <= 0)
                {
                    perror("Erro ao ler do FIFO\n");
                    exit(EXIT_FAILURE);
                }
                close(logs);

                int fifo_cliente = open(CLIENTE, O_WRONLY);
                ssize_t bytes_escritos = write(fifo_cliente, logs_output, strlen(logs_output));
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