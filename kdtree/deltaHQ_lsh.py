import lsh
import json
#  import pprint
import sys
#  from signal import signal, SIGPIPE, SIG_DFL

points = []
l2lsh = lsh.LSHIndex


def init(nodes):
    global l2lsh
    global points

    d = 2
    k = 1
    L = 2
    radius = 3
    w = radius * 1.1

    # TODO
    # radius別のテーブルを作成する
    l2lsh = lsh.LSHIndex(lsh.L2HashFamily(w, d), k, L, radius)

    for node in nodes:
        points.append([node['x'], node['y']])

    l2lsh.index(points)


def update(nodes):
    global points

    for n in nodes:
        points[n['id']] = [n['x'], n['y']]

    # TODO
    # radius別のテーブルに追加

    l2lsh.update(nodes, points)


def get_neighbor_ids(nodes):
    neighbor = []
    for n in nodes:
        neighbor.append(l2lsh.query([n['x'], n['y']], lsh.L2_norm))

    return neighbor


def send_deltaQ(neighbors, nodes, key):
    send_json = json.loads('{}')
    send_json[key] = []

    for i, node in enumerate(nodes):
        send_json[key].append([{'center': node}])

        send_json[key][i].append({'neighbor': []})

        for n in neighbors[i]:
            send_json[key][i][1]['neighbor'].append({
                'id': n,
                #  'x': points[n][0],
                #  'y': points[n][1],
                #  'r': 243
            })

        #  pprint.pprint(send_json)
    print(json.dumps(send_json))


def main():

    while True:
        try:
            recv_msg = input()
            if recv_msg == '':
                continue

            sys.stdin.flush()
            json_data = json.loads(recv_msg)

        except json.decoder.JSONDecodeError as e:
            print('json decode error')
            #  pass
            continue

        except BrokenPipeError:
            continue

        if 'init' in json_data:
            init(json_data['init'])
            #  print(json.dumps(json_data))
            send_deltaQ(
                get_neighbor_ids(json_data['init']),
                json_data['init'],
                'init',
            )

        if 'update' in json_data:

            update(json_data['update'])

            send_deltaQ(
                get_neighbor_ids(json_data['update']),
                json_data['update'],
                'neighbors',
            )

        if 'finish' in json_data:
            print(json.dumps(json_data))


if __name__ == '__main__':
    #  signal(SIGPIPE, SIG_DFL)
    main()
