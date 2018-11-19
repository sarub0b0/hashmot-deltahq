import socket
import sys
from contextlib import closing

def main(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    with closing(sock):
        sock.bind((host, int(port)))
        while True:
            hoge = sock.recv(1024).decode('utf-8')
            print(hoge)
            #  sys.stdout.write(hoge)
    return

if __name__ == '__main__':
    print("host: " + sys.argv[1])
    print("port: " + sys.argv[2])
    main(sys.argv[1], sys.argv[2])

