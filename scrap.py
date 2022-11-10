#!/usr/bin/python3 
import time
from tempgrapher import *

ser = serial.Serial('/dev/ttyACM0', 9600)
rl = ReadLine(ser)


while True:
	print(str(rl.readline(), 'utf-8'))
	time.sleep(.5)



