import matplotlib.pyplot as plt
import numpy as np
import serial
import time

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

Fs = 100.0  # sampling rate
Ts = 1 / Fs
t = np.arange(0,10,10*Ts)
x = np.arange(0,1,Ts) 
y = np.arange(0,1,Ts)
z = np.arange(0,1,Ts) 
log = np.arange(0,1,Ts)

for i in range(0, int(Fs)):
    line = s.readline()
    x[i] = float(line)

for i in range(0, int(Fs)):
    line = s.readline()
    y[i] = float(line)

for i in range(0, int(Fs)):
    line = s.readline()
    z[i] = float(line)

for i in range(0, int(Fs)):
    line = s.readline()
    log[i] = int(line)

fig, ax = plt.subplots(2, 1)
ax[0].plot(t, x, label = 'x')
ax[0].plot(t, y, label = 'y')
ax[0].plot(t, z, label = 'z')
ax[0].set_ylabel('Acc Vector')
ax[0].set_xlabel('Time')
ax[0].legend()
ax[1].stem(t, log, use_line_collection = True)
ax[1].set_ylabel('Tilt')
ax[1].set_xlabel('Time')

plt.show()
s.close()