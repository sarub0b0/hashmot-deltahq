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

for c in ${ary_coe[@]}
do
    ary_loop+="`echo "${node_number}*${c}" | bc | sed "s/\.0//"` "
done

echo ${ary_loop[@]}

json_dir="../density_json/${node_number}node/"
if [ -d $json_dir ]
then
    files=("`ls $json_dir | grep --color=never -v out`")
else
    echo "create json file. Use create_measured_init_json.sh"
    exit
fi


log="../density_json/${node_number}.log"
touch $log
echo "" > $log

for file in ${files[@]}
do
    echo "-----------------------------------------------------------------------"
    echo "-- file $file"

    json=../density_json/${node_number}node/${file}
    lib_stdin_file=${json}.out
    for max_loop in ${ary_loop[@]}
    do
        echo "max loop(${max_loop})"
        python3 -u measure.py $json ${max_loop} 0 1 r > $lib_stdin_file
        cat $lib_stdin_file | ./location_info_base $json t 2>&1 | tee -a $log
    done

    echo ""
done
