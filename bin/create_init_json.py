#!/usr/bin/env python
import random
import json
import sys

if len(sys.argv) < 4:
    print("arg: node_number, max_number(x, y), radius\n")

node_num = int(sys.argv[1])

root = json.loads('{}')
root['init'] = {}
root['init']['node'] = []
root['init']['connection'] = []

max_num = int(sys.argv[2])
radius = int(sys.argv[3])

for i in range(node_num):
    root['init']['node'].append({
        'name': 'node00' + str(i),
        'type': 'regular',
        'connection': 'infrastructure',
        'adapter': 'orinoco',
        'x': round(random.uniform(0, max_num), 2),
        'y': round(random.uniform(0, max_num), 2),
        'z': round(random.uniform(0, max_num), 2),
        'Pt': 20,
        'internal_delay': 1,
        'radius': radius,
    })

    print(root["init"]["node"][-1]['x'], root["init"]["node"][-1]['y'])

    root['init']['connection'].append({
        'from_node': 'node00' + str(i),
        'to_node': 'auto_connect',
        'through_environment': 'env_outdoor',
        'standard': '802.11b',
        'packet_size': 1024,
        'consider_interference': False
    })


fd = open(str(node_num) + "node.json", 'w')

out = json.dumps(root, separators=(',' ':'))

out = out + "\n"

fd.write(out)
