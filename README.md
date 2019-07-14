# Usage
Hashmot and deltaHQ are a wireless network simulator (L0, L1, L2, L3) using network emulator.


## Require

- clang >= 6.0

- packages:
    - RapidJSON >= v1.1.0
    - Jansson >= 2.11
    - Expat >= 1.0


## Compile

```
$ cd hashmot-dir or deltaHQ-dir
$ mkdir build
$ cd build
$ cmake ..
```

## Run

### Execution order

1. deltaHQ
2. hashmot
3. mobility-generator


- deltaHQ

```
$ ./deltaHQ scenario.json \
    -t ${num of thread}       \
    -i ${own id}              \
    -L ${listen port}         \
    -a ${emulator ip address} \
    -p ${emulator listen port}
```

- hashmot

```
$ ./hashmot scenario.json \
    -a ${algorithm}           \
    -L ${listen port}         \
    -A ${deltaHQ ip address}  \
    -p ${deltaHQ listen port}

algorithm:
    h: lsh
    t: kd-tree
    l: linear

```

- mobility-generator

```
$ ./mobility scenario.json \
    ${loop count}              \
    ${exec delay}              \
    ${node speed}              \
    ${node update (r: random, l: linear)}
```

### Example
```
$ ./create_init_json.py 100 0.01 160
# $1: number of node
# $2: area [km^2]
# $3: radius of communication area [m]

==== Machine A ====
$ ./deltaHQ 10node.json -t 8 -i 0 -L 10000 

==== Machine B ====
$ ./mobility 10node.json 10 0 1 l |\
    ./hashmot 10node.json -a t -A 192.168.1.255 -p 10000

====================
When deltaHQ and hashmot initialized, please input return key!
```





