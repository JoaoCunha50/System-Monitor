#include "../include/orchestrator.h"

// executa com a flag -u
Comandos executa_u(int fifo, Comandos *received, Comandos comando_exec, int logs)
{
    char *args = strdup(comando_exec.prog_name);
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
        strcpy(comando_exec.status, "EXECUTING");

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
        exit(EXIT_FAILURE);
    }

    waitpid(pid, &status, 0);
    pid_t pid_comando = 0;
    close(fd[1]);
    read(fd[0], &pid_comando, sizeof(pid_comando));
    comando_exec.pid = pid_comando;

    gettimeofday(&end, NULL);
    long time_exec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    remove_task_fromQueue(received, comando_exec);

    strcpy(comando_exec.status, "EXECUTED");
    comando_exec.exec_time = (float)time_exec;

    atualizaLogs(comando_exec, logs);

    for (int i = 1; i < num_args; i++)
    {
        free(lista_argumentos[i]);
    }
    free(lista_argumentos);

    return comando_exec;
}

void separaTarefas(char *prog, char *tarefas[], int* num_tarefas) {
    char *aux = strtok(prog, "|");
    int i = 0;

    for (; aux != NULL && i < 10; i++) {
        while (*aux == ' '){
            aux++;
        }
        int len = strlen(aux);
        while (len > 0 && aux[len - 1] == ' ') {
            aux[--len] = '\0';
        }

        tarefas[i] = malloc((strlen(aux) + 1) * sizeof(char*));
        strcpy(tarefas[i], aux);
        (*num_tarefas)++;

        aux = strtok(NULL, "|");
    }

    for (; i < 10; i++) {
        tarefas[i] = NULL;
        i++;
    }
}

Comandos executa_p(int fifo, Comandos *received, Comandos comando_exec, int logs) {
    char* tarefas[10];
    int num_tarefas = 0;
    char* prog = strdup(comando_exec.prog_name);

    separaTarefas(prog, tarefas, &num_tarefas);

    int fd[num_tarefas - 1][2];
    int fd1[2];
    pipe(fd1);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    char exec[1024];
    sprintf(exec, "EXECUTING \"%s\"\n", comando_exec.prog_name);
    write(1, &exec, strlen(exec));

    strcpy(comando_exec.status, "EXECUTING");

    for (int i = 0; i < num_tarefas; ++i) {
        char *aux = strdup(tarefas[i]);
        char *token = strtok(aux, " ");
        char *programa = token;
        char *lista_argumentos[11];

        int j = 0;
        while (token != NULL && j < 10) {
            lista_argumentos[j] = token;
            token = strtok(NULL, " ");
            j++;
        }
        lista_argumentos[j] = NULL;

        pid_t pid;

        if (i == 0) {
            pipe(fd[i]);
            if((pid = fork()) == 0) {

                dup2(fd[i][1], STDOUT_FILENO);
                dup2(logs,STDERR_FILENO);
                close(fd[i][0]);
                close(fd[i][1]);

                execvp(programa, lista_argumentos);

                perror("Entrada Inválida.\n");
                _exit(1);
            } else if (pid == -1) {
                perror("fork()");
                _exit(1);
            }

            close(fd[i][1]);

        } else if (i < num_tarefas - 1) {
            pipe(fd[i]);
            if((pid = fork()) == 0) {

                dup2(fd[i - 1][0], STDIN_FILENO);
                dup2(fd[i][1], STDOUT_FILENO);
                dup2(logs,STDERR_FILENO);
                close(fd[i - 1][0]);
                close(fd[i][0]);
                close(fd[i][1]);

                execvp(programa, lista_argumentos);

                perror("Entrada Inválida.\n");
                _exit(1);
            } else if (pid == -1) {
                perror("fork()");
                _exit(1);
            }

            close(fd[i - 1][0]);
            close(fd[i][1]);

        } else {
            if((pid = fork()) == 0) {

                char pid_message[32];
                pid_t pid_child = getpid();
                sprintf(pid_message, "PID: %d\n", pid_child);
                write(1, pid_message, strlen(pid_message));
                close(fd1[0]);
                write(fd1[1], &pid_child, sizeof(pid_child));

                dup2(fd[i - 1][0], STDIN_FILENO);
                dup2(logs,STDOUT_FILENO);
                dup2(logs,STDERR_FILENO);
                close(fd[i - 1][0]);

                execvp(programa, lista_argumentos);

                perror("Entrada Inválida.\n");
                _exit(1);
            } else if (pid == -1) {
                perror("fork()");
                _exit(1);
            }

            close(fd[i - 1][0]);
        }
    }

    for (int i = 0; i < num_tarefas; i++) {
        wait(NULL);
    }

    pid_t pid_comando = 0;
    close(fd1[1]);
    read(fd1[0], &pid_comando, sizeof(pid_comando));
    comando_exec.pid = pid_comando;

    gettimeofday(&end, NULL);
    long time_exec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    remove_task_fromQueue(received, comando_exec);

    strcpy(comando_exec.status, "EXECUTED");
    comando_exec.exec_time = (float)time_exec;

    atualizaLogs(comando_exec, logs);

    for (int i = 0; i < num_tarefas; i++) {
        free(tarefas[i]);
    }

    return comando_exec;
}

