import time
import random

tempdatafile = "randomdata.txt"
file = open(tempdatafile, "w")



while True:
    file.write(", ".join([str(round(random.uniform(70.00,80.00), 2)) for temp in range(5)]))
    file.write("\n")
    time.sleep(.25)

file.close()