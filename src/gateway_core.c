#include "connector/connector_if.h"

#include <pthread.h>

#define MAX_CONNECTORS 8

static connector_t *g_connectors[MAX_CONNECTORS];
static int g_connector_count = 0;
static pthread_mutex_t g_connector_mutex = PTHREAD_MUTEX_INITIALIZER;

int gateway_register_connector(connector_t *connector)
{
    pthread_mutex_lock(&g_connector_mutex);
    if (g_connector_count >= MAX_CONNECTORS)
    {
        pthread_mutex_unlock(&g_connector_mutex);
        return -1; // Max connectors reached
    }
    g_connectors[g_connector_count++] = connector;
    pthread_mutex_unlock(&g_connector_mutex);
    return 0; // Success
}

void gateway_start(void)
{
    for(int i = 0; i < g_connector_count; i++)
    {
        if (g_connectors[i]->ops && g_connectors[i]->ops->start)
        {
            g_connectors[i]->ops->start(g_connectors[i]);
        }
    }
}

void gateway_stop(void)
{
    for(int i = 0; i < g_connector_count; i++)
    {
        if (g_connectors[i]->ops && g_connectors[i]->ops->stop)
        {
            g_connectors[i]->ops->stop(g_connectors[i]);
        }
    }
}

void gateway_list_connectors(void)
{
    for(int i = 0; i < g_connector_count; i++)
    {
        printf("Connector %d: %s\n", i, g_connectors[i]->name);
    }
}
