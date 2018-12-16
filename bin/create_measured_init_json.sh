#!/bin/bash

if [ -z "$1" ]; then
    echo -ne "set argument\n\t\$1: node number\n\t\$2: radius\n"
    exit
fi
if [ -z "$2" ]; then
    echo "set radius"
    exit
fi

node_number=$1
radius=$2

echo "-- node($node_number) radius($radius)"

ary_density=(100 200 500 1000 2000 5000 10000 20000)
# ary_density=(100 200 500)

dir=../density_json

if [ ! -d $dir ]; then
    echo "mkdir $dir"
    mkdir $dir
fi

if [ ! -d ${dir}/${node_number}node ]; then
    echo "mkdir ${dir}/${node_number}node"
    mkdir ${dir}/${node_number}node
fi

ary_density=(1 2 5 10 20 50 100)

for density in ${ary_density[@]}
do
    printf "density %6d[nodes/km^2]  "  $density
    area=`echo "scale=2;$node_number / $density" | bc`
    printf "area %8.2f[km^2]  " $area
    length=`echo "scale=2;sqrt($area) * 1000" | bc`
    printf "length %8.2f[m]\n" $length

    # python3 create_init_json.py $node_number $area $radius
    # echo "mv ${node_number}node.json ${dir}/${node_number}node/n${node_number}_d${density}_r${radius}.json"
    # mv ${node_number}node.json ${dir}/${node_number}node/n${node_number}_d${density}_r${radius}.json
    # echo "create density($density) size($length)"

done
