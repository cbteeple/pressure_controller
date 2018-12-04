import serial
import time

s = serial.Serial('COM15')
s.write('on')

time.sleep(1)

for i in range(1,100):
    line = ser.readline()   # read a '\n' terminated line
    print(line)
    
print("Hello, World!")     # natch