import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import datetime as dt


tempdatafile = "randomdata.txt"

class Sensor:
	def __init__(self, number):
		self.name = number
		self.temp_data = list()


class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)

def readSerial(ser, rl, temp1, temp2, temp3, temp4, temp5):
	newline = rl.readline()
	split = newline.split(', ')
	temp1.append(split[0])
	temp2.append(split[1])
	temp3.append(split[2])
	temp4.append(split[3])
	temp5.append(split[4])

def main2():
	ser = serial.Serial('/dev/ttyACM0', 9600)
	rl = ReadLine(ser)
	temp1 = list()
	temp2 = list()
	temp3 = list()
	temp4 = list()
	temp5 = list()
	x = list()

	x_len = 200
	y_range = [0, 100]

	fig = plt.figure()
	ax = fig.add_subplot(1,1,1)

	x = list(range(0, 200))
	temp1 = [0] * x_len
	temp2 = [0] * x_len
	temp3 = [0] * x_len
	temp4 = [0] * x_len
	temp5 = [0] * x_len

	ax.set_ylim(y_range)

	line1, = ax.plot(x, temp1)
	line2, = ax.plot(x, temp2)
	line3, = ax.plot(x, temp3)
	line4, = ax.plot(x, temp4)
	line5, = ax.plot(x, temp5)



	ani = animation.FuncAnimation(fig, animate, fargs=(rl, x, temp1, temp2, temp3, temp4, temp5), interval=1000)
	plt.show()



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

def main1():
	with open(tempdatafile, mode='r') as file:
		for line in follow(file):
			print(line)

if __name__ == "__main__":
	main1()