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
// #include <sys/wait.h>
#include <sys/time.h>
#include <stdbool.h>

#define ERROR 255
#define FIFO "../tmp/fifo"

typedef struct comandos
{
    int pid;
    char *prog_name;
    char *status; // @enum QUEUED; EXECUTED; EXECUTING
    float exec_time;
    float estimated_time;
} Comandos;
