import serial
import time

s = serial.Serial('COM17',115200)
time.sleep(1)

s.write('load')
s.write('time;100')
s.write('set;0')
#s.write('on')

time.sleep(2)

factor=1.8


def serialRead(ser):
    while ser.in_waiting:  # Or: while ser.inWaiting():
        print (ser.readline())
        


with open('traj.txt','r') as f:
    lines = f.readlines()
    f.close()
    


try:
    while True:
        for line in lines:
            curr_set = [float(x) for x in line.split('\t')]
            string="set;%0.3f;%0.3f;%0.3f;%0.3f" % (factor*curr_set[1],factor*curr_set[2],factor*curr_set[3],factor*curr_set[4])
            print(string)
            s.write(string+'\n')
            time.sleep(curr_set[0])
except KeyboardInterrupt:
    s.close()