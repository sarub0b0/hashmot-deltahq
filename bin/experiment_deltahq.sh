#!/bin/bash


if [ -z "$1" ] || [ -z "$2" ];
then
    echo "set argument"
    echo "  \$1: type d or p (d:distributed p:parallel)"
    echo "  \$2: exp loop number"
    exit
fi

nodes=(10000)


exec_type=$1
loop=$2


for n in ${nodes[@]};
do
    ./deltahq_density_measure.sh $n $loop $exec_type
done
