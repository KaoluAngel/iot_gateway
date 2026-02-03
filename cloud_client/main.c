#include <stdio.h>
#include "log/log.h"
#include "config/config_yaml.h"
#include "mqtt/mqtt_client.h"
#include <unistd.h>

// 如果以后封装了 paho-mqtt，可以放这里
// #include "mqtt/mqtt_client.h"

int main()
{
    printf("Cloud client running...\n");

    LOG_INFO("Cloud client started");
    mqtt_start();
    static int count = 0;
    static char payload[128];
    while (1)
    {
        snprintf(payload, sizeof(payload), "Hello, MQTT! %d", count++);
        mqtt_publish("test/topic", payload);
        // log_info("Cloud client heartbeat...");
        sleep(2);
    }

    return 0;
}
