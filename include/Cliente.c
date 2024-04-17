#include "../include/info.h"

int main(int argc, char *argv[]){
    int read_fifo = mkfifo(CLIENTE, 0644);
    if(read_fifo != 0) perror("Error making Client FIFO\n");
    int write_fifo = mkfifo(SERVIDOR, 0644);
    if(write_fifo != 0) perror("Error making Server FIFO\n");
    
    //TUDO
}