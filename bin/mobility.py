#!/usr/bin/env python

import random
import time
import json
import pprint
import sys
import numpy as np

from signal import signal, SIGPIPE, SIG_DFL

if __name__ == '__main__':

    if len(sys.argv) < 5:
        print("arguments:")
        print("\t$1   filename")
        print("\t$2   max loop")
        print("\t$3   delay")
        print("\t$4   single or multi line [s, m]")

        sys.exit(1)

    signal(SIGPIPE, SIG_DFL)



    f = open(sys.argv[1], 'r')

    is_single = False
    if sys.argv[4] == "s":
        is_single = True
    else:
        is_single = False


    if is_single:
        line = f.readline()
        j = json.loads(line)
        sys.stdout.write(json.dumps(j, separators=(',' ':')))
        sys.stdout.write("\n")
        sys.stdout.flush()

        node = j["init"]["node"]

        line = f.readline()
        j = json.loads(line)
        sys.stdout.write(json.dumps(j, separators=(',' ':')))
        sys.stdout.write("\n")
        sys.stdout.flush()


    else:
        node = json.loads("[]")
        line = f.readline()
        while line:
            j = json.loads(line)

            init = j['init']
            if 'node' in init:
                n = init['node']
                node.append(n)


            sys.stdout.write(json.dumps(j, separators=(',' ':')))
            sys.stdout.write("\n")
            sys.stdout.flush()
            line = f.readline()

    node = np.array(node)

    #  sys.exit()

    #  print(json.dumps(node))
    #  print("node length: %d" % len(node))

    time.sleep(0.5)

    if 0 < int(sys.argv[2]):
        loop_max = int(sys.argv[2])
    else:
        loop_max = 0

    delay = 0
    if 0 < float(sys.argv[3]):
        delay = float(sys.argv[3])

    mt = 0
    loop_count = 0
    while True:
        if 0 < loop_max:
            if loop_max <= loop_count:
                sys.stdout.write('{"finish":"finish"}')
                sys.stdout.write("\n")
                sys.stdout.flush()
                time.sleep(2)

                sys.exit(1)

        t0 = time.perf_counter()

        idx = random.randint(0, len(node) - 1)

        speed = 1
        x = node[idx]['x']
        y = node[idx]['y']
        r = node[idx]['radius']

        x = round(random.uniform(x - speed, y + speed), 1)
        y = round(random.uniform(y - speed, y + speed), 1)


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

        sys.stdout.write(json.dumps(update, separators=(',' ':')))
        sys.stdout.write("\n")
        sys.stdout.flush()

        loop_count += 1

        #  sys.exit()

        t1 = time.perf_counter()

        mt = mt + (t1 - t0)

        #  print("%04d elapsed: %f" % (idx, (t1 - t0)))

        time.sleep(delay)
        #  sys.exit()

        #  break

    print("all elapsed: %f" % mt)
