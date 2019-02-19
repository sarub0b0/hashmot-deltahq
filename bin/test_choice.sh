#!/bin/bash

node_number=10
loop=5

choice_id=(`python -c "import numpy as np; print(np.random.choice(range(0, $node_number), $loop, replace=False))" | sed "s/\(\[\)\(.*\)\(\]\)/\2/g"`)



echo ${choice_id[@]}

for i in ${choice_id[@]}
do
    echo $i
done
