add_executable(cloud_client main.c mqtt/mqtt_client.c)

target_link_libraries(cloud_client 
    common
    paho-mqtt3a
)