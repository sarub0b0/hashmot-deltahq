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

plt.plot(x, y, '.')
plt.show()
