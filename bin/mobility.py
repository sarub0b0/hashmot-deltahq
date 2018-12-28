#!/usr/bin/env python

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
        sys.stderr.write("arguments:\n")
        sys.stderr.write("\t$1   filename\n")
        sys.stderr.write("\t$2   max loop\n")
        sys.stderr.write("\t$3   delay\n")
        sys.stderr.write("\t$4   node speed\n")
        sys.stderr.write("\t$5   update random or linear[r, l]\n")
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

    # single or multi line
    #  is_single = False
    #  if sys.argv[6] == "s":
    #      is_single = True
    #  else:
    #      is_single = False


    #  if is_single:
    line = f.readline()
    j = json.loads(line)
    #  sys.stdout.write(json.dumps(j, separators=(',' ':')))
    #  sys.stdout.write("\n")
    #  sys.stdout.flush()

    node = j["init"]["node"]


    #  else:
    #      node = json.loads("[]")
    #      line = f.readline()
    #      while line:
    #          j = json.loads(line)

    #          init = j['init']
    #          if 'node' in init:
    #              n = init['node']
    #              node.append(n)


    #          #  sys.stdout.write(json.dumps(j, separators=(',' ':')))
    #          #  sys.stdout.write("\n")
    #          #  sys.stdout.flush()
    #          line = f.readline()

    node = np.array(node)

    #  time.sleep(0.2)
    #  sys.exit()

    #  print(json.dumps(node))
    #  print("node length: %d" % len(node))


    #  print("Enter Start")

    now = datetime.now()
    now_ts = now.timestamp()
    random.seed(now_ts)

    mt = 0

    loop_count = 0

    idx = 0
    json_list = []
    while True:
        if 0 < loop_max:
            if loop_max <= loop_count:
                #  sys.stdout.write('{"finish":"finish"}')
                #  sys.stdout.write("\n")
                #  sys.stdout.flush()
                #  time.sleep(2)
                json_list.append('{"finish":"finish"}\n')
                break



        t0 = time.perf_counter()

        if is_random:
            idx = random.randint(0, len(node) - 1)
        else:
            idx = idx % len(node)

        x = node[idx]['x']
        y = node[idx]['y']
        r = node[idx]['radius']

        x = round(random.uniform(x - speed, x + speed), 1)
        y = round(random.uniform(y - speed, y + speed), 1)

        #  x = x + 1

        json_update = {}
        json_update['id'] = idx
        json_update['x'] = x
        json_update['y'] = y
        json_update['r'] = r

        node[idx]['x'] = x
        node[idx]['y'] = y

        #  update = {'update': []}
        #  update['update'].append(json_update)

        update = {'update': {}}
        update['update']['node'] = json_update

        #  idx = 1
        #  x = json_init['init']['node'][idx]['x']
        #  y = json_init['init']['node'][idx]['y']

        #  speed = 10
        #  json_update = {}
        #  json_update['id'] = idx
        #  json_update['x'] = round(random.uniform(x - speed, y + speed), 1)
        #  json_update['y'] = round(random.uniform(y - speed, y + speed), 1)

        #  update['update'].append(json_update)



        #  print(json.dumps(update))

        #  sys.stdout.write(json.dumps(update, separators=(',' ':')))
        #  sys.stdout.write("\n")
        #  sys.stdout.flush()

        json_list.append(json.dumps(update, separators=(',' ':')))

        loop_count += 1

        #  sys.exit()

        t1 = time.perf_counter()

        mt = mt + (t1 - t0)

        #  sys.stderr.write("%04d elapsed: %f\n" % (idx, (t1 - t0)))

        idx = idx + 1

        time.sleep(delay)
        #  sys.exit()

        #  break

    mt = 0
    str_array = np.array(json_list)
    sys.stderr.write("-- mobility stand-by\n")
    input()
    for j in json_list:
        t0 = time.perf_counter()

        sys.stdout.write(j)
        sys.stdout.write("\n")
        sys.stdout.flush()

        t1 = time.perf_counter()
        mt = mt + (t1 - t0)

        #  sys.stderr.write("print elapsed: %f\n" % (t1 - t0))

    sys.stderr.write("-- mobility elapsed: %f (%f)\n" % (mt, mt / len(json_list)))