int main(int argc, char *argv[])
{
    Comandos *queue = calloc(0, sizeof(Comandos));
    start_queue(queue);
    Comandos *received = calloc(0, sizeof(Comandos));
    start_queue(received);

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

    write(1, "Servidor operacional ...\n", strlen("Servidor operacional ...\n"));

    char outputPath[128];
    strcpy(outputPath, argv[1]);

    char output_log_path[256];
    snprintf(output_log_path, sizeof(output_log_path), "%s/Commands.log", outputPath);

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
    int nr_comandos = 0;

    while (flag)
    {
        Comandos comando_lido;
        Comandos comando_exec;
        int fifo_servidor = open(SERVIDOR, O_RDONLY);
        if (fifo_servidor < 0)
        {
            write(2, "Error opening reading fifo.\n", 29);
            return 1;
        }
        while ((read(fifo_servidor, &comando_lido, sizeof(Comandos)) > 0))
        {
            if (strcmp(comando_lido.status, "EXECUTED") == 0)
            {
                remove_task_fromQueue(received, comando_lido); // remove o comando com o status EXECUTING
                add_task_toQueue(received, comando_lido); // adiciona o comando com o status EXECUTED
                nr_comandos--;
                close(fifo_servidor);
                continue; 
            }
            if (strcmp(comando_lido.command, "shutdown") == 0)
            {
                write(1, "Terminando servidor...\n", 24);
                flag = 0;
                close(fifo_servidor);
                break;
            }
            if (strcmp(comando_lido.command, "status") == 0)
            {
                int fifo_cliente = open(CLIENTE, O_WRONLY);
                atualizaStatus(fifo_cliente, received, queue);
                close(fifo_cliente);
                close(fifo_servidor);
                continue;
            }

            if (strcmp(comando_lido.command, "execute") == 0)
            {
                int fifo_cliente = open(CLIENTE, O_WRONLY);
                comando_lido.id = next_task_id();
                write(fifo_cliente, &comando_lido.id, sizeof(int));
                close(fifo_cliente);

                strcpy(comando_lido.status, "QUEUED");
                add_task_toQueue(queue, comando_lido);
                close(fifo_servidor);
            }
        }
        if (nr_comandos < tasks_parallel && !is_queue_empty(queue))
        {
            comando_exec = queueGetNextTask(queue);
            remove_task_fromQueue(queue, comando_exec);

            strcpy(comando_exec.status, "EXECUTING");
            add_task_toQueue(received, comando_exec);
            nr_comandos++;

            pid_t pid = fork();
            if (pid == 0)
            {
                if (strcmp(comando_exec.flag, "-u") == 0) {
                    comando_exec = executa_u(fifo_servidor, received, comando_exec, logs);
                } else if (strcmp(comando_exec.flag, "-p") == 0) {
                    comando_exec = executa_p(fifo_servidor, received, comando_exec, logs);
                }
                strcpy(comando_exec.status, "EXECUTED");

                fifo_servidor = open(SERVIDOR, O_WRONLY);
                write(fifo_servidor, &comando_exec, sizeof(Comandos));
                close(fifo_servidor);
                exit(EXIT_SUCCESS);
            }
            else if (pid == -1)
            {
                perror("Erro ao criar processo-filho");
                exit(EXIT_FAILURE);
            }
        }
    }
    close(logs);
    unlink(SERVIDOR);
    unlink(CLIENTE);
    return 0;
}
