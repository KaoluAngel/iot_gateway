
#include <stdlib.h>
#include <string.h>
#include "can_connector.h"

typedef struct
{
    char iface[16]; // Interface name, e.g., "can0"
    // 这里放 can config, socket fd 等等
} can_ctx_t;

static int can_init(connector_t *self, const char *config)
{
    can_ctx_t *ctx = malloc(sizeof(can_ctx_t));
    if (!ctx)
    {
        return -1; // 内存分配失败
    }
#if 0
    // 解析 config 字符串，初始化 ctx
    strncpy(ctx->iface, config, sizeof(ctx->iface) - 1);
    ctx->iface[sizeof(ctx->iface) - 1] = '\0';
#else
    strcpy(ctx->iface, "can0"); // 默认使用 can0 接口
#endif
    // 这里可以添加更多的初始化代码，比如打开 socket 等等

    self->context = ctx;
    return 0; // 成功
}

static int can_start(connector_t *self)
{
    can_ctx_t *ctx = (can_ctx_t *)self->context;
    // 这里添加启动 CAN 连接的代码
    // 例如，打开 socket，绑定接口等
    // 创建线程接收数据等
    return 0; // 成功
}

static int can_stop(connector_t *self)
{
    can_ctx_t *ctx = (can_ctx_t *)self->context;
    // 这里添加停止 CAN 连接的代码
    // 例如，关闭 socket，停止线程等
    return 0; // 成功
}

static void can_destroy(connector_t *self)
{
    if (self->context)
    {
        free(self->context);
    }
    if (self)
    {
        free(self);
    }
}

static connector_ops_t can_ops = {
    .init = can_init,
    .start = can_start,
    .stop = can_stop,
    .destroy = can_destroy,
};

connector_t *create_can_connector(void)
{
#if 0
    connector_t *conn = malloc(sizeof(connector_t));
    if (!conn)
    {
        return NULL; // 内存分配失败
    }
    strncpy(conn->name, "can", sizeof(conn->name) - 1);
    conn->name[sizeof(conn->name) - 1] = '\0';
    conn->context = NULL;
    conn->ops = &can_ops;
    return conn;

#else
    connector_t *c = calloc(1, sizeof(connector_t));
    if (c == NULL)
    {
        return NULL;
    }
    strcpy(c->name, "can");
    c->ops = &can_ops;
    return c;
#endif
}