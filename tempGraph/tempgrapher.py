import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import time
import datetime as dt


class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i + 1]
            self.buf = self.buf[i + 1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i + 1]
                self.buf[0:] = data[i + 1:]
                return r
            else:
                self.buf.extend(data)


def readSerial(rl, temp1, temp2, temp3, temp4, temp5, file):
    newline = str(rl.readline(), 'utf-8')
    file.write(newline)
    file.write('\n')
    split = newline.split(', ')
    temp1.append(float(split[0]))
    temp2.append(float(split[1]))
    temp3.append(float(split[2]))
    temp4.append(float(split[3]))
    #temp5.append(float(split[4]))
    #temps = [float(split[0]), float(split[1]), float(split[2]), float(split[3]), float(split[4])]
    return temp1, temp2, temp3, temp4, temp5

def animate(i, rl, temp1, temp2, temp3, temp4, temp5, file):
    temp1, temp2, temp3, temp4, temp5 = readSerial(rl, temp1, temp2, temp3, temp4, temp5, file)
    # if i % 5 == 0:
    #     temps = [temp for temp in temps if temp != -1000]
    #     max_temp = max(temps)
    #     min_temp = min(temps)
    #     ax.set_ylim(min_temp - 10, max_temp + 10)
    # temp1, temp2, temp3, temp4, temp5 = yeildline(temp1, temp2, temp3, temp4, temp5)
    # print('temps yeilded')

    temp1, temp2, temp3, temp4, temp5 = temp1[-x_len::], temp2[-x_len:], temp3[-x_len:], temp4[-x_len:], temp5[-x_len:]


    line1.set_ydata(temp1)
    line2.set_ydata(temp2)
    line3.set_ydata(temp3)
    line4.set_ydata(temp4)
    #line5.set_ydata(temp5)

    return line1, line2, line3, line4



ser = serial.Serial('COM9', 9600)
rl = ReadLine(ser)

temp1 = list()
temp2 = list()
temp3 = list()
temp4 = list()
temp5 = list()
x = list()

x_len = 125
y_range = [65, 90]

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)



x = list(range(0, x_len))
temp1 = [0] * x_len
temp2 = [0] * x_len
temp3 = [0] * x_len
temp4 = [0] * x_len
temp5 = [0] * x_len

ax.set_ylim(y_range)

line1, = ax.plot(x, temp1)
line2, = ax.plot(x, temp2)
line3, = ax.plot(x, temp3, label="Fridge Temp 2", color="green")
line4, = ax.plot(x, temp4, label= "Fridge Temp 1", color="red")
# line5, = ax.plot(x, temp5)

plt.title('Temperature over Time')
plt.xlabel('Samples')
plt.ylabel('Temperature (deg F)')
plt.legend()

datafile = open("datafile.txt", mode='w')


ani = animation.FuncAnimation(fig, animate, fargs=(rl, temp1, temp2, temp3, temp4, temp5, datafile), blit=True)
plt.show()
