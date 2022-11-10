import matplotlib.pyplot as plt

tempdatafile = "sensordata.txt"

class Sensor:

	def __init__(self, number):
		self.name = number
		self.temp_data = list()

def plot(sensor1, sensor2, sensor3, sensor4, sensor5):
	time = [ele for ele in range(len(sensor1))]
	plt.plot(time, sensor1)
	plt.plot(time, sensor2)
	plt.plot(time, sensor3)
	plt.plot(time, sensor4)
	plt.plot(time, sensor5)
	plt.show()

def main():
	with open(tempdatafile, mode='r') as file:
		data = file.read()
	sensor1 = Sensor(1)
	sensor2 = Sensor(2)
	sensor3 = Sensor(3)
	sensor4 = Sensor(4)
	sensor5 = Sensor(5)
	for line in data.split('\n'):
		if len(line) > 1:
			column = line.split(', ')
			if column[1] == -100:
				column[1] = None
			if column[0] == "1":
				sensor1.temp_data.append(column[1])
			if column[0] == "2":
				sensor2.temp_data.append(column[1])
			if column[0] == "3":
				sensor3.temp_data.append(column[1])
			if column[0] == "4":
				sensor4.temp_data.append(column[1])
			if column[0] == "5":
				sensor5.temp_data.append(column[1])
	plot(sensor1.temp_data, sensor2.temp_data, sensor3.temp_data, sensor4.temp_data, sensor5.temp_data)

if __name__ == "__main__":
	main()