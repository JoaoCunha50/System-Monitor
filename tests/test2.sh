#!/bin/bash

cd ../bin

./client execute 2 -u "ls -l" & 
sleep 0.25
./client execute 7 -u "find ." & 
sleep 0.25
./client execute 3 -u "uname -a" & 
sleep 0.25
./client execute 10 -p "cat ../src/orchestrator.c | grep "open" | wc -l"

sleep 10 &

./client status 