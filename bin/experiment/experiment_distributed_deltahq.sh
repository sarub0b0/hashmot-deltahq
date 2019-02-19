#!/bin/bash


if [ -z "$1" ] \
    || [ -z "$2" ] \
    || [ -z "$3" ] \
    || [ -z "$4" ] \
    || [ -z "$5" ] \
    || [ -z "$6" ];
then
    echo "set argument"
    echo "  \$1: node number"
    echo "  \$2: machine id ( 0 ~ n )"
    echo "  \$3: l or L"
    echo "  \$4: exp loop number"
    echo "  \$5: ip addr"
    exit
fi

nodes=(100 200 500 1000 2000 5000 10000)

node_number=$1
machine_id=$2
numbering=$3
loop=$4
ipaddr=$5

proc_ary=(24 36 48 60 72)

for p in ${proc_ary[@]}
do
    for n in ${nodes[@]};
    do
        ./deltahq_distributed_density_measure.sh $node_number $p $machine_id $numbering $loop $ipaddr
    done
done
