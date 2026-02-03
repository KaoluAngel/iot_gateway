#include "uds_bus.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int uds_create_pair(uds_bus_t *a, uds_bus_t *b)
{
    if (!a || !b)
        return -1;
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sv) != 0)
    {
        if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sv) != 0)
            return -1;
    }
    a->fd = sv[0];
    b->fd = sv[1];
    return 0;
}

int uds_send(uds_bus_t *ch, const uds_msg_t *msg)
{
    if (!ch || ch->fd < 0 || !msg)
        return -1;
    ssize_t n = send(ch->fd, msg, sizeof(uds_msg_t), 0);
    return (n == (ssize_t)sizeof(uds_msg_t)) ? 0 : -1;
}

int uds_recv(uds_bus_t *ch, uds_msg_t *msg)
{
    if (!ch || ch->fd < 0 || !msg)
        return -1;
    ssize_t n = recv(ch->fd, msg, sizeof(uds_msg_t), 0);
    return (n == (ssize_t)sizeof(uds_msg_t)) ? 0 : -1;
}

void uds_close(uds_bus_t *ch)
{
    if (!ch)
        return;
    if (ch->fd >= 0)
    {
        close(ch->fd);
        ch->fd = -1;
    }
}
