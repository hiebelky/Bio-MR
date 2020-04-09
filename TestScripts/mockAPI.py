import socket
import time

UDP_IP = "127.0.0.1" # Local host
UDP_PORT = 60001 # Communication Port
MESSAGE = "M;2;;;GrabItem;User Has Picked up an item;S;V\r\n"

print ("UDP target IP:", UDP_IP)
print ("UDP target port:", UDP_PORT)
print ("message:", MESSAGE)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
for i in range(1):
    sock.sendto(MESSAGE.encode('utf-8'), (UDP_IP, UDP_PORT))

time.sleep(1);

MESSAGE = "M;2;;;GrabItem;;E;\r\n"
for i in range(1):
    sock.sendto(MESSAGE.encode('utf-8'), (UDP_IP, UDP_PORT))
