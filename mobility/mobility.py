import random
import time
import json
import pprint
import sys
from signal import signal, SIGPIPE, SIG_DFL

if __name__ == '__main__':

    signal(SIGPIPE, SIG_DFL)

    f = open(sys.argv[1], 'r')
    json_init = json.load(f)


    sys.stdout.write(json.dumps(json_init))
    sys.stdout.write("\n")
    sys.stdout.flush()

    #  sys.exit()

    time.sleep(1)

    loop_count = 0
    while True:
        #  if 3 < loop_count:
        #      sys.stdout.write('{"finish":"finish"}')
        #      sys.stdout.write("\n")
        #      sys.stdout.flush()
        #      sys.exit(1)

        idx = random.randint(0, len(json_init['init']['node']) - 1)

        speed = 20

        x = json_init['init']['node'][idx]['x']
        y = json_init['init']['node'][idx]['y']

        x = round(random.uniform(x - speed, y + speed), 1)
        y = round(random.uniform(y - speed, y + speed), 1)


        json_update = {}
        json_update['id'] = idx
        json_update['x'] = x
        json_update['y'] = y

        json_init['init']['node'][idx]['x'] = x
        json_init['init']['node'][idx]['y'] = y

        update = {'update': []}
        update['update'].append(json_update)

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

        sys.stdout.write(json.dumps(update))
        sys.stdout.write("\n")
        sys.stdout.flush()

        loop_count += 1

        #  sys.exit()

        time.sleep(0.1)
        #  sys.exit()

        #  break
