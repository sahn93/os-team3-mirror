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
./trial 179424691 1
./trial 179424691 3
./trial 179424691 5
./trial 179424691 7
./trial 179424691 9
./trial 179424691 11
./trial 179424691 13
./trial 179424691 15

echo "finish!"
killall ./fork16
