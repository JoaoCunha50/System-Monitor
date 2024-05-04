CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =

all: folders orchestrator client  

orchestrator: bin/orchestrator

client: bin/client

folders:
	@mkdir -p src include obj bin tmp

bin/orchestrator: obj/orchestrator.o obj/queue.o obj/commands.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/client: obj/client.o obj/queue.o obj/commands.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f obj/* tmp/* bin/client bin/orchestrator
