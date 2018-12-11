#!/bin/bash

nodes=(100 200 500 1000 2000 5000 10000 20000 50000 100000)

for n in ${nodes[@]};
do
    ./create_measured_init_json.sh $n 160

done

exit


for n in ${nodes[@]};
do
    ./density_measure.sh $n t
    cp -r ../density_json/ ../tree_density
done

for n in ${nodes[@]};
do
    ./density_measure.sh $n h
    cp -r ../density_json/ ../hash_density
done

for n in ${nodes[@]};
do
    ./density_measure.sh $n l
    cp -r ../density_json/ ../linear_density
done
