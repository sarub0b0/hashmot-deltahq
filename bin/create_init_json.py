#!/usr/bin/env python
#  import random
import json
import sys
import numpy as np
import math


def write_file(fd, root):
    out = json.dumps(root, separators=(',' ':'))
    out = out + "\n"
    fd.write(out)


if len(sys.argv) < 4:
    print("arg: $1 $2 $3 $4")
    print("\t$1: node_number")
    print("\t$2: area")
    print("\t$3: radius")
    sys.exit(1)

node_num = int(sys.argv[1])
area = float(sys.argv[2])
radius = int(sys.argv[3])
length = math.sqrt(area) * 1000

fd = open(str(node_num) + "node.json", 'w')

root = json.loads('{}')
root['init'] = {}
root['init']['node'] = []
root['init']['environment'] = []
root['init']['connection'] = []
root['init']['field'] = {'width': length, 'height': length, "area": area}


rand_x = np.random.uniform(0, length, node_num)
rand_y = np.random.uniform(0, length, node_num)

for i in range(node_num):
    root['init']['node'].append({
        'name': 'node' + str(i),
        'type': 'regular',
        'connection': 'infrastructure',
        'adapter': 'orinoco',
        #  'x': int(pos_x[i]),
        #  'y': int(pos_y[i]),
        'x': round(rand_x[i], 2),
        'y': round(rand_y[i], 2),
        #  'x': round(random.uniform(0, max_num), 2),
        #  'y': round(random.uniform(0, max_num), 2),
        #  'z': round(random.uniform(0, max_num), 2),
        'z': 0,
        'Pt': 20,
        'internal_delay': 1,
        'radius': radius,
    })

    root['init']['connection'].append({
        'from_node': 'node' + str(i),
        'to_node': 'auto_connect',
        'through_environment': 'env_outdoor',
        'standard': '802.11b',
        'packet_size': 1024,
        'consider_interference': False
    })


root['init']['environment'].append({
            'name': 'env_outdoor',
            'is_dynamic': False,
            'alpha': 3.2,
            'sigma': 0.0,
            'W': 0.0,
            'noise_power': -100.0
        })

write_file(fd, root)
#  fd.write("{\"init\":\"finish\"}\n")
#  fd.write("{\"finish\":\"finish\"}\n")
fd.close()
