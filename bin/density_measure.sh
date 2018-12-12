#!/bin/bash

if [ -z "$1" ]
then
    echo "set argument"
    echo -ne "\t\$1: node number\n"
    echo -ne "\t\$2: algorithm\n"
    exit
fi

if [ -z "$2" ]
then
    echo "set argument"
    echo -ne "\t\$1: node number\n"
    echo -ne "\t\$2: algorithm\n"
    exit
fi
node_number=$1
algorithm=$2

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
echo "node_number neighbor_avg density update_count all_elapsed parse_elapsed update_elapsed search_elapsed send_elapsed all_avg_elapsed parse_avg_elapsed update_avg_elapsed search_avg_elapsed send_avg_elapsed" > $log


for file in ${files[@]}
do
    echo "-----------------------------------------------------------------------"
    echo "-- file $file"

    json=../density_json/${node_number}node/${file}
    lib_stdin_file=${json}.out
    for max_loop in ${ary_loop[@]}
    do
        echo "max loop(${max_loop})"
        lib_stdin_file=${json}.u${max_loop}.out
        if [ ! -f $lib_stdin_file ]; then
            python3 -u measure.py $json ${max_loop} 0 1 r > $lib_stdin_file &
        else
            cat -u $lib_stdin_file | ./location_info_base $json $algorithm | tee -a $log
        fi
    done

    echo ""
done
