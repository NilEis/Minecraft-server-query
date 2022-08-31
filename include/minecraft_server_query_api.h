#ifndef MINECRAFT_SERVER_QUERY_API_H
#define MINECRAFT_SERVER_QUERY_API_H

#include <stdint.h>
#include <inttypes.h>

typedef struct
{
    char *MoTD;
    char *gametype;
    char *map;
    int32_t num_players;
    int32_t max_players;
    uint16_t hostport;
    char *hostip;
} msq_basic_stats_t;

typedef struct
{
    char *hostname;
    char *game_type;
    char *game_id;
    char *version;
    char *plugins;
    char *map;
    int32_t num_players;
    int32_t max_players;
    uint16_t hostport;
    char *hostip;
    char **players;
} msq_full_stats_t;

int msq_init(const char *ip, int port);
int msq_get_basic_stats(msq_basic_stats_t *ret);
int msq_get_full_stats(msq_full_stats_t *ret);
char *msq_get_error(int c);
void msq_print_basic_stats(msq_basic_stats_t *s);
void msq_free_basic_stats(msq_basic_stats_t *p);
void msq_print_full_stats(msq_full_stats_t *s);
void msq_free_full_stats(msq_full_stats_t *p);
void msq_free();

#endif // MINECRAFT_SERVER_QUERY_API_H
