import matplotlib.pyplot as plt

tempdatafile = "sensordata.txt"

class Sensor:

	def __init__(number):
		self.name = number
		self.tempdata = list()

def main():
	with open(tempdatafile, mode='r') as file:
		data = file.read()
	print(data)
	sensor1 = Sensor(1)
	sensor2 = Sensor(2)

	for line in data.split('\n'):
		print(len(line))
		if len(line) > 1:
			for column in line.split(', '):
				if column[0] == "2":
					sensor2.tempdata.append(column[1])
	print(''.join(sensor2.tempdata))