#ifndef QUEUE_H
#define QUEUE_H

#include "commands.h"

#define TAMANHO_LISTA 150

int next_task_id();

void start_queue(Comandos *queue);

bool is_queue_empty(Comandos *queue);

bool is_command_empty(Comandos queue);

int compareComandosTime(Comandos novo, Comandos queued);

Comandos *shiftRight(Comandos *queue);

int get_correct_index(Comandos *queue, Comandos novo);

Comandos *addTask(Comandos *queue, Comandos novo, int index);

void add_task_toQueue(Comandos *queue, Comandos novo);

Comandos *move_elements_back(Comandos *queue, int start_index);

void remove_task_fromQueue(Comandos *queue, Comandos remove);

Comandos queueGetNextTask(Comandos *queue);

bool is_command_empty(Comandos queue);

#endif