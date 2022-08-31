#include "minecraft_server_query_api.h"

static char *errors[] =
    {
        "Already initialized",
        "Could not create address",
        "Could not create socket",
        "Could not connect socket",
        "Could not send handshake",
        "Could not receive handshake",
        "Could not send request for basic stats",
        "Could not receive basic stats",
        "Could not send request for full stats",
        "Could not receive full stats"};

char *msq_get_error(int c)
{
    return errors[c - 1];
}