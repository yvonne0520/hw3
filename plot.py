import matplotlib.pyplot as plt
import numpy as np
import serial
import time

# serial device
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

# sampling details
Fs = 100.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,10,10*Ts) # time vector; create Fs samples between 0 and 10.0 sec.
x = np.arange(0,1,Ts) # signal vector; create Fs samples
y = np.arange(0,1,Ts) # signal vector; create Fs samples
z = np.arange(0,1,Ts) # signal vector; create Fs samples
log = np.arange(0,1,Ts) # log data; create Fs samples

# read data from mbed
# x
for i in range(0, int(Fs)):
    line = s.readline()
    x[i] = float(line)
# y
for i in range(0, int(Fs)):
    line = s.readline()
    y[i] = float(line)
# z
for i in range(0, int(Fs)):
    line = s.readline()
    z[i] = float(line)
# log
for i in range(0, int(Fs)):
    line = s.readline()
    log[i] = int(line)

# plot
fig, ax = plt.subplots(2, 1)
ax[0].plot(t, x, label = 'x')
ax[0].plot(t, y, label = 'y')
ax[0].plot(t, z, label = 'z')
ax[0].set_ylabel('Acc Vector')
ax[0].set_xlabel('Time')
ax[0].legend()     # add legend
ax[1].stem(t, log, use_line_collection = True)
ax[1].set_ylabel('Tilt')
ax[1].set_xlabel('Time')

plt.show()
s.close()