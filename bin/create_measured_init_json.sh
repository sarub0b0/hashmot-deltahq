#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo -ne "set argument\n\t\$1: node number\n\t\$2: radius\n\t\$3: json dir\n"
    exit
fi

node_number=$1
radius=$2
dir=$3

echo "-- node($node_number) radius($radius)"

ary_density=(100 200 500 1000 2000 5000 10000)
# ary_density=(20000)


if [ ! -d $dir ]; then
    echo "mkdir $dir"
    mkdir $dir
fi

if [ ! -d ${dir}/${node_number}node ]; then
    echo "mkdir ${dir}/${node_number}node"
    mkdir ${dir}/${node_number}node
fi

# ary_density=(1 2 5 10 20 50 100 200)

for density in ${ary_density[@]}
do
    printf "density %6d[nodes/km^2]  "  $density
    area=`echo "scale=4;$node_number / $density" | bc`
    printf "area %8.4f[km^2]  " $area
    length=`echo "scale=4;sqrt($area) * 1000" | bc`
    printf "length %8.4f[m]\n" $length

    if [ "Darwin" == "$(uname)" ]; then
        python3 ${HOME}/work/hashmot/bin/create_init_json.py $node_number $area $radius
    else
        python3 ${HOME}/hashmot/bin/create_init_json.py $node_number $area $radius
    fi
    echo "mv ${node_number}node.json ${dir}/${node_number}node/n${node_number}_d${density}_r${radius}.json"
    mv ${node_number}node.json ${dir}/${node_number}node/n${node_number}_d${density}_r${radius}.json
    echo "create density($density) size($length)"

done
