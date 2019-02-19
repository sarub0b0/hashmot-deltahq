#!/usr/bin/env python3
import subprocess as sp
import sys
import os
import numpy as np
from multiprocessing import Process

scenario_dir = "../deltahq_distributed_json"


def run_deltahq(idx):
    command = ["./deltaHQ", "-t", "1", "-i", str(idx), "100node.json", "-L", "10000"]
    result = sp.Popen(command, stdout=sp.PIPE, stderr=sp.PIPE, shell=False)
    print(idx, result)
    for line in iter(result.stdout.readline, b''):
        #  print(line)
        print(idx, line.rstrip().decode("utf8"))

    #  for line in iter(result.stderr.readline, b''):
    #      print(line.rstrip().decode("utf8"))


def make_id_list(node_number, process_number, machine_id, numbering):
    machine_number = int(node_number / process_number)
    machine_number_mod = node_number % process_number

    print("id(%d) machine number(%d, mod=%d)" % (machine_id, machine_number, machine_number_mod))

    if machine_number <= machine_id:
        sys.stderr.write("-- Error too big machine id\n")
        return

    own_ids = []
    if numbering == "l":
        own_ids = []

        mmod = machine_number_mod
        idx = machine_id * process_number

        if mmod <= machine_id:
            idx = idx + mmod
            mmod = 0
            own_ids.append(idx)
        else:
            if machine_id != 0:
                idx = idx + 1
            own_ids.append(idx)

        for i in range(1, process_number):
            idx = idx + 1
            own_ids.append(idx)

        if 0 < mmod:
            idx = idx + 1
            own_ids.append(idx)

    if numbering == "L":
        own_ids.append(machine_id)
        mmod = machine_number_mod

        if mmod <= machine_id:
            mmod = 0
        for i in range(1, process_number):
            idx = own_ids[-1] + machine_number
            own_ids.append(idx)

        if 0 < mmod:
            idx = own_ids[-1] + machine_number
            own_ids.append(idx)

    return own_ids


def execute():
    pass


def run(node_number, process_number, machine_id, numbering):

    os.makedirs(scenario_dir, exist_ok=True)

    own_ids = np.array(make_id_list(node_number, process_number, machine_id, numbering))

    processes = []
    for idx in own_ids:
        processes.append(Process(target=run_deltahq, args=(idx,)))
        processes[-1].start()

    command = ["./measure.py", "100node.json", "100", "0", "1", "r"]
    p1 = sp.Popen(command, stdout=sp.PIPE, stderr=sp.PIPE)



    command = ["./location_info_base", "-A", "172.16.73.255", "-p", "10000", "100node.json", "-a", "t"]
    p2 = sp.Popen(command, stdin=p1.stdout, stdout=sp.PIPE, stderr=sp.PIPE)

    p1.stdout.close()

    output = p2.communicate()
    #  print(output)
    for line in output:
        print(line.rstrip().decode("utf8"))


    for proc in processes:
        proc.join()





if __name__ == '__main__':
    if len(sys.argv) < 5:
        sys.stderr.write("set argument\n")
        sys.stderr.write("  $1: node number\n")
        sys.stderr.write("  $2: process number\n")
        sys.stderr.write("  $3: machine id ( 0 ~ n )\n")
        sys.stderr.write("  $4: l or L\n")
        sys.exit(1)

    node_number = int(sys.argv[1])
    process_number = int(sys.argv[2])
    machine_id = int(sys.argv[3])
    numbering = sys.argv[4]

    #  print(node_number, process_number, machine_id, numbering)

    run(node_number, process_number, machine_id, numbering)
