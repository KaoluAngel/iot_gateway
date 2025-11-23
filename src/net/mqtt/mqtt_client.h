#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "../../utils/msg_bus.h"

typedef struct
{
    char **sub_topics;
    int sub_count;
} mqtt_config_t;

int mqtt_client_test(void);

int mqtt_init(msg_bus_t *tx_bus, msg_bus_t *rx_bus);
int mqtt_start(void);

#endif // MQTT_CLIENT_H