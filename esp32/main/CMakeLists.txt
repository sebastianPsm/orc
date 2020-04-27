# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.cpp" "ble_stuff.c" "imu.cpp" "display.c" "storage.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
