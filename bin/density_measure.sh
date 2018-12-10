#!/bin/bash

if [ -z "$1" ]
then
    echo "set argument"
    echo -ne "\t\$1: node number\n"
    exit
fi

node_number=$1

ary_loop=()

ary_coe=("0.1" "0.2" "0.4" "0.6" "0.8" "1.0")

echo ${ary_coe[@]}

for c in ${ary_coe[@]}
do
    echo $c
    ary_loop+="`echo "${node_number}*${c}" | bc | sed "s/\.0//"` "
done

echo ${ary_loop[@]}

files=("`ls ../density_json/${node_number}node/`")

log="../density_json/${node_number}.log"
touch $log
echo "" > $log

for file in ${files[@]}
do
    echo "-----------------------------------------------------------------------"
    echo "-- file $file"

    for max_loop in ${ary_loop[@]}
    do
        echo "max loop(${max_loop})"
        json=../density_json/${node_number}node/${file}
        python3 -u measure.py $json ${max_loop} 0 1 r | ./location_info_base $json t 2>&1 | tee -a $log
    done

    echo ""
done
