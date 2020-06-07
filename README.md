# orc - The Open Rowing Computer
Orc was started during the corona pendemic and the time where it wasn't possible to me to go rowing. The basic concept simpy: Use available electronic parts and a 3D-printer to create a good and solid rowing computer for the day-to-day-training.

The system consists of a microcontroller, SD-card, acceleration sensor, e-paper display, an li-ion battery and a charing electronic.

## State of the project

- Electronic parts working together
- Acceleration sensor software works
- e-paper display software works
- Logging to SD-card works
- charging works

## Next steps

- Design, build and test a housing
- Add a voltage divider to show the battery state-of-charge
- 


## Wiring

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
