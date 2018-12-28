#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ];
then
    echo "set argument"
    echo "  \$1: node number"
    echo "  \$2: exp loop number"
    echo "  \$3: type d or p (d:distributed p:parallel)"
    exit
fi

node_number=$1
loop=$2
exec_type=$3

exp_dir=../deltahq_json
node_dir=$exp_dir/${node_number}node

log_name="${exp_dir}/${node_number}.log"

touch ${log_name}

echo -n > $log_name

# =======================================================================
# id選出
# =======================================================================
choice_id=(`python3 -c "import numpy as np; print(np.random.choice(range(0, $node_number), $loop, replace=False))" | sed "s/\(\[\)\(.*\)\(\]\)/\2/g"`)


# =======================================================================
# 実験用シナリオ置き場
# =======================================================================
if [ ! -d $exp_dir ]
then
    mkdir $exp_dir
fi

rm ${node_dir}/*.tmplog

# =======================================================================
# 実験ループ
# =======================================================================
pproc_ary=(1 2 4 8 16 24)
dproc_ary=(1 2 4 8 16 24)
for i in `seq 1 $loop`;
do

    # =======================================================================
    # 実験シナリオ生成
    # =======================================================================
    ./create_measured_init_json.sh $node_number 160 $exp_dir
    files=("`ls $node_dir | grep --color=never -v out | grep -v --color=never tmplog | grep -v --color=never avg`")

    # =======================================================================
    # 各シナリオ実行
    # =======================================================================
    for file in ${files[@]};
    do
        json=${node_dir}/${file}

        echo "-----------------------------------------------------------------------"
        echo "-- file $file"

        # =======================================================================
        # 分散処理
        # =======================================================================
        if [ "${exec_type}" == "d" ]
        then
            for p in ${dproc_ary[@]};
            do
                id=${choice_id[$(($i - 1))]}
                tmp_log=${json}.dp${p}.tmplog
                lib_stdout_file=${json}.dp${p}.out
                # =======================================================================
                # 1ファイルごとに実行時間の平均とる
                # =======================================================================
                for j in `seq 1 $loop`;
                do
                    # =======================================================================
                    # mobility -> hashmot の結果をファイルに保存
                    # =======================================================================
                    python3 -u measure.py $json $node_number 0 1 r | ./location_info_base $json -a t > $lib_stdout_file

                    ./deltaHQ -t $p -i $id $json < $lib_stdout_file | tee -a $tmp_log
                done
            done
        fi
        # =======================================================================
        # 並列処理
        # =======================================================================
        if [ "${exec_type}" == "p" ]
        then
            for p in ${pproc_ary[@]};
            do
                # id=$((${node_number} / $p))
                tmp_log=${json}.pp${p}.tmplog
                lib_stdout_file=${json}.pp${p}.out
                # =======================================================================
                # 1ファイルごとに実行時間の平均とる
                # =======================================================================
                for j in `seq 1 $loop`;
                do
                    # =======================================================================
                    # mobility -> hashmot の結果をファイルに保存
                    # =======================================================================
                    python3 -u measure.py $json ${node_number} 0 1 r | ./location_info_base $json -a t > $lib_stdout_file

                    ./deltaHQ -t $p $json < $lib_stdout_file | tee -a $tmp_log
                done
            done
        fi
        echo ""
    done
done

for f in `ls ${node_dir} | grep --color=never tmplog`;
do
    cat ${node_dir}/${f} | grep -E --color=never '^[0-9]' | awk -f deltahq_measure_average.awk | tee -a ${log_name}
done
