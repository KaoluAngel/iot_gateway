// 把本地消息总线上面的所有其他协议的消息转发到MQTT
// 基于Paho MQTT C Client Library
#include "mqtt_client.h"
#include "MQTTAsync.h"
#include "MQTTClient.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define wq
#ifndef wq
#define SERVER_ADDRESS "tcp://broker.emqx.io:1883" // 公共测试Broker
#define CLIENT_ID "mqttx_3a7fffd6"
#else
#define SERVER_ADDRESS "tcp://ika.0w0.fun:1883"
#define CLIENT_ID "c_publisher_001"
#endif
#define TOPIC "sensor/temperature"
#define QOS 1
#define TIMEOUT 10000L

static const char *default_sub_topics[] = {
    "/gw/cmd/#",
    "/gw/config/reload",
    "/gw/upgrade/#",
};

static MQTTAsync g_client;
static pthread_t g_pub_thread;

static mqtt_subscription_list_t g_mqtt_cfg = {
    .sub_topics = (char **)default_sub_topics,
    .sub_count = sizeof(default_sub_topics) / sizeof(default_sub_topics[0]),

};

// 认证信息 - 在实际项目中应该从配置文件或安全存储中读取
// #define MQTT_USERNAME "weiqing"
// #define MQTT_PASSWORD "1234"
#if 0
volatile MQTTClient_deliveryToken deliveredtoken;
#endif
static void _mqtt_subscribe_all()
{
    int rc;
    for (int i = 0; i < g_mqtt_cfg.sub_count; i++)
    {
        const char *topic = g_mqtt_cfg.sub_topics[i];
        rc = MQTTAsync_subscribe(g_client, topic, 1, NULL);
        if (rc != MQTTASYNC_SUCCESS)
        {
            printf("订阅失败:%s, rc=%d\n", topic, rc);
        }
        else
        {
            printf("订阅成功:%s\n", topic);
        }
    }
}
static void delivered(void *context, MQTTClient_deliveryToken dt)
{
    #ifdef DEBUG
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
    #endif
}

static int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    // TODO: 这里怎么搞? 放进消息队列里面吗?

#if 1
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char *)message->payload);
#endif
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    // MQTTClient_freeMessage(&message);
    // MQTTClient_free(topicName);
    return 1;
}

static void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

static void publish(MQTTAsync client, const char *topic, const char *payload)
{
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    // 准备消息
    pubmsg.payload = (void *)payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    // 异步发布，并检查返回码便于排查
    rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("发布失败，主题:%s rc=%d\n", topic, rc);
    }
    else
    {
        // printf("📤 消息已发送: %s\n", payload);
    }
}

static void onSubscribeSuccess(void *context, MQTTAsync_successData *response)
{
    printf("Subscribe succeeded, waiting for messages...\n");
}
static void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    printf("Subscribe failed, rc %d\n", response ? response->code : 0);
    // finished = 1;
}

// 连接失败回调
static void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
    printf("❌ 连接失败，错误码: %d\n", response ? response->code : -1);
    assert(0);
}

static void onConnectSuccess(void *context, MQTTAsync_successData *response)
{
    printf("✅ 连接成功，开始订阅配置中的主题...\n");
    _mqtt_subscribe_all();
}
int mqtt_client_test(void)
{
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;
    // 1. 创建客户端
    rc = MQTTAsync_create(&client, SERVER_ADDRESS, CLIENT_ID,
                          MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to create client\n");
        return 1;
    }
    // 配置回调
    rc = MQTTAsync_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        goto destroy_exit;
    }

    // 2. 配置连接参数
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    // 下面非必需
    conn_opts.onSuccess = onConnectSuccess;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    // conn_opts.username = MQTT_USERNAME;
    // conn_opts.password = MQTT_PASSWORD;

    // 3. 发起异步连接
    printf("正在连接MQTT Broker: %s\n", SERVER_ADDRESS);
    rc = MQTTAsync_connect(client, &conn_opts);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, %d\n", rc);
        goto destroy_exit;
    }

    // 订阅
    // 一直订阅直到成功?

    printf("订阅 主题: %s\n client_id:%s Qos%d\n\n", TOPIC, CLIENT_ID, QOS);
    for (rc = -1; rc != MQTTASYNC_SUCCESS;)
    {
        rc = MQTTAsync_subscribe(client, TOPIC, QOS, NULL);
        if (rc != MQTTASYNC_SUCCESS)
        {
            printf("订阅主题失败:%d\n", rc);
            sleep(1);
        }

        else
            printf("订阅成功!\n");
    }
    // end 订阅

    // 发布
    char payload[64];
    for (double i = 25.6;; i = i + 0.5)
    {
        sleep(2);
        snprintf(payload, sizeof(payload), "{\"temp\": %.1f, \"unit\": \"C\"}", i);
        publish(client, TOPIC, payload);
    }

    // end 发布
    //  5. 断开连接
    rc = MQTTAsync_disconnect(client, NULL);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("断开连接失败:%d\n", rc);
    }

destroy_exit:
    MQTTAsync_destroy(&client);
exit:

    return rc;
}



int mqtt_start(void)
{
    int rc;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    // 1. 创建客户端
    rc = MQTTAsync_create(&g_client, SERVER_ADDRESS, CLIENT_ID,
                          MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to create client\n");
        return -1;
    }
    // 配置回调
    rc = MQTTAsync_setCallbacks(g_client, NULL, connlost, msgarrvd, delivered);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        return -2;
    }

    // 2. 配置连接参数
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    // 下面非必需
    conn_opts.onSuccess = onConnectSuccess;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = g_client;
    // conn_opts.username = MQTT_USERNAME;
    // conn_opts.password = MQTT_PASSWORD;

    // 3. 发起异步连接
    printf("正在连接MQTT Broker: %s\n", SERVER_ADDRESS);
    rc = MQTTAsync_connect(g_client, &conn_opts);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, %d\n", rc);
        return -3;
    }

    // TODO: 订阅呢? 放在哪里?
}

int mqtt_publish(const char *topic, const char *payload)
{
    if (!g_client)
    {
        printf("mqtt client 未启动，无法发布\n");
        return -1;
    }
    publish(g_client, topic, payload);
    return 0;
}