#ifndef CONNECTOR_IF_H
#define CONNECTOR_IF_H
typedef struct ops connector_ops_t;

typedef struct connector
{
    char name[32]; // "can", "modbus"
    void *context; // connector 私有数据
    connector_ops_t *ops;
} connector_t;

typedef struct ops
{
    int (*init)(struct connector *self, const char *config);
    int (*start)(struct connector *self);
    int (*stop)(struct connector *self);
    int (*destroy)(struct connector *self);
} connector_ops_t;

#endif // CONNECTOR_IF_H