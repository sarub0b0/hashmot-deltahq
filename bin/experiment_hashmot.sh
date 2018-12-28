#!/bin/bash

nodes=(100 200 500 1000 2000 5000 10000 20000 50000 100000)
# nodes=(100 200 500 1000 2000 5000 10000)
# nodes=(100000)

# if [ ! -d ../density_json ]; then
#     for n in ${nodes[@]};
#     do
#         ./create_measured_init_json.sh $n 160

#     done
# fi

if [ -z "$1" ]; then
    echo "set argument"
    echo -ne "  \$1 algorithm\n"
    echo -ne "  \$2 loop number\n"
    exit
fi
if [ -z "$2" ]; then
    echo "set argument"
    echo -ne "  \$1 algorithm\n"
    echo -ne "  \$2 loop number\n"
    exit
fi

algorithm=$1
loop=$2

for n in ${nodes[@]};
do
    ./hashmot_density_measure.sh $n $algorithm $loop
done

