#
# orc tool to plot live (real-time) quaternion data 
#
# Install dependencies with
#  pip install pyserial
#  pip install panda3d==1.10.6
#
import sys
import signal
import time
import math
import json
import serial
import csv

from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
import panda3d.core as p3d

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
        return (None, None, None)
    if not line.startswith(DATA_PREFIX):
        return (None, None, None)
    data_str = line[len(DATA_PREFIX):]
    data = json.loads(data_str)
    quat = [fp_str2float(s, 32) for s in data[0:4]]
    accel = [fp_str2float(s, 16) for s in data[4:7]]
    gyro =  [fp_str2float(s, 16) for s in data[7:10]]
    return (quat, accel, gyro)

class MyApp(ShowBase):
    def __init__(self, logcsv):
        ShowBase.__init__(self)
        self.logcsv = logcsv
        self.log_count = 0
        self.log_time_offset = 0
        self.log_time = 0

        # Add the spinCameraTask procedure to the task manager.
        self.taskMgr.add(self.spinCameraTask, "SpinCameraTask")
        self.taskMgr.add(self.getSerialDataTask, "GetSerialDataTask")

        # Load and transform the panda actor.
        self.pandaActor = Actor("models/panda-model",
                                {"walk": "models/panda-walk4"})
        self.pandaActor.setScale(0.005, 0.005, 0.005)
        
        self.pandaActor.reparentTo(self.render)
        # Loop its animation.
        self.pandaActor.loop("walk")

        node_x, node_y, node_z = self.createAccelNodes(1,1,1)

        self.np_x = p3d.NodePath(node_x)
        self.np_y = p3d.NodePath(node_y)
        self.np_z = p3d.NodePath(node_z)

        self.np_x.reparentTo(self.render)
        self.np_y.reparentTo(self.render)
        self.np_z.reparentTo(self.render)
    
    def createAccelNodes(self, x,y,z):
        accel_x = p3d.LineSegs()
        accel_x.setColor(1,0,0,0)
        accel_x.moveTo(0, 0, 0)
        accel_x.drawTo(x, 0, 0)
        accel_x.setThickness(4)
        accel_y = p3d.LineSegs()
        accel_y.setColor(0,1,0,0)
        accel_y.moveTo(0, 0, 0)
        accel_y.drawTo(0, y, 0)
        accel_y.setThickness(4)
        accel_z = p3d.LineSegs()
        accel_z.setColor(0,0,1,0)
        accel_z.moveTo(0, 0, 0)
        accel_z.drawTo(0, 0, z)
        accel_z.setThickness(4)

        node_x = accel_x.create()
        node_y = accel_y.create()
        node_z = accel_z.create()

        return node_x, node_y, node_z
   
    # Define a procedure to move the camera.
    def spinCameraTask(self, task):
        t = task.time
        angleDegrees = t * 6.0
        angleRadians = angleDegrees * (math.pi / 180.0)
        self.camera.setPos(20 * math.sin(angleRadians), -20 * math.cos(angleRadians), 3)
        self.camera.setHpr(angleDegrees, 0, 0)
        return Task.cont
    
    def getSerialDataTask(self, task):
        quat, accel, gyro = get_data_via_serial()
        if not quat or not accel or not gyro:
            return Task.cont

        if self.log_time_offset == 0:
            self.log_time_offset = time.time()
        t = time.time() - self.log_time_offset

        self.logcsv.writerow([(t - self.log_time)*1000, self.log_count] + quat + accel + gyro)
        self.log_count += 1
        self.log_time = t
        
        if quat:
            self.pandaActor.setQuat(p3d.LQuaternionf(quat[0], quat[1], quat[2], quat[3]))
        if accel:
            self.np_x.removeNode()
            self.np_y.removeNode()
            self.np_z.removeNode()

            node_x, node_y, node_z = self.createAccelNodes(accel[0] * 10,accel[1] * 10,accel[2] * 10)

            self.np_x = p3d.NodePath(node_x)
            self.np_y = p3d.NodePath(node_y)
            self.np_z = p3d.NodePath(node_z)

            self.np_x.reparentTo(self.render)
            self.np_y.reparentTo(self.render)
            self.np_z.reparentTo(self.render)
        
        return Task.cont

if __name__ == "__main__":
    script_name = sys.argv[0]
    if len(sys.argv) != 2:
        print(f"usage: python {script_name} serial_interface")
        print(f" e.g.: python {script_name} COM1")
        sys.exit(0)
    port = sys.argv[1]

    signal.signal(signal.SIGINT, signal_handler)

    SER = serial.Serial(port, 115200, timeout=1)

    with open("log.csv", "w", newline='') as file:
        csvfile = csv.writer(file, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
        app = MyApp(csvfile)
        app.run()
