#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#define TAMANHO_LISTA 150

typedef struct comandos
{
    int id;
    int pid;
    char command[20];
    char flag[10];
    char prog_name[301];
    char status[30]; // @enum QUEUED; EXECUTED; EXECUTING
    float exec_time;
    int estimated_time;
} Comandos;

int compareCommands(const void *a, const void *b);
int atualizaLogs(Comandos comando, int logs);
int atualizaStatus(int fifo_cliente, Comandos *received, Comandos *queue);

#endif
