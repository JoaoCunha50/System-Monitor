#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
//#include <sys/wait.h>
#include <sys/time.h>
#include <stdbool.h>

#define ERROR 255
#define CLIENTE "Cliente"
#define SERVIDOR "Servidor"

typedef struct executando
{
    int pid;
    char *prog_name;
    long int secs;
    long int usec;
} Executando;

typedef struct executados
{
    int pid;
    char *prog_name;
    float exec_time;
} Executados;