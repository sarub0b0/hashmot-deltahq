python ../mobility.py 20node.json | python ../deltaHQ_lsh.py 50 | ./deltaHQ env.xml -t 4 -a 127.255.255.255 -p 12345 
