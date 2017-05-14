#!/bin/bash
if [ $# -eq 0 ]
then
    echo "usage: ./exetime_per_procnum.sh ./fork16 ./trial"
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

for i in {1..4}
do
    echo "./trial with ./fork16 $i times..."
    # ./fork16&
    ./fork16& 
    sleep 3
    # ./trial
# 1400305337
#    ./trial 1400305337 10
#    ./trial 776531401 10
    ./trial 275604541 10
done

echo "finish!"
killall ./fork16
