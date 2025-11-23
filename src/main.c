// ble -> mqtt
#include "utils/msg_bus.h"
#include "net/mqtt/mqtt_client.h"
#include <stdio.h>
#include <unistd.h>
void main(void)
{
    printf("Hello World!\n");
    // mqtt_client_test();
    msg_bus_t tx_bus;
    msg_bus_t rx_bus;

    msg_t msg;

    bus_init(&tx_bus);
    bus_init(&rx_bus);

    mqtt_init(&tx_bus, &rx_bus);
    mqtt_start();
    for (;;)
    {
        // sleep(1);
        bus_pop(&rx_bus, &msg);
        printf("从云端收到: topic:%s\n 内容:%s\n", msg.topic, msg.payload);
    }
    return;
}