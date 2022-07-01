#!/bin/bash

#clean the environment
rm /dev/shm/buffer
rm /dev/shm/sem.Sc
rm /dev/shm/sem.Sp
rm /dev/shm/sem.Sqp
rm /dev/shm/sem.Sqc

#compile with warning flags to ensure program correctness   
gcc producer.c -Wall -Wextra -o prod.out
gcc consumer.c -Wall -Wextra -o cons.out
gcc setup.c -Wall -Wextra -o setup.out

#run setup script to create shared memory and semaphores
./setup.out

#run producer and consumer
for ((i=0;i<$1;i++))
do
    gnome-terminal --title="Producer $i" -- sh -c "bash -c \"./prod.out; exec bash\""
done
for ((i=0;i<$2;i++))
do
    gnome-terminal --title="Consumer $i" -- sh -c "bash -c \"./cons.out; exec bash\""
done