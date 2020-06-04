# orc
Open Rowing Computer

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