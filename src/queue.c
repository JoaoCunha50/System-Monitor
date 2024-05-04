#include "../include/queue.h"

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
        strcpy(queue[i].status, "QUEUED");
        queue[i].exec_time = 0;
    }
}

bool is_queue_empty(Comandos *queue)
{
    for (int i = 0; i < TAMANHO_LISTA; i++)
    {
        if (strcmp(queue[i].prog_name, "") != 0)
        {
            return false;
        }
    }
    return true;
}

bool is_command_empty(Comandos queue)
{
    if (strcmp(queue.prog_name, "") == 0 && queue.id == 0)
    {
        return true;
    }
    return false;
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
    while (index < TAMANHO_LISTA && strcmp(queue[index].prog_name, "") != 0 && compareComandosTime(novo, queue[index]) >= 0)
    {
        index++;
    }
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
    if (is_queue_empty(queue) || compareComandosTime(novo, queue[0]) < 0)
    {
        queue = shiftRight(queue);
        queue[0] = novo;
    }
    else
    {
        int index_novo = get_correct_index(queue, novo);
        if (index_novo < TAMANHO_LISTA)
        {
            queue = addTask(queue, novo, index_novo);
        }
        else
        {
            printf("A fila está cheia. Não é possível adicionar mais tarefas.\n");
        }
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
