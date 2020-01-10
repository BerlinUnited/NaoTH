import socket
import struct
import argparse

parser = argparse.ArgumentParser(description='this script connects to a multicast group and dumps the first 10 received packages',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--mcast-address", default="239.255.42.99", help='ip of multicast group')
parser.add_argument("--port", default=1511, help='port to listen to', type=int)
args = parser.parse_args()

multicastAddress = args.mcast_address
port = args.port

_socket = socket.socket(socket.AF_INET,      # internet, see `man 2 socket`
                        socket.SOCK_DGRAM,   # socket type allowing datagrams,
                                             # i.e. connectionless, possibly unreliable,
                                             # possibly unorderd received packets of fixed length
                                             # used for protocols like UDP
                        socket.IPPROTO_UDP)  # use UDP protocol

_socket.setsockopt(socket.SOL_SOCKET,    # modify option on socket level
                   socket.SO_REUSEADDR,  # allow multiple sockets to use the same address, see `man 7 socket`
                   1)                    # enable SO_REUSEADDR

_socket.bind(('', port))  # bind the socket to all local interfaces, '' means INADDR_ANY

mreq = struct.pack("4sl", socket.inet_aton(multicastAddress), socket.INADDR_ANY)
_socket.setsockopt(socket.IPPROTO_IP,         # modify option on IP protocol level
                   socket.IP_ADD_MEMBERSHIP,  # join multicast group, see `man 7 ip`
                   mreq)                      # byte object containing <ip multicast group><ip-address of local interface>,
                                              # if the local address is INADDR_ANY the interface is choosen by the system

for _ in range(10):
    data, addr = _socket.recvfrom(32768)  # 32k byte buffer size
    if(len(data) > 0):
        print("received from {}:".format(addr))
        print(data.decode('ascii', 'ignore'))
        print("-"*50)
