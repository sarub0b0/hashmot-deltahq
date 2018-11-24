#!/usr/bin/env python

import random
import time
import json
import pprint
import sys
import numpy as np

from signal import signal, SIGPIPE, SIG_DFL

if __name__ == '__main__':

    if len(sys.argv) < 3:
        print("arguments:")
        print("\t$1   filename")
        print("\t$2   delay")
        print("\t$3   max loop (default 300)")

        sys.exit(1)

    signal(SIGPIPE, SIG_DFL)

    # filename
    f = open(sys.argv[1], 'r')

    # delay
    delay = 0
    if 0 < float(sys.argv[2]):
        delay = float(sys.argv[2])

    line = f.readline()
    j = json.loads(line)

    node = j["init"]["node"]

    node = np.array(node)

    loop_count = 0

    max_loop = 300
    if 4 == len(sys.argv):
        max_loop = int(sys.argv[3])


    idx = 0
    while True:

        idx = 0

        x = node[idx]['x']
        y = node[idx]['y']
        r = node[idx]['radius']

        if max_loop < x:
            sys.stdout.write("{\"finish\":\"finish\"}\n")
            sys.stdout.flush()
            sys.exit(1)
            x = 1

        x = x + 1

        json_update = {}
        json_update['id'] = idx
        json_update['x'] = x
        json_update['y'] = y
        json_update['r'] = r

        node[idx]['x'] = x
        node[idx]['y'] = y


        update = {'update': {}}
        update['update']['node'] = json_update

        sys.stdout.write(json.dumps(update, separators=(',' ':')))
        sys.stdout.write("\n")
        sys.stdout.flush()

        loop_count += 1

        time.sleep(delay)

