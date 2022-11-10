import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import datetime as dt


tempdatafile = "randomdata.txt"

class Sensor:
	def __init__(self, number):
		self.name = number
		self.temp_data = list()




def animate(i, rl, x, temp1, temp2, temp3, temp4, temp5):
	newline = rl.readline()
	split = newline.split(', ')
	temp1.append(split[0])
	temp2.append(split[1])
	temp3.append(split[2])
	temp4.append(split[3])
	temp5.append(split[4])
	x.append(dt.datetime.now().strftime('%H:%M:%S.%f'))


	temp1 = temp1[-20:]
	temp2 = temp2[-20:]
	temp3 = temp3[-20:]
	temp4 = temp4[-20:]
	temp5 = temp5[-20:]
	x = x[-20:]

	ax.clear()
	ax.plot(x, temp1)
	ax.plot(x, temp2)
	ax.plot(x, temp3)
	ax.plot(x, temp4)
	ax.plot(x, temp5)

	plt.xticks(rotation=45, ha='right')
	plt.subplots_adjust(bottom=0.30)
	plt.title('Temperature over Time')
	plt.ylabel('Temperature (deg F)')


def follow(thefile):
    thefile.seek(0,2) # Go to the end of the file
    while True:
        line = thefile.readline()
        if not line:
            time.sleep(0.01) # Sleep briefly
            continue
        yield line

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

	with open(tempdatafile, mode='r') as file:
		for line in follow(file):
			temps = line.split(', ')
			sensor1.temp_data.append(temps[0])
			sensor2.temp_data.append(temps[1])
			sensor3.temp_data.append(temps[2])
			sensor4.temp_data.append(temps[3])
			sensor5.temp_data.append(temps[4])

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

def yeildline(temp1, temp2, temp3, temp4, temp5):
	file = open(tempdatafile, mode='r')
	line = follow(file)

	split = line.split(', ')
	temp1.append(int(split[0]))
	# print(split[0])
	temp2.append(int(split[1]))
	temp3.append(int(split[2]))
	temp4.append(int(split[3]))
	temp5.appendo(int(split[4]))
	return temp1, temp2, temp3, temp4, temp5

if __name__ == "__main__":
	file = open(tempdatafile, mode='r')
	for line in follow(file):
		print(line)