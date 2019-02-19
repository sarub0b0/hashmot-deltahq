#!/bin/bash


if [ -z "$1" ] || [ -z "$2" ];
then
    echo "set argument"
    echo "  \$1: type d or p (d:distributed p:parallel)"
    echo "  \$2: exp loop number"
    exit
fi

exec_type=$1
loop=$2

nodes=(100 200 500 1000 2000 5000 10000)

for n in ${nodes[@]};
do
    ./hashmot_deltahq_measure.sh $n $loop $exec_type
done



