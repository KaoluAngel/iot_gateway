# 由CMakeLists.txt调用，独立进程避免污染主CMake
cmake_minimum_required(VERSION 3.10)

set(SRC_DIR ${CMAKE_ARGV3})
set(TOOLCHAIN ${CMAKE_ARGV4})
set(CROSS_COMPILE ${CMAKE_ARGV5})

# 基础配置命令（不含工具链）
set(CONFIGURE_CMD 
    ${CMAKE_COMMAND}
    -S ${SRC_DIR}/vendor/paho.mqtt.c
    -B ${SRC_DIR}/build/${TOOLCHAIN}/paho-build
    -DCMAKE_INSTALL_PREFIX=${SRC_DIR}/build/${TOOLCHAIN}
    -DPAHO_WITH_SSL=OFF
    -DPAHO_BUILD_STATIC=ON
    -DPAHO_BUILD_SAMPLES=OFF
    -DPAHO_BUILD_DOCUMENTATION=OFF
)

# 条件性添加工具链参数（仅非 native 时）
if(NOT TOOLCHAIN STREQUAL "native")
    list(APPEND CONFIGURE_CMD 
        -DCMAKE_TOOLCHAIN_FILE=${SRC_DIR}/cmake/toolchain-${TOOLCHAIN}.cmake
    )
endif()

# 执行配置、构建、安装三连击
execute_process(
    COMMAND ${CONFIGURE_CMD}
    COMMAND ${CMAKE_COMMAND} --build ${SRC_DIR}/build/${TOOLCHAIN}/paho-build -j
    COMMAND ${CMAKE_COMMAND} --install ${SRC_DIR}/build/${TOOLCHAIN}/paho-build
    RESULT_VARIABLE RESULT
)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "paho.mqtt.c 编译失败")
endif()