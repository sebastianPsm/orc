Unfortunatly, the TDK 'Software License Agreement' doesn't allow to
redistribute the motion driver for MPU chips. Therefore, the motion driver
must be downloaded separatly and extracred here.

Download eMD 6.12 from https://invensense.tdk.com/developers/software-downloads
and extract the content here. The directory should look like the following:

- \components
  - \mpu
    - \hal
    - \motion_driver_6.12         <- add content of the TDK [motion_driver_6.12.zip](https://invensense.tdk.com/developers/download/emd-6-12/?wpdmdl=45) here
    - CMakeLists.txt
    - Kconfig
    - motion_driver_6.12.zip      <- Please ignore this archive and download from the link above. This file is only used for continous integration with GitHub
    - patch.diff
    - README.txt
- \main
  - CMakeLists.txt
  - component.mk
  - main.c
- .gitignore
- CMakeLists.txt
- License
- Makefile
- README.md
