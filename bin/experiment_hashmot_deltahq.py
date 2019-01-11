#!/usr/bin/env python3
import subprocess as sp
import sys
import os
#  import time
import numpy as np
#  from multiprocessing import Process
import pexpect
#  import glob
#  import math

remote_dir = '/home/kosay/hashmot'
local_dir = '/Users/kosay/work/hashmot'
remote_scenario_dir = remote_dir + "/exp_hashmot_deltahq_json"
local_scenario_dir = local_dir + "/exp_hashmot_deltahq_json"
deltaHQ_path = remote_dir + '/bin/deltaHQ'

remote_addr = '172.31.0.11'


def make_id_list(node_number, process_number, machine_id, numbering):
    machine_number = int(node_number / process_number)
    machine_number_mod = node_number % process_number

    print("id(%d) machine number(%d, mod=%d)" % (machine_id, machine_number,
                                                 machine_number_mod))

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


def create_scenario_files(node_number, communication_radius, scenario_dir):

    create_command = [
        './create_measured_init_json.sh',
        str(node_number),
        str(communication_radius),
        scenario_dir,
    ]
    sp.run(create_command)


def send_scenario_files(node_number):

    sp.run([
        'scp',
        '-r',
        local_scenario_dir + "/%dnode/" % node_number,
        remote_addr + ":" + remote_scenario_dir,
    ])

    output_dir = remote_scenario_dir + '/%dnode' % node_number
    #  scenario_files = glob.glob(output_dir + "/*.json")

    p = sp.run([
        'ssh',
        remote_addr,
        'ls',
        output_dir + '/*.json',
    ], stdout=sp.PIPE)

    output = p.stdout.decode('utf-8')
    scenario_files = output.splitlines()

    return scenario_files


def run(node_number, process_number, machine_id, numbering, exec_type, address,
        port):

    own_ids = np.array(
        make_id_list(node_number, process_number, machine_id, numbering))

    print("own_ids(%d)" % len(own_ids))

    deltahq_childs = []

    if exec_type == 'd':
        for scenario_json in scenario_files:
            json_name = scenario_json.split('/')[-1]
            local_scenario_json = local_scenario_dir + '/%dnode/' % node_number + json_name

            log_file = local_scenario_dir + '/%dnode/' % node_number + json_name + '.dp' + str(process_number) + '.log'
            try:
                os.remove(log_file)
            except OSError:
                pass

            for ids in own_ids:
                deltahq_command = "ssh " + remote_addr + ' ' + remote_dir + "/bin/deltaHQ -t 1 -i " + str(
                    ids) + " " + scenario_json + " -L " + str(
                        port) + " 2>&1 | tee " + log_file

                deltahq_childs.append(pexpect.spawn(deltahq_command))
                deltahq_childs[-1].logfile_read = sys.stdout.buffer
                print("spawn deltahq")
                deltahq_childs[-1].expect(
                    "-- Initial Neighbors Update:", timeout=-1)
                print("match deltahq Initial")

                #  childs = pexpect.spawn(deltahq_command)
                #  childs.logfile_read = sys.stdout.buffer
                #  print("spawn deltahq")
                #  childs.expect("-- Initial Neighbors Update:")
                #  print("match deltahq Initial")

            #  for ch in childs:
            #      ch.expect("Initial Neighbors Update")

            hashmot_command = "bash -c \"./measure.py " + local_scenario_json + " " + str(
                node_number
            ) + " 0 1 r | ./location_info_base -A " + address + " -p " + str(
                port
            ) + " " + local_scenario_json + " -a t 2>&1| tee -a " + log_file + "\""
            hashmot_child = pexpect.spawn(hashmot_command)
            hashmot_child.logfile_read = sys.stdout.buffer

            print("spawn hashmot")

            hashmot_child.expect("-- Update Wait --", timeout=-1)
            print("match hashot update wait")

            #  time.sleep(5)

            hashmot_child.sendline("\n")

            #  hashmot_child.expect("-- Success --")
            hashmot_child.expect(pexpect.EOF, timeout=-1)
            #  hashmot_child.close()

            for ch in deltahq_childs:
                #  ch.expect("-- Scenario processing completed successfully")
                ch.expect(pexpect.EOF, timeout=-1)
                #  ch.close()


if __name__ == '__main__':
    if len(sys.argv) < 5:
        sys.stderr.write("set argument\n")
        sys.stderr.write("  $1: l or L\n")
        sys.stderr.write("  $2: d or p (distributed or parallel)\n")
        sys.stderr.write("  $3: address\n")
        sys.stderr.write("  $4: port\n")
        sys.exit(1)

    node_list = [100, 200, 500, 1000, 2000, 5000, 10000]
    proc_list = [1, 2, 4, 8, 16, 24, 48, 72]

    machine_id = 0
    numbering = sys.argv[1]
    exec_type = sys.argv[2]
    address = sys.argv[3]
    port = int(sys.argv[4])

    sp.run(['mkdir', remote_scenario_dir])

    os.makedirs(local_scenario_dir, exist_ok=True)
    #  print(node_number, process_number, machine_id, numbering)

    for node in node_list:
        create_scenario_files(node, 160, local_scenario_dir)
        scenario_files = send_scenario_files(node)

        for proc in proc_list:
            run(node, proc, machine_id, numbering, exec_type, address, port)
