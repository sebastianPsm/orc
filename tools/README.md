# Tools section

## live_plot.py
This little script uses [Panda3D](https://www.panda3d.org/) to visualize the quaternion rotation from the MPU chip and the applied rotation on the acceleration vector.

### Dependencies
- Python 3
- pyserial (pip install pyserial)
- panda3d (pip install panda3d==1.10.6)

### Quick start
Install the dependencies from above and call: ```python live_plot.py COM1```. Instead of COM1 use the serial interface that ORC is connected to.

### How does it work
The script opens the given serial interface and searches for lines in the ORC USB data stream that begins with "motion data: ". This lines also contains the rotation quaternion and acceleration from the MPU.

![panda](https://github.com/sebastianPsm/orc/raw/master/img/panda.png)