#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H


typedef struct
{
    char **sub_topics;
    int sub_count;
} mqtt_subscription_list_t;

int mqtt_client_test(void);

int mqtt_start(void);
// TODO: mqtt_reload_config 或者 mqtt_restart 或者 mqtt_reload_subscriptions
int mqtt_reload_subscriptions(mqtt_subscription_list_t *new_sub_list);

int mqtt_publish(const char *topic, const char *payload);
#endif // MQTT_CLIENT_H