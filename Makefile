CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =

all: folders orchestrator client  

orchestrator: bin/orchestrator

client: bin/client

folders:
	@mkdir -p src include obj bin tmp

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/orchestrator: obj/orchestrator.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/client: obj/client.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f obj/* tmp/* bin/*
