import os
import socket


class NetworkSocket(socket.socket):
    def __init__(self):
        NETLINK_KOBJECT_UEVENT = 15
        socket.socket.__init__(self, socket.AF_NETLINK, socket.SOCK_DGRAM, NETLINK_KOBJECT_UEVENT)
        self.bind((os.getpid(), -1))

    def parse(self):
        data = self.recv(512)
        print("{0} received data size: {1} {0}".format('#'*20, len(data)))
        for item in data.split(b'\0'):
            if len(item) > 0:
                try:
                    print(item.decode('utf-8'))
                except UnicodeDecodeError:
                    pass


if __name__ == "__main__":
    nls = NetworkSocket()
    try:
        while True:
            nls.parse()
    except KeyboardInterrupt:
        print("\nDone")
