#ifndef UDS_BUS_H
#define UDS_BUS_H

#include <stddef.h>

#define UDS_BUS_MAX_MSG_SIZE 512
#define UDS_BUS_MAX_TOPIC_SIZE 512

typedef struct
{
    char topic[UDS_BUS_MAX_TOPIC_SIZE];
    char payload[UDS_BUS_MAX_MSG_SIZE];
} uds_msg_t;

typedef struct
{
    int fd;
} uds_bus_t;

/* 创建一对相连的 UDS 端点：a <--> b */
int uds_create_pair(uds_bus_t *a, uds_bus_t *b);

/* 发送/接收（阻塞） */
int uds_send(uds_bus_t *ch, const uds_msg_t *msg);
int uds_recv(uds_bus_t *ch, uds_msg_t *msg);

/* 关闭通道 */
void uds_close(uds_bus_t *ch);

#endif // UDS_BUS_H
