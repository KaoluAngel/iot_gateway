add_library(common STATIC
    log/log.c
    ipc/ipc_unix.c
    config/config_yaml.c
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 如果 common 依赖 cjson 或 libmodbus
target_link_libraries(common
    cjson
)
