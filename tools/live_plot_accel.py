#
# orc tool to plot live (real-time) acceleration data 
#
# Install dependencies with
#  pip install pyserial
#
import sys
import signal
import time
import math
import json
import serial

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style

SER = None
DATA_PREFIX = "motion data: "

fp_str2float = lambda fp, frac: int(fp) / 2**frac # convert fixed-point string to float

def signal_handler(sig, frame):
    if SER:
        SER.close()
    sys.exit(0)

def get_data_via_serial():
    try:
        line = SER.readline().decode("ascii")
    except UnicodeDecodeError:
        return None
    
    print(line)
    return None

    if not line.startswith(DATA_PREFIX):
        return None    

    data_str = line[len(DATA_PREFIX):]
    data = json.loads(data_str)
    quat = [fp_str2float(s, 32) for s in data[0:4]]
    accel = [fp_str2float(s, 16) for s in data[4:7]]
    accel_xy = float(data[7])
    accel_z = float(data[8])
    
    return (quat, accel, accel_xy, accel_z)

xs = list(range(1, 1001))
ys_accel_xy = [0] * 1000
def animate(i):
    data = get_data_via_serial()
    if not data: return
    (quat, accel, accel_xy, accel_z) = data

    #print(accel_xy)
    #ys_accel_xy[i%1000] = accel_xy
    #
    #if(i%10 == 0):
    #    ax1.clear()
    #    ax1.plot(xs, ys_accel_xy)

if __name__ == "__main__":
    script_name = sys.argv[0]
    if len(sys.argv) != 2:
        print(f"usage: python {script_name} serial_interface")
        print(f" e.g.: python {script_name} COM1")
        sys.exit(0)
    port = sys.argv[1]

    signal.signal(signal.SIGINT, signal_handler)

    SER = serial.Serial(port, 115200, timeout=1)

    fig = plt.figure()
    ax1 = fig.add_subplot(1,1,1)
    ani = animation.FuncAnimation(fig, animate, interval=100)
    plt.show()
