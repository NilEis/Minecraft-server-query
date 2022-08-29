#include "minecraft_server_query_api.h"

const static char **errors =
    {
        "Already initialized",
        "Could not create address",
        "Could not create socket",
        "Could not bind socket"
        };

char *mcq_get_error(int c)
{
    return errors[c-1];
}