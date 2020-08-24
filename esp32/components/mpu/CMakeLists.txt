set(COMPONENT_ADD_INCLUDEDIRS "hal/include"
                              "motion_driver_6.12/msp430/eMD-6.0/core/driver"
                              "motion_driver_6.12/msp430/eMD-6.0/core/driver/include"
                              "motion_driver_6.12/msp430/eMD-6.0/core/")
set(COMPONENT_SRCS "hal/src/mpu_idf_esp32_hal.c"
                   "motion_driver_6.12/msp430/eMD-6.0/core/driver/eMPL/inv_mpu.c"
                   "motion_driver_6.12/msp430/eMD-6.0/core/driver/eMPL/inv_mpu_dmp_motion_driver.c"
                   "motion_driver_6.12/msp430/eMD-6.0/core/mllite/ml_math_func.c")

register_component()