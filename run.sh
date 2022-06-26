#!/bin/bash
gcc producer.c -Wall -Wextra -o prod.out
gcc consumer.c -Wall -Wextra -o cons.out
gcc setup.c -Wall -Wextra -o setup.out

./setup.out

for ((i=0;i<$1;i++))
do
# start the producer with time delay i + 1
sleep $i
gnome-terminal -- sh -c "bash -c \"./prod.out $((i+1)); exec bash\""
sleep $i
done
sleep 2
for ((i=0;i<$2;i++))
do
sleep $i
gnome-terminal -- sh -c "bash -c \"./cons.out $((i+1)); exec bash\""
sleep $i
done