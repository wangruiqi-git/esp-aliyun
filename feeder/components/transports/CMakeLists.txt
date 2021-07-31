set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_SRCS "transport_uart.c")

set(COMPONENT_REQUIRES "esp-aliyun")
set(COMPONENT_PRIV_REQUIRES "cmd_entry")
register_component()