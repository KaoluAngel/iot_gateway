// 把本地消息总线上面的所有其他协议的消息转发到MQTT
// 基于Paho MQTT C Client Library
#include "mqtt_client.h"
#include "MQTTAsync.h"
#include "MQTTClient.h"
#
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

// 认证信息 - 在实际项目中应该从配置文件或安全存储中读取    
// #define MQTT_USERNAME "weiqing"
// #define MQTT_PASSWORD "1234"

static void publish(MQTTAsync client, char *topic, char *payload)
{
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    // 异步发布
    MQTTAsync_sendMessage(client, TOPIC, &pubmsg, NULL);
    printf("📤 消息已发送: %s\n", payload);
}

void onSubscribeSuccess(void *context, MQTTAsync_successData *response)
{
    printf("Subscribe succeeded, waiting for messages...\n");
}
void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    printf("Subscribe failed, rc %d\n", response ? response->code : 0);
    // finished = 1;
}

static void onConnectSuccess(void *context, MQTTAsync_successData *response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    printf("Connect succeeded\n");

    // 连接成功后订阅主题
    opts.onSuccess = onSubscribeSuccess;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;

    rc = MQTTAsync_subscribe(client, TOPIC, QOS, &opts);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        // finished = 1;
    }
    printf("✅ 连接成功，准备发布消息...\n");
}

// 连接失败回调
static void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
    printf("❌ 连接失败，错误码: %d\n", response ? response->code : -1);
    assert(0);
}

// 1. 消息到达回调（核心接收函数）
int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    printf("Message arrived\n");
    printf("  Topic: %s\n", topicName);
    printf("  Payload: %.*s\n", message->payloadlen, (char *)message->payload);
    printf("  QoS: %d\n", message->qos);

    // 必须释放消息内存
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return 1; // 返回1表示消息已处理
}

int mqtt_client_test(void)
{
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    // 1. 创建客户端
    int rc = MQTTAsync_create(&client, SERVER_ADDRESS, CLIENT_ID,
                              MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to create client\n");
        return 1;
    }

    // 2. 配置连接参数
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnectSuccess;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    // conn_opts.username = MQTT_USERNAME;
    // conn_opts.password = MQTT_PASSWORD;
    MQTTAsync_setCallbacks(client, client, NULL, messageArrived, NULL);
    // 3. 发起异步连接
    printf("正在连接MQTT Broker: %s\n", SERVER_ADDRESS);
    rc = MQTTAsync_connect(client, &conn_opts);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect\n");
        MQTTAsync_destroy(&client);
        return 1;
    }

    // 4. 保持程序运行，等待回调执行
    printf("等待连接建立...\n");
    // sleep(5);

    char payload[64];
    for (double i = 25.6;; i = i + 0.5)
    {
        sleep(2);
        snprintf(payload, sizeof(payload), "{\"temp\": %.1f, \"unit\": \"C\"}", i);
        publish(client, TOPIC, payload);
    }

    // 5. 断开连接
    MQTTAsync_disconnect(client, NULL);
    MQTTAsync_destroy(&client);

    return 0;
}