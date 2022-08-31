#include <stdio.h>
#include "minecraft_server_query_api.h"

void main(int argc, char **argv)
{
    char *ip = "SERVER_IP";
    int port = /*PORT*/0;
    if (argc > 1)
    {
        ip = argv[1];
        sscanf(argv[2], "%d", &port);
    }
    int ret = msq_init(ip, port);
    if (ret != 0)
    {
        printf("Error: %s\n", msq_get_error(ret));
    }
    msq_basic_stats_t bp;
    ret = msq_get_basic_stats(&bp);
    if (ret != 0)
    {
        printf("Error: %s\n", msq_get_error(ret));
    }
    else
    {
        msq_print_basic_stats(&bp);
        msq_free_basic_stats(&bp);
    }
    msq_full_stats_t fp;
    ret = msq_get_full_stats(&fp);
    if (ret != 0)
    {
        printf("Error: %s\n", msq_get_error(ret));
    }
    else
    {
        msq_print_full_stats(&fp);
        msq_free_full_stats(&fp);
    }
    msq_free();
}