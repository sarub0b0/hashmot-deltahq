#!/usr/bin/env python3
import random
import time
import json
#  import pprint
import sys
import numpy as np
from datetime import datetime

from signal import signal, SIGPIPE, SIG_DFL

if __name__ == '__main__':

    if len(sys.argv) < 6:
        print("arguments:")
        print("\t$1   filename")
        print("\t$2   max loop")
        print("\t$3   delay")
        print("\t$4   node speed")
        print("\t$5   update random or linear[r, l]")
        #  print("\t$6   single or multi line [s, m]")

        sys.exit(1)

    signal(SIGPIPE, SIG_DFL)

    # filename
    f = open(sys.argv[1], 'r')

    # max loop
    if 0 < int(sys.argv[2]):
        loop_max = int(sys.argv[2])
    else:
        loop_max = 0
        sys.exit(1)

    # delay
    delay = 0
    if 0 < float(sys.argv[3]):
        delay = float(sys.argv[3])

    # node speed
    speed = int(sys.argv[4])

    # update random or linear
    is_random = False
    if sys.argv[5] == "r":
        is_random = True
    else:
        is_random = False

    line = f.readline()
    j = json.loads(line)

    json_node = j["init"]["node"]

    #  print('json_node', type(json_node))

    node = np.array(json_node)

    now = datetime.now()
    now_ts = now.timestamp()
    random.seed(0)
    np.random.seed(0)

    mt = 0

    loop_count = 0

    idx = 0
    json_list = []
    node_len = len(node)

    update_node = node

    if is_random:
        choice_nodes = np.random.choice(node, loop_max, replace=False)
    else:
        choice_nodes = node

    #  print(choice_nodes)

    #  while node_len - loop_max - 1 != len(node):
    for cnode in choice_nodes:
        if 0 < loop_max:
            if loop_max <= loop_count:
                break

        t0 = time.perf_counter()

        name = cnode['name']
        identifier = int(name[4:len(name)])
        x = cnode['x']
        y = cnode['y']
        r = cnode['radius']

        x = x + speed

        json_update = {}
        json_update['id'] = identifier
        json_update['x'] = round(x, 2)
        json_update['y'] = round(y, 2)
        json_update['r'] = r

        update_node[idx]['x'] = x
        update_node[idx]['y'] = y



        update = {'update': {}}
        update['update']['node'] = json_update

        json_list.append(json.dumps(update, separators=(',' ':')) + '\n')

        loop_count += 1

        t1 = time.perf_counter()

        mt = mt + (t1 - t0)


        time.sleep(delay)
        #  sys.exit()

        #  break

    #  print(update_node)

    json_list.append('{"finish":"finish"}\n')
    mt = 0
    str_array = np.array(json_list)
    sys.stderr.write("-- mobility stand-by\n")
    input()
    t0 = time.perf_counter()
    for j in json_list:

        sys.stdout.write(j)
        #  sys.stdout.write('\n')
        sys.stdout.flush()


        #  sys.stderr.write("print elapsed: %f\n" % (t1 - t0))

    t1 = time.perf_counter()
    mt = t1 - t0
    sys.stderr.write(
        "-- mobility elapsed: %f (%f)\n" % (mt, mt / len(json_list)))
