set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_SRCS "src/epdif.c" "src/epdpaint.c" "src/epd2in9b.c")

set(COMPONENT_REQUIRES esp-tls)

register_component()
