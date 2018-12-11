#!/usr/bin/env python
#  import random
import json
import sys
import numpy as np


def write_file(fd, root):
    out = json.dumps(root, separators=(',' ':'))
    out = out + "\n"
    fd.write(out)


if len(sys.argv) < 4:
    print("arg: $1 $2 $3 $4")
    print("\t$1: node_number")
    print("\t$2: max_number(x, y), radius")
    print("\t$3: radius")
    #  print("\t$4: single or multi line")
    sys.exit(1)

node_num = int(sys.argv[1])

max_num = float(sys.argv[2])
radius = int(sys.argv[3])

fd = open(str(node_num) + "node.json", 'w')


#  if sys.argv[4] == "m":

#      for i in range(node_num):
#          root = json.loads('{}')
#          root['init'] = {}
#          #  root['init']['node'] = {}

#          #  root['init']['node'].append({
#          root['init']['node'] = {
#              'name': 'node' + str(i),
#              'type': 'regular',
#              'connection': 'infrastructure',
#              'adapter': 'orinoco',
#              'x': round(random.uniform(0, max_num), 2),
#              'y': round(random.uniform(0, max_num), 2),
#              'z': 0,
#              #  'z': round(random.uniform(0, max_num), 2),
#              'Pt': 20,
#              'internal_delay': 1,
#              'radius': radius,
#          }

#              #  print(root["init"]["node"][-1]['x'], root["init"]["node"][-1]['y'])
#          write_file(fd, root)

#      root = json.loads('{}')
#      root['init'] = {}

#      root['init']['environment'] = {
#                  'name': 'env_outdoor',
#                  'is_dynamic': False,
#                  'alpha': 3.2,
#                  'sigma': 0.0,
#                  'W': 0.0,
#                  'noise_power': -100.0
#              }

#      write_file(fd, root)

#      for i in range(node_num):
#          root = json.loads('{}')
#          root['init'] = {}
#          #  root['init']['node'] = []
#          #  root['init']['connection'] = []

#          #  root['init']['connection'].append({
#          root['init']['connection'] = {
#              'from_node': 'node' + str(i),
#              'to_node': 'auto_connect',
#              'through_environment': 'env_outdoor',
#              'standard': '802.11b',
#              'packet_size': 1024,
#              'consider_interference': False
#          }

#          write_file(fd, root)
#  else:
root = json.loads('{}')
root['init'] = {}
root['init']['node'] = []
root['init']['environment'] = []
root['init']['connection'] = []

#  pos_x = np.random.randint(0, max_num, node_num)
#  pos_y = np.random.randint(0, max_num, node_num)

rand_x = np.random.uniform(0, max_num, node_num)
rand_y = np.random.uniform(0, max_num, node_num)

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
