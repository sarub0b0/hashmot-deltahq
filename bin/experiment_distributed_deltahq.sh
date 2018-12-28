#!/bin/bash


if [ -z "$1" ] || [ -z "$2" ];
then
    echo "set argument"
    echo "  \$1: type d or p (d:distributed p:parallel)"
    echo "  \$2: exp loop number"
    exit
fi

nodes=(100 200 500 1000 2000 5000 10000)


exec_type=$1
loop=$2

if [ -z "$1" ] \
    || [ -z "$2" ] \
    || [ -z "$3" ] \
    || [ -z "$4" ] \
    || [ -z "$5" ] \
    || [ -z "$6" ];
then
    echo "set argument"
    echo "  \$1: node number"
    echo "  \$2: process number"
    echo "  \$3: machine id ( 0 ~ n )"
    echo "  \$4: l or L"
    echo "  \$5: loop number"
    echo "  \$6: ip addr"
    exit
fi


for n in ${nodes[@]};
do
    ./deltahq_distributed_density_measure.sh $n $loop $exec_type
done
