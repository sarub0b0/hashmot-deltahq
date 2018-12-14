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
# ary_coe=("0.1" "0.4" "1.0")

for c in ${ary_coe[@]}
do
    ary_loop+="`echo "${node_number}*${c}" | bc | sed "s/\.0//"` "
done

echo ${ary_loop[@]}

json_dir="../density_json/${node_number}node/"
# if [ -d $json_dir ]
# then
#     files=("`ls $json_dir | grep --color=never -v out`")
# else
#     echo "create json file. Use create_measured_init_json.sh"
#     exit
# fi


log="../density_json/${node_number}.log"
touch $log
echo "node_number neighbor_avg density update_count all_elapsed parse_elapsed update_elapsed search_elapsed send_elapsed all_avg_elapsed parse_avg_elapsed update_avg_elapsed search_avg_elapsed send_avg_elapsed" > $log

loop=10

for i in `seq 1 $loop`;
do
    ./create_measured_init_json.sh $node_number 160
    files=("`ls $json_dir | grep --color=never -v out | grep -v --color=never tmplog | grep -v --color=never avg`")

    for file in ${files[@]}
    do
        echo "-----------------------------------------------------------------------"
        echo "-- file $file"

        json=../density_json/${node_number}node/${file}
        lib_stdin_file=${json}.out
        for max_loop in ${ary_loop[@]}
        do
            tmp_log=${json}.u${max_loop}.tmplog
            echo "max loop(${max_loop})"
            lib_stdin_file=${json}.u${max_loop}.out
            # if [ ! -f $lib_stdin_file ]; then
            # touch ${log}.tmp
            for i in `seq 1 $loop`;
            do
                python3 -u measure.py $json ${max_loop} 0 1 r > $lib_stdin_file
                cat -u $lib_stdin_file | ./location_info_base $json $algorithm | tee -a $tmp_log
            done

            # cat ${tmp_log} | grep -E --color=never '^[0-9]' | awk '{OFMT="%.9f"}{a+=$1;b+=$2;c+=$3;d+=$4;e+=$5;f+=$6;g+=$7; h+=$8;i+=$9;j+=$10;k+=$11;l+=$12;m+=$13;n+=$14;} END {print a/NR,b/NR,c/NR,d/NR,e/NR,f/NR,g/NR,h/NR,i/NR,j/NR,k/NR,l/NR,m/NR,n/NR}' | tee -a ${tmp_log}.avg

            # fi
        done

        echo ""
    done
done

for f in `ls ${json_dir} | grep --color=never tmplog`;
do
        cat ${json_dir}/${f} | grep -E --color=never '^[0-9]' | awk '{OFMT="%.9f"}{a+=$1;b+=$2;c+=$3;d+=$4;e+=$5;f+=$6;g+=$7;h+=$8;i+=$9;j+=$10;k+=$11;l+=$12;m+=$13;n+=$14;o+=$15;p+=$15} END {printf "%d %d %.2f %.2f %.2f %d %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f\n", a/NR,b/NR,c/NR,d/NR,e/NR,f/NR,g/NR,h/NR,i/NR,j/NR,k/NR,l/NR,m/NR,n/NR,o/NR,p/NR}' | tee -a ${log}

        rm ${json_dir}/${f}
done
