import socket

UDP_IP = "127.0.0.1" # Local host
UDP_PORT = 60003 # Communication Port
MESSAGE = "RegisterCommand;Rain Intensity;False;Float;0;0;1;"

print ("UDP target IP:", UDP_IP)
print ("UDP target port:", UDP_PORT)
print ("message:", MESSAGE)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
for i in range(1):
    sock.sendto(MESSAGE.encode('utf-8'), (UDP_IP, UDP_PORT))

MESSAGE = "RegisterCommand;Fetch Quest;True;Int;1;3;100;" 
for i in range(1):
    sock.sendto(MESSAGE.encode('utf-8'), (UDP_IP, UDP_PORT))
