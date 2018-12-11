import numpy as np
import matplotlib.pyplot as plt
import random

import json
import sys

f = open(sys.argv[1], 'r')
line = f.readline()
j = json.loads(line)

node = j["init"]["node"]

node = np.array(node)

x = []
y = []
for i in node:
    x.append(i['x'])
    y.append(i['y'])

x = np.random.randint(0, len(node), len(node))
y = np.random.randint(0, len(node), len(node))

plt.plot(x, y, '.')
plt.show()
