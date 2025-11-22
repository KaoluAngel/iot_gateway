cmake_minimum_required(VERSION 3.10)
project(mqtt_demo C)

# 1. 自动检测架构（默认本地编译）
if(NOT DEFINED TOOLCHAIN)
    set(TOOLCHAIN "native")
    message(STATUS "🖥️  本地编译模式（默认）")
else()
    message(STATUS "🔧 交叉编译模式：${TOOLCHAIN}")
endif()

# 2. 设置编译产物目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${TOOLCHAIN}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${TOOLCHAIN}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${TOOLCHAIN}/bin)

# 3. 定义paho编译脚本（自动执行）
function(build_paho_mqtt)
    # 避免重复编译
    if(EXISTS ${CMAKE_SOURCE_DIR}/build/${TOOLCHAIN}/lib/libpaho-mqtt3a.a)
        return()
    endif()

    message(STATUS "⏳ 首次编译 paho.mqtt.c...")
   execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/build-paho.cmake
        ${CMAKE_SOURCE_DIR} ${TOOLCHAIN} ${CROSS_COMPILE}
        RESULT_VARIABLE PAHO_RESULT
    )
    
    if(NOT PAHO_RESULT EQUAL 0)
        message(FATAL_ERROR "paho.mqtt.c 编译失败！")
    endif()
endfunction()

# 4. 编译paho（仅开发环境需要）
if(NOT DEFINED ENV{STAGING_DIR})
    build_paho_mqtt()
    set(PAHO_DIR ${CMAKE_SOURCE_DIR}/build/${TOOLCHAIN})
else()
    # Buildroot模式：直接引用staging目录
    set(PAHO_DIR $ENV{STAGING_DIR}/usr)
endif()


# 使用 GLOB_RECURSE 自动查找（方便但需小心）
# 自动递归查找 src 下所有 .c 文件：
file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(mqtt_demo ${SRC_FILES})

# 5. 你的应用
# add_executable(mqtt_demo src/main.c)

# 6. 包含路径和链接
target_include_directories(mqtt_demo PRIVATE 
    ${PAHO_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/paho.mqtt.c/src  # 源码调试用
)

target_link_libraries(mqtt_demo PRIVATE
    ${PAHO_DIR}/lib/libpaho-mqtt3a.a
    pthread
)

# 7. 本地测试便捷目标
if(TOOLCHAIN STREQUAL "native")
    add_custom_target(run
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mqtt_demo
        DEPENDS mqtt_demo
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    
    add_custom_target(clean-all
        COMMAND rm -rf ${CMAKE_SOURCE_DIR}/build
    )
endif()