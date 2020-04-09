import socket
import time
import random

UDP_IP = "127.0.0.1"; # Local host
UDP_PORT = 60001; # Communication Port

HR = 80;
RANDOM_AMPLITUDE = 1;
COUNTER = 0;

PEAK_CHANCE = 1./100.;
IN_PEAK = False;
PEAK_LENGTH = 10;
PEAK_HEIGHT = 50;

VALLEY_CHANCE = 1./100.;
IN_VALLEY = False;
VALLEY_LENGTH = 10;
VALLEY_HEIGHT = 50;


print ("UDP target IP:", UDP_IP);
print ("UDP target port:", UDP_PORT);


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM); # UDP
while (1):
	# Print the message and send it to iMotions
	MESSAGE = "E;1;HRSensor;1;;;;HRData;" + str(HR) + "\r\n";
	print ("message:", MESSAGE);
	sock.sendto(MESSAGE.encode('utf-8'), (UDP_IP, UDP_PORT));
	
	# Randomly change the HR
	HR += (random.random() - 0.5) * RANDOM_AMPLITUDE;
	
	# Randomly start a peak or a valley
	if (random.random()) < PEAK_CHANCE and not IN_VALLEY and not IN_PEAK:
		COUNTER = 0;
		IN_PEAK = True;
	elif (random.random()) < VALLEY_CHANCE and not IN_VALLEY and not IN_PEAK:
		COUNTER = 0;
		IN_VALLEY = True;
		
	# Update the HR if we're in a peak or valley
	if IN_PEAK:
		if COUNTER / PEAK_LENGTH < 0.5:
			HR += PEAK_HEIGHT / PEAK_LENGTH * 2;
		else:
			HR -= PEAK_HEIGHT / PEAK_LENGTH * 2;
		COUNTER += 1;
		if COUNTER >= PEAK_LENGTH:
			IN_PEAK = False;
			
	if IN_VALLEY:
		if COUNTER / VALLEY_LENGTH < 0.5:
			HR -= VALLEY_HEIGHT / VALLEY_LENGTH * 2;
		else:
			HR += VALLEY_HEIGHT / VALLEY_LENGTH * 2;
		COUNTER += 1;
		if COUNTER >= VALLEY_LENGTH:
			IN_VALLEY = False;
	
	# Wait for the next packet
	time.sleep(0.5);
