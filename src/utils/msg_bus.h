#ifndef MSG_BUS_H
#define MSG_BUS_H

#include <pthread.h>

#define MSG_BUS_MAX_MSG_SIZE 512
#define MSG_BUS_MAX_TOPIC_SIZE 512
#define MSG_BUS_QUEUE_SIZE 128
#define MSG_BUS_STATUS_RUNNING 1
#define MSG_BUS_STATUS_STOP 0
typedef struct
{
    char topic[MSG_BUS_MAX_TOPIC_SIZE];
    char payload[MSG_BUS_MAX_MSG_SIZE];
} msg_t;

typedef struct
{
    pthread_mutex_t lock;
    pthread_cond_t cond;

    msg_t queue[MSG_BUS_QUEUE_SIZE];
    int head;
    int tail;
    int count;

    int is_running;
} msg_bus_t;

/* 创建与销毁 */
int bus_init(msg_bus_t *bus);
void bus_deinit(msg_bus_t *bus);

/* 队列操作 */
int bus_push(msg_bus_t *bus, const msg_t *msg);
int bus_pop(msg_bus_t *bus, msg_t *msg); // 阻塞等待

/* 停止队列 (让阻塞pop()退出)*/
void bus_stop(msg_bus_t *bus);

#endif // MSG_BUS_H
