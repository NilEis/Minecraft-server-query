#include "minecraft_server_query_api.h"

static char *errors[] =
    {
        "Already initialized",
        "Could not create address",
        "Could not create socket",
        "Could not connect socket"};

char *msq_get_error(int c)
{
    return errors[c - 1];
}