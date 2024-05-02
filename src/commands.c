#include "../include/info.h"

void setListaComandosPid(Comandos* lista_comandos, int i, int pid) {
    lista_comandos[i].pid = pid;
}

void setListaComandosProgName(Comandos* lista_comandos, int i, char* prog_name) {
    lista_comandos[i].prog_name = malloc(strlen(prog_name) + 1);
    strcpy(lista_comandos[i].prog_name, prog_name);
}

void setListaComandosStatus(Comandos* lista_comandos, int i, char* status) {
    lista_comandos[i].status = malloc(strlen(status) + 1);
    strcpy(lista_comandos[i].status, status);
}

void setListaComandosExec_time(Comandos* lista_comandos, int i, long int exec_time) {
    lista_comandos[i].exec_time = (float) exec_time;
}

void setListaComandosEstimated_time(Comandos* lista_comandos, int i, int estimated_time) {
    lista_comandos[i].estimated_time = (float) estimated_time;
}

int sizeofComands()
{
    int tamanho = sizeof(Comandos);
    return tamanho;
}

int compareCommands(const void *a, const void *b)
{
    int compare = ((Comandos *)b)->estimated_time - ((Comandos *)a)->estimated_time;
    return compare;
}