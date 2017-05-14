#!/bin/bash
if [ $# -eq 0 ]
then
    echo "usage: ./exetime_per_weight.sh ./fork16 ../trial"
    exit
fi
# trap ctrl-c and call ctrl_c()
trap ctrl_c SIGINT

function ctrl_c() {
    echo "** Trapped CTRL-C"
    killall ./fork16
    echo "killed all ./fork16"
    exit
}
./fork16&
sleep 1
echo "Executed 16 inf loops!"
echo "Prime factorize 179424691 from 1 to 15 by step 2"
for i in {1..15..2}
do
    echo "called weight $i"
    ./trial 179424691 $i&
done

killall ./fork16
