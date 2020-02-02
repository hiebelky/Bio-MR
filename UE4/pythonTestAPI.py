import socket

UDP_IP = "127.0.0.1" # Local host
UDP_PORT = 60221 # Communication Port
MESSAGE = "00000938;EyeTracker;EyeData;18332;15040;18319;1379;601;1379;601;"

print ("UDP target IP:", UDP_IP)
print ("UDP target port:", UDP_PORT)
print ("message:", MESSAGE)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE.encode('utf-8'), (UDP_IP, UDP_PORT))
