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


# 1. 添加子目录（会构建第三方库）
# 只构建你显式依赖的目标，不构建测试/示例
add_subdirectory(vendor/cjson EXCLUDE_FROM_ALL)
add_subdirectory(vendor/paho.mqtt.c EXCLUDE_FROM_ALL)

# 2. 直接链接目标名（假设第三方 CMakeLists.txt 定义了这些目标）
target_link_libraries(iot_gateway PRIVATE cjson paho-mqtt3a)