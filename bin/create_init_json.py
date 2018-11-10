#!/usr/bin/env python
import random
import json
import sys

def write_file(fd, root):
    out = json.dumps(root, separators=(',' ':'))
    out = out + "\n"
    fd.write(out)


if len(sys.argv) < 4:
    print("arg: node_number, max_number(x, y), radius\n")
    sys.exit(1)

node_num = int(sys.argv[1])

#  root = json.loads('{}')
#  root['init'] = {}
#  root['init']['node'] = []
#  root['init']['connection'] = []

max_num = int(sys.argv[2])
radius = int(sys.argv[3])

fd = open(str(node_num) + "node.json", 'w')

for i in range(node_num):
    root = json.loads('{}')
    root['init'] = {}
    #  root['init']['node'] = {}

    #  root['init']['node'].append({
    root['init']['node'] = {
        'name': 'node' + str(i),
        'type': 'regular',
        'connection': 'infrastructure',
        'adapter': 'orinoco',
        'x': round(random.uniform(0, max_num), 2),
        'y': round(random.uniform(0, max_num), 2),
        'z': round(random.uniform(0, max_num), 2),
        'Pt': 20,
        'internal_delay': 1,
        'radius': radius,
    }

        #  print(root["init"]["node"][-1]['x'], root["init"]["node"][-1]['y'])
    write_file(fd, root)

root = json.loads('{}')
root['init'] = {}

root['init']['environment'] = {
            'name': 'env_outdoor',
            'is_dynamic': False,
            'alpha': 3.2,
            'sigma': 0.0,
            'W': 0.0,
            'noise_power': -100.0
        }

write_file(fd, root)

for i in range(node_num):
    root = json.loads('{}')
    root['init'] = {}
    #  root['init']['node'] = []
    #  root['init']['connection'] = []

    #  root['init']['connection'].append({
    root['init']['connection'] = {
        'from_node': 'node' + str(i),
        'to_node': 'auto_connect',
        'through_environment': 'env_outdoor',
        'standard': '802.11b',
        'packet_size': 1024,
        'consider_interference': False
    }

    write_file(fd, root)


