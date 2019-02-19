#!/bin/bash
json=$1
node_number=$2
process=$3
tmp_log=$4
id=$5
( python3 -u measure.py $json $node_number 0 1 r | ./location_info_base -a t $json | ./deltaHQ -t $process -i $id $json; ) 2>&1 | tee -a $tmp_log
