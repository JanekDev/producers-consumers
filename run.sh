#!/bin/bash
gcc producer.c -Wall -Wextra -o prod.out
gcc consumer.c -Wall -Wextra -o cons.out
gcc setup.c -Wall -Wextra -o setup.out

./setup.out

for ((i=0;i<$1;i++))
do
gnome-terminal -- sh -c "bash -c \"./prod.out; exec bash\""
done
sleep 2
for ((i=0;i<$2;i++))
do
gnome-terminal -- sh -c "bash -c \"./cons.out; exec bash\""
done