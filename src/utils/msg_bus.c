#include "msg_bus.h"
#include <string.h>
#include <stdlib.h>

/* 创建与销毁 */
int bus_init(msg_bus_t *bus)
{
    // TODO: 不负责分配内存

    if (!bus)
        return -1;
    memset(bus, 0, sizeof(msg_bus_t));
    pthread_mutex_init(&bus->lock, NULL);
    pthread_cond_init(&bus->cond, NULL);

    bus->head = 0;
    bus->tail = 0;
    bus->is_running = MSG_BUS_STATUS_RUNNING;

    return 0;
}
void bus_deinit(msg_bus_t *bus)
{
    if (!bus)
        return;
    bus->is_running = MSG_BUS_STATUS_STOP;
    // 唤醒所有等待的线程
    pthread_cond_broadcast(&bus->cond);

    pthread_mutex_destroy(&bus->lock);
    pthread_cond_destroy(&bus->cond);
}

/* 队列操作 */
int bus_push(msg_bus_t *bus, const msg_t *msg)
{
    pthread_mutex_lock(&bus->lock);
    if (!bus->is_running)
    {
        pthread_mutex_unlock(&bus->lock);
        return -1;
    }

    // 如果队列满
    if (bus->count == MSG_BUS_QUEUE_SIZE)
    {
        // 覆盖最老的(head)
        bus->head = (bus->head + 1) % MSG_BUS_QUEUE_SIZE;
        // TODO: 这里为啥要--? 因为下面count又++了
        bus->count--;
    }
    /*
    这是 结构体值拷贝（深拷贝），并不是保存指针。
    也就是说，Bus 队列里保存的是 msg 的 完整副本。
    无论你外面的 msg 变量是不是：
    局部变量
    栈变量
    临时变量
    函数退出就销毁
    都不影响 Bus 队列里的内容。
    */
    bus->queue[bus->tail] = *msg;
    bus->tail = (bus->tail + 1) % MSG_BUS_QUEUE_SIZE;
    bus->count++;

    pthread_cond_signal(&bus->cond);
    pthread_mutex_unlock(&bus->lock);
    return 0;
}
int bus_pop(msg_bus_t *bus, msg_t *msg) // 阻塞等待
{
    pthread_mutex_lock(&bus->lock);

    for (; bus->count == 0 && bus->is_running == MSG_BUS_STATUS_RUNNING;)
    {
        pthread_cond_wait(&bus->cond, &bus->lock);
    }
    if (bus->is_running != MSG_BUS_STATUS_RUNNING)
    {
        pthread_mutex_unlock(&bus->lock);
        return -1;
    }

    *msg = bus->queue[bus->head];
    bus->head = (bus->head + 1) % MSG_BUS_QUEUE_SIZE;
    bus->count--;

    pthread_mutex_unlock(&bus->lock);
    return 0;
}

/* 停止队列 (让阻塞pop()退出)*/
void bus_stop(msg_bus_t *bus)
{
    pthread_mutex_lock(&bus->lock);
    bus->is_running = MSG_BUS_STATUS_STOP;
    pthread_cond_broadcast(&bus->cond);
    pthread_mutex_unlock(&bus->lock);
}