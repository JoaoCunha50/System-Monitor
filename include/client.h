#ifndef CLIENT_H
#define CLIENT_H

#include "commands.h"
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
#include <sys/wait.h>
#include <sys/time.h>
#include <stdbool.h>

#define ERROR 255
#define SERVIDOR "../tmp/servidor"
#define CLIENTE "../tmp/cliente"

#endif