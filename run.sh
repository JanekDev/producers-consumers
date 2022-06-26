#!/bin/bash
gcc producer.c -Wall -Wextra -o prod
gcc consumer.c -Wall -Wextra -o cons
gcc setup.c -Wall -Wextra -o setup

./setup

for ((i=0;i<$1;i++))
do
gnome-terminal -- sh -c "bash -c \"./prod; exec bash\""
done
sleep 2
for ((i=0;i<$2;i++))
do
gnome-terminal -- sh -c "bash -c \"./cons; exec bash\""
done