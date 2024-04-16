#include "../include/info.h"

char** parse_args(char *args){
    int n_args = 0;
    char** lista_argumentos = malloc(sizeof(*char));
    if(lista_argumentos == NULL) perror("Falha ao alocar memória\n");

    char *nome_prog = strtok(args, " ");
    char *args = strtok(NULL, "");
    lista_argumentos[0] = nome_prog;
    n_args++;

    for(; args!=NULL ; args = strtok(NULL, "")){
        n_args++;
        lista_argumentos = realloc(sizeof(*char) * n_args);
        lista_argumentos[n_args - 1] = strdup(args);
    }
    lista_argumentos = realloc(lista_argumentos, sizeof(char*) * (num_args + 1)); // Null no ultimo slot do array para satisfazer execvp
    lista_argumentos[num_args] = NULL;

    return lista_argumentos;
}

void freeargs(char **lista_argumentos){
    for (int i = 1; i < num_args; i++) {
        free(lista_argumentos[i]);
    }
    free(lista_argumentos);
}

//executa com a flag -u
void executa_u(char *args, int fifo){
    char **lista_argumentos = parse_args(args);

    pid_t pid = 0;
    int fd[2];
    pipe(fd);

    if(fork() != 0){
        write(2, "Erro ao criar processo-filho\n", 31);
        wait(NULL);
        close(fd[1]);
        read(fd[0], &pid, sizeof(pid_t));
        _exit(ERROR);
    }
    else {
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
        sprintf(message_to_fifo, "EM EXECUÇÃO;PID: %d;%s;%ld;%ld", getpid(), prog_name, time.tv_sec, time.tv_usec);
        write(fifo, message_to_fifo, 1024);

        if(execvp(nome_prog, lista_argumentos) < 0){
            write(1, "Entrada Inválida.\n", 20);
            gettimeofday(&time, NULL);
            char message_to_fifo2[1024]; 
            sprintf(message_to_fifo2, "EXECUTADO;PID: %d;%s;%ld;%ld", getpid(), prog_name, time.tv_sec, time.tv_usec);
            write(fifo, message_to_fifo2, 1024);
            _exit(ERROR);
        }
    
    struct timeval time2; 
    gettimeofday(&time2, NULL);
    char message_to_fifo2[1024]; 
    sprintf(message_to_fifo2, "EXECUTADO;PID: %d;%s;%ld;%ld", getpid(), prog_name, time2.tv_sec, time2.tv_usec);
    write(fifo, message_to_fifo2, 1024);
    _exit(0);

    dup2(fifo, STDERR_FILENO);
    dup2(fifo, STDOUT_FILENO);
    freeargs(lista_argumentos);
}
