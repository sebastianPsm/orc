Orc was started during the corona pendemic and the time where it wasn't possible for me to go rowing. The basic concept: Use available electronic parts (e.g. from [Reichelt Elektronik](https://www.reichelt.de/)) and a 3D-printer ([service](https://www.treatstock.com)) to create a good and solid rowing computer for the day-to-day-training.

The system consists of a microcontroller, SD-card, 9-DOF-sensor, e-paper display, a li-ion battery and charing electronic.

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

## Wiring

Wiring lists of the electronic components


| ESP32-WROOM | ePaper |
| ----------- | ------ |
| D35         | BUSY   |
| D32         | RST    |
| D33         | DC     |
| D25         | CS     |
| D26         | CLK    |
| D27         | DIN    |
| GND         | GND    |

| ESP32-WROOM | Accel  |
| ----------- | ------ |
| D13         | INT    |
| D5          | CS     |
| D18         | SCLK   |
| D23         | MOSI   |
| D19         | MISO   |

| ESP32-WROOM | SD-Card          |
| ----------- | ---------------- |
|             | Pin 1 (DAT 2)    |
| D14         | Pin 2 (DAT 3)    |
| D27         | Pin 3 (CMD I/O)  |
| GND         | Pin 4 (GND)      |
| 3V3         | Pin 5 (VDD)      |
| D26         | Pin 6 (CLK)      |
| GND         | Pin 7 (GND)      |
| D12         | Pin 8 (DAT 0)    |
|             | Pin 9 (DAT 1)    |

| ESP32-WROOM | Charger  |
| ----------- | -------- |
| VIN         | 5V       |
| GND         | GND      |

| ePaper      | Charger  |
| ----------- | -------- |
| VCC         | 5V       |
