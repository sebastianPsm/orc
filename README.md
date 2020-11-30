Orc was started during the corona pendemic and the time where it wasn't possible for me to go rowing. The basic concept: Use available electronic parts (e.g. from [Reichelt Elektronik](https://www.reichelt.de/)) and a 3D-printer ([service](https://www.treatstock.com)) to create a good and solid rowing computer for the day-to-day-training.

The system consists of a microcontroller, SD-card, 9-DOF-sensor, e-paper display, a li-ion battery and charing electronic.

![prototype](https://github.com/sebastianPsm/orc/raw/master/img/prototype.jpg)

## State of the project

- Electronic parts working together
- 9-DOF-sensor software works
- e-paper display software works
- Logging to SD-card works (result is stored as csv file)
- Bluetooth works
- charging works

## Next steps

- Design, build and test a housing
- Add a voltage divider to show the battery state-of-charge
- Use Bluetooth to publish battery status and SRM (strokes per minute) value
- Use [Web Bluetooth](https://developer.mozilla.org/en-US/docs/Web/API/Web_Bluetooth_API) to show orc values like battery status and SRM

## Parts

- The needed electronic parts can be found in the parts_list directory. Currently theres only one parts list for Reichelt Elektronik available.
- More information about the first version of the housing will be uploaded soon

### Datasheets

- [Display](https://www.waveshare.com/w/upload/b/bb/2.9inch-e-paper-b-specification.pdf)
- [ESP32 Module (espressif esp32-devkitc)](https://www.espressif.com/en/products/devkits/esp32-devkitc/resources)
- [9 DOF Sensor](https://www.seeedstudio.com/Grove-IMU-9DOF-v2-0.html)
- [Charger](https://github.com/adafruit/Adafruit-PowerBoost-500-Charger-PCB)

## Wiring

Wiring lists of the electronic components

| ESP32-WROOM | ePaper |
| ----------- | ------ |
| D35         | BUSY - ![violett](https://via.placeholder.com/15/8800ff/000000?text=+) violett |
| D32         | RST - ![white](https://via.placeholder.com/15/ffffff/000000?text=+) white |
| D33         | DC - ![green](https://via.placeholder.com/15/00ff00/000000?text=+) green |
| D25         | CS - ![orange](https://via.placeholder.com/15/ffa500/000000?text=+) orange |
| D26         | CLK - ![yellow](https://via.placeholder.com/15/ffff00/000000?text=+) yellow |
| D27         | DIN - ![blue](https://via.placeholder.com/15/0000ff/000000?text=+) blue |
| 3V3         | VCC    |
| GND         | GND    |

| ESP32-WROOM | Accel sensor (IMU) |
| ----------- | ------ |
| D13         | INT    |
| D21         | SDA - ![white](https://via.placeholder.com/15/ffffff/000000?text=+) white |
| D22         | SCL - ![yellow](https://via.placeholder.com/15/ffff00/000000?text=+) yellow |

| Charger     | Accel sensor (IMU) |
| ----------- | -------- |
| VIN         | +5V - ![red](https://via.placeholder.com/15/ff0000/000000?text=+) red |
| GND         | GND - ![black](https://via.placeholder.com/15/000000/000000?text=+) black |

| ESP32-WROOM | Micro-SD Breakout Board |
| ----------- | ----------------------- |
| D5 (CS)     | CS                      |
| D23 (MOSI)  | DI                      |
| D18 (CLK)   | CLK                     |
| D19 (MISO)  | DO                      |
| GND         | GND                     |
| 3V3         | 3v                      |
SPI: VSPI

| ESP32-WROOM | Charger  |
| ----------- | -------- |
| VIN         | 5V       |
| GND         | GND      |
