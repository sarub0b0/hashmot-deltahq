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
    echo "  \$2: process number"
    echo "  \$3: machine id ( 0 ~ n )"
    echo "  \$4: l or L"
    echo "  \$5: loop number"
    echo "  \$6: ip addr"
    exit
fi

scenario_dir=../deltahq_distributed_json

if [ ! -d $scenario_dir ];
then
    mkdir $scenario_dir
fi

ipaddr=$6
port=10000

loop=$5

node_number=$1
process_number=$2
machine_id=$3

numbering=$4

machine_number=$(($node_number / $process_number))
machine_number_mod=$(($node_number % $process_number))
echo "id(${machine_id}) machine number(${machine_number}, mod($machine_number_mod))"

if [ $machine_id -ge $machine_number ];
then
    echo "-- Error too big machine_id"
    exit
fi

own_ids=()
if [ "$numbering" = "l" ];
then
    own_ids=()

    mmod=$machine_number_mod
    idx=$(($machine_id * $process_number))

    if [ $mmod -le $machine_id ]
    then
        idx=$(($idx + $mmod))
        mmod=0
        own_ids+=($idx)
    else
        if [ $machine_id -ne 0 ];
        then
            idx=$(($idx + 1))
        fi
        own_ids+=($idx)
    fi

    for i in `seq 1 $(($process_number - 1))`;
    do
        idx=$(($idx + 1))
        own_ids+=($idx)
    done

    if [ 0 -lt $mmod ]
    then
        idx=$(($idx + 1))
        own_ids+=($idx)
    fi

fi

if [ "$numbering" = "L" ];
then
    own_ids=($machine_id)
    mmod=$machine_number_mod

    if [ $mmod -le $machine_id ]
    then
        mmod=0
    fi

    for i in `seq 1 $(($process_number - 1))`;
    do
        tail_id=${own_ids[$((${#own_ids[@]} - 1))]}
        id=$(($tail_id + $machine_number))
        own_ids+=($id)
    done

    if [ 0 -lt $mmod ]
    then
        tail_id=${own_ids[$((${#own_ids[@]} - 1))]}
        id=$(($tail_id + $machine_number))
        own_ids+=($id)
    fi

fi

echo ${own_ids[@]}

node_dir=$scenario_dir/${node_number}node
logname="$scenario_dir/${node_number}.log"

log_dir=$scenario_dir/log

if [ ! -d $log_dir ]
then
    mkdir $log_dir
fi

touch ${logname}

echo -n > $logname

rm $node_dir/*.tmplog
rm $node_dir/*.avglog

for i in `seq 1 $loop`;
do

    # =======================================================================
    # 実験シナリオ生成
    # =======================================================================
    ./create_measured_init_json.sh $node_number 160 $scenario_dir
    files=("`ls $node_dir | grep --color=never -v libin | grep -v --color=never tmplog | grep -v --color=never avg`")

    # =======================================================================
    # 各シナリオ実行
    # =======================================================================
    for file in ${files[@]};
    do
        scenario=${node_dir}/$file
        lib_input_file=${scenario}.p${process_number}.libin

        echo "-----------------------------------------------------------------------"
        echo "-- file $file"

        for j in `seq 1 $loop`
        do
            python3 -u measure.py $scenario $node_number 0 1 r > $lib_input_file 2>/dev/null
            for idx in ${own_ids[@]};
            do
                tmplog=${scenario}.p${process_number}.id${idx}.tmplog

                sh -c "./deltaHQ -t 1 -i $idx $scenario -L $port >> $tmplog 2>/dev/null" &
            done

            sleep 1
            ./location_info_base -A $ipaddr -p $port $scenario -a t < $lib_input_file 2>/dev/null
        done

    done

done

for f in `ls ${node_dir} | grep --color=never tmplog`;
do
    avglog=`echo ${f} | sed 's/tmplog/avglog/g'`
    cat ${node_dir}/${f} | grep -E --color=never '^[0-9]' | awk -f deltahq_measure_average.awk | tee -a ${node_dir}/${avglog}
done
