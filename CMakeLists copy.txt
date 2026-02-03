cmake_minimum_required(VERSION 3.10)

project(iot_gateway C)

# 推荐设置 C 标准（按需调整为 C99/C11 等）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)



# 使用 GLOB_RECURSE 自动查找（方便但需小心）
# 自动递归查找 src 下所有 .c 文件：
file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(iot_gateway ${SRC_FILES})


# ========== 第三方库（CMake 原生支持） ==========
add_subdirectory(vendor/cjson EXCLUDE_FROM_ALL)
add_subdirectory(vendor/paho.mqtt.c EXCLUDE_FROM_ALL)


# ========== libmodbus（Autotools 项目） ==========
include(ExternalProject)

set(LIBMODBUS_PREFIX "${CMAKE_BINARY_DIR}/libmodbus")
set(LIBMODBUS_INSTALL_DIR "${LIBMODBUS_PREFIX}/install")
set(LIBMODBUS_STATIC_LIB "${LIBMODBUS_INSTALL_DIR}/lib/libmodbus.a")

ExternalProject_Add(libmodbus_ext
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/libmodbus
  BINARY_DIR ${LIBMODBUS_PREFIX}/build
  PREFIX ${LIBMODBUS_PREFIX}
  INSTALL_DIR ${LIBMODBUS_INSTALL_DIR}
  
  # 关键修复1：统一在 SOURCE_DIR 执行 autogen，在 BINARY_DIR 执行 configure
  CONFIGURE_COMMAND 
    ${CMAKE_COMMAND} -E chdir <SOURCE_DIR> ./autogen.sh 
    COMMAND 
    ${CMAKE_COMMAND} -E chdir <BINARY_DIR>
    <SOURCE_DIR>/configure --prefix=${LIBMODBUS_INSTALL_DIR} --enable-static --disable-shared
  
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j${CMAKE_BUILD_PARALLEL_LEVEL}
  INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
  UPDATE_COMMAND ""
)
add_library(libmodbus_static STATIC IMPORTED GLOBAL)

# 确保包含目录存在
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/libmodbus/install/include")

set_target_properties(libmodbus_static PROPERTIES
  IMPORTED_LOCATION ${LIBMODBUS_STATIC_LIB}
  INTERFACE_INCLUDE_DIRECTORIES "${LIBMODBUS_INSTALL_DIR}/include"
)
add_dependencies(libmodbus_static libmodbus_ext)

# 2. 直接链接目标名（假设第三方 CMakeLists.txt 定义了这些目标）
target_link_libraries(iot_gateway PRIVATE cjson paho-mqtt3a libmodbus_static)