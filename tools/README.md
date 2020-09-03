# Tools section

## live_plot.py
This little script uses [Panda3D](https://www.panda3d.org/) to visualize the quaternion rotation from the MPU chip and the applied rotation on the acceleration vector.

### Dependencies
- Python 3
- pyserial (pip install pyserial)
- panda3d (pip install panda3d==1.10.6)

### Quick start
Install the dependencies from above and call for example: ```python live_plot.py COM1```

### How does it work
The script opens the given serial interface and searches for "motion data: " entries in the data stream from the ORC. The following data contains therotation quaternion and acceleration from the MPU.

![panda](https://github.com/sebastianPsm/orc/raw/master/img/panda.png)