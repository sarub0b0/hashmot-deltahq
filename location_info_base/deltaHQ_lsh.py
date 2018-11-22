import lsh
import json
import pprint
import sys
from signal import signal, SIGPIPE, SIG_DFL

points = []
l2lsh = lsh.LSHIndex


def init(nodes, radius):
    global l2lsh
    global points

    d = 2
    k = 3
    L = 32
    #  radius = 243
    w = radius * 1.05

    # TODO
    # radius別のテーブルを作成する
    l2lsh = lsh.LSHIndex(lsh.L2HashFamily(w, d), k, L, radius)

    for node in nodes:
        points.append([node['x'], node['y']])

    l2lsh.index(points)


def update(nodes):
    global points
    id_list = []

    points[nodes['id']] = [nodes['x'], nodes['y']]
    id_list.append(nodes['id'])

    # TODO
    # radius別のテーブルに追加

    l2lsh.update(id_list, points)


def get_neighbor_ids(nodes):
    neighbor = []
    for n in nodes:
        neighbor.append(l2lsh.query([n['x'], n['y']], lsh.L2_norm))

    return neighbor


def send_deltaQ(neighbors, json_data, key):
    neighbor_json = json.loads('{}')
    neighbor_json['neighbors'] = []

    if key == 'init':
        for i, node in enumerate(json_data['init']['node']):
            neighbor_json['neighbors'].append({
                'center': {
                    'id': i,
                    'x': node['x'],
                    'y': node['y']
                },
                'neighbor': []
            })

            for n in neighbors[i]:
                neighbor_json['neighbors'][i]['neighbor'].append({
                    'id': n,
                })

        json_data = json.loads('{}')
        json_data['update'] = neighbor_json

    if key == 'update':
        for i, node in enumerate(json_data['update']):
            neighbor_json['neighbors'].append({
                'center': {
                    'id': node['id'],
                    'x': node['x'],
                    'y': node['y']
                },
                'neighbor': []
            })

            for n in neighbors[i]:
                neighbor_json['neighbors'][i]['neighbor'].append({
                    'id': n,
                })

        json_data['update'] = neighbor_json


    sys.stdout.write(json.dumps(json_data, separators=(',', ':')))
    sys.stdout.write("\n")
    sys.stdout.flush()


def main(argv):
    radius = int(argv[1])

    while True:
        try:
            recv_msg = input()
            if recv_msg == '':
                continue

            json_data = json.loads(recv_msg)

        except json.decoder.JSONDecodeError as e:
            print('json decode error')
            #  pass
            continue

        if 'init' in json_data:
            sys.stdout.write(json.dumps(json_data, separators=(',' ':')))
            sys.stdout.write("\n")
            sys.stdout.flush()

            init(json_data['init']['node'], radius)
            #  print(json.dumps(json_data))
            send_deltaQ(
                get_neighbor_ids(json_data['init']['node']),
                json_data,
                'init',
            )

        if 'update' in json_data:

            update(json_data['update'])

            send_deltaQ(
                get_neighbor_ids(json_data['update']),
                json_data,
                'update',
            )

        if 'finish' in json_data:
            #  print(json.dumps(json_data))
            sys.stdout.write(json.dumps(json_data, separators=(',' ':')))
            sys.stdout.write("\n")
            sys.stdout.flush()


if __name__ == '__main__':
    signal(SIGPIPE, SIG_DFL)
    main(sys.argv)
