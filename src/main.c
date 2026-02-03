#include "utils/uds_bus.h"
#include "mqtt/mqtt_client.h"
#include <stdio.h>
#include <unistd.h>
void main(void)
{
    printf("Hello World!\n");
    // mqtt_client_test();
    uds_bus_t tx_bus;
    uds_bus_t rx_bus;

    uds_msg_t msg;

    uds_create_pair(&tx_bus, &rx_bus);

    mqtt_init(&tx_bus, &rx_bus);
    mqtt_start();
    for (;;)
    {
        uds_recv(&rx_bus, &msg);
        printf("从云端收到: topic:%s\n 内容:%s\n", msg.topic, msg.payload);
    }
    return;
}