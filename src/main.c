#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "plibsys.h"
#include "minecraft_server_query_api.h"

#define SWAP_U32(x) ((((x) >> 24) & 0xff) | (((x) << 8) & 0xff0000) | (((x) >> 8) & 0xff00) | (((x) << 24) & 0xff000000))
#define SWAP_I16(x) (((x) << 8) | (((x) >> 8) & 0xFF))
#define SWAP_U16(x) (((x) << 8) | ((x) >> 8))
#define PARSE_KEY(x)                \
    do                              \
    {                               \
        i += len + 1;               \
        len = strlen(&response[i]); \
        i += len + 1;               \
        len = strlen(&response[i]); \
        x = (char *)malloc(len+1);    \
        strcpy(x, &response[i]);    \
    } while (0);

#define PARSE_KEY_SCANF(x, f)              \
    do                                     \
    {                                      \
        i += len + 1;                      \
        len = strlen(&response[i]);        \
        i += len + 1;                      \
        len = strlen(&response[i]);        \
        sscanf(&response[i], "%" f, &(x)); \
    } while (0);

#ifndef NDEBUG
#define LOG(x, ...) printf(x, __VA_ARGS__)
#else
#define LOG(x, ...) ;
#endif

PSocketAddress *addr = NULL;
PSocket *sock = NULL;
int init = 0;

uint32_t challenge_token = 0;

static const uint32_t SESSION_ID = (0xDEAD & 0x0F0F0F0F);

static int __handshake();

int check_for_endianness()
{
    unsigned int x = 1;
    char *c = (char *)&x;
    return (int)*c;
}

int msq_init(char *ip, int port)
{
    if (init != 0)
    {
        return 1;
    }
    init = 1;
    p_libsys_init();
    addr = p_socket_address_new(ip, port);
    if (addr == NULL)
    {
        return 2;
    }
    sock = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_DATAGRAM, P_SOCKET_PROTOCOL_UDP, NULL);
    if (sock == NULL)
    {
        p_socket_address_free(addr);
        return 3;
    }
    if (!p_socket_connect(sock, addr, NULL))
    {
        p_socket_address_free(addr);
        p_socket_free(sock);
        return 4;
    }
    p_socket_set_timeout(sock, 1000);
    return __handshake();
}

static int __handshake()
{
    static struct __attribute__((__packed__))
    {
        uint8_t type;
        int32_t session_id;
        char challenge_token_str[16];
    } handshake_response;
    static const pchar hndshke[] = {0xFE, 0xFD, 0x09, 0xFF & (SESSION_ID >> 24), 0xFF & (SESSION_ID >> 16), 0xFF & (SESSION_ID >> 8), 0xFF & SESSION_ID};
    static const int hndshke_length = sizeof(hndshke);
    if (p_socket_send(sock, hndshke, hndshke_length, NULL) == -1)
    {
        return 5;
    }
    if (p_socket_receive(sock, (pchar *)(&handshake_response), sizeof(handshake_response), NULL) == -1)
    {
        return 6;
    }
    sscanf(handshake_response.challenge_token_str, "%" SCNu32, &challenge_token);
    if (check_for_endianness() == 0)
    {
        challenge_token = SWAP_U32(challenge_token);
    }
    else
    {
        handshake_response.session_id = SWAP_U32(handshake_response.session_id);
    }
    LOG("type: %" PRIu8 "\nsession_id: 0x%" PRIX32 "\nchallenge_token_str: %s -> 0x%" PRIX32 "\n", handshake_response.type, handshake_response.session_id, handshake_response.challenge_token_str, challenge_token);
    return 0;
}

int msq_get_basic_stats(msq_basic_stats_t *ret)
{
    pchar send[] = {0xFE, 0xFD,
                    0x00, 0xFF & (SESSION_ID >> 24), 0xFF & (SESSION_ID >> 16), 0xFF & (SESSION_ID >> 8), 0xFF & SESSION_ID,
                    0xFF & (challenge_token >> 24), 0xFF & (challenge_token >> 16), 0xFF & (challenge_token >> 8), 0xFF & challenge_token};
    int send_length = sizeof(send);
    int lock = 0;
#define RESPONSE_BUFFER_SIZE 256
REQUEST:
    if (p_socket_send(sock, send, send_length, NULL) == -1)
    {
        return 7;
    }
    pchar response[RESPONSE_BUFFER_SIZE] = {0};
    if (p_socket_receive(sock, response, sizeof(response), NULL) == -1)
    {
        if (lock)
        {
            return 8;
        }
        lock = 1;
        int ret = __handshake();
        if (ret != 0)
        {
            return ret;
        }
        goto REQUEST;
    }
    int i = 5;
    size_t len = strlen(&response[i]);
    ret->MoTD = (char *)malloc(len + 1);
    strcpy(ret->MoTD, &response[i]);
    i += len + 1;
    len = strlen(&response[i]);
    ret->gametype = (char *)malloc(len + 1);
    strcpy(ret->gametype, &response[i]);
    i += len + 1;
    len = strlen(&response[i]);
    ret->map = (char *)malloc(len + 1);
    strcpy(ret->map, &response[i]);
    i += len + 1;
    len = strlen(&response[i]);
    sscanf(&response[i], "%" SCNi32, &(ret->num_players));
    i += len + 1;
    len = strlen(&response[i]);
    sscanf(&response[i], "%" SCNi32, &(ret->max_players));
    i += len + 1;
    ret->hostport = *((uint16_t *)&(response[i]));
    if (check_for_endianness() == 0)
    {
        ret->hostport = SWAP_U16(ret->hostport);
    }
    i += 2;
    len = strlen(&response[i]);
    ret->hostip = (char *)malloc(len + 1);
    strcpy(ret->hostip, &response[i]);
    return 0;
#undef RESPONSE_BUFFER_SIZE
}

int msq_get_full_stats(msq_full_stats_t *ret)
{
#define RESPONSE_BUFFER_SIZE 256
    pchar send[] = {0xFE, 0xFD,
                    0x00, 0xFF & (SESSION_ID >> 24), 0xFF & (SESSION_ID >> 16), 0xFF & (SESSION_ID >> 8), 0xFF & SESSION_ID,
                    0xFF & (challenge_token >> 24), 0xFF & (challenge_token >> 16), 0xFF & (challenge_token >> 8), 0xFF & challenge_token,
                    0x00, 0x00, 0x00, 0x00};
    int send_length = sizeof(send);
    int lock = 0;
#define RESPONSE_BUFFER_SIZE 1024
REQUEST:
    if (p_socket_send(sock, send, send_length, NULL) == -1)
    {
        return 9;
    }
    pchar response[RESPONSE_BUFFER_SIZE] = {0};
    if (p_socket_receive(sock, response, sizeof(response), NULL) == -1)
    {
        if (lock)
        {
            return 10;
        }
        lock = 1;
        int ret = __handshake();
        if (ret != 0)
        {
            return ret;
        }
        goto REQUEST;
    }
    int i = 16;
    size_t len = strlen(&response[i]);
    i += len + 1;
    len = strlen(&response[i]);
    ret->hostname = (char *)malloc(len+1);
    strcpy(ret->hostname, &response[i]);
    PARSE_KEY(ret->game_type);
    PARSE_KEY(ret->game_id);
    PARSE_KEY(ret->version);
    PARSE_KEY(ret->plugins);
    PARSE_KEY(ret->map);
    PARSE_KEY_SCANF(ret->num_players, SCNu32);
    PARSE_KEY_SCANF(ret->max_players, SCNu32);
    PARSE_KEY_SCANF(ret->hostport, SCNu16);
    PARSE_KEY(ret->hostip);
    i += len + 12;
    ret->players = (char **)malloc(ret->num_players * sizeof(char *));
    for (int p = 0; p < ret->num_players; p++)
    {
        len = strlen(&response[i]);
        ret->players[p] = (char *)malloc(len+1);
        strcpy(ret->players[p], &response[i]);
        i += len + 1;
    }
#undef RESPONSE_BUFFER_SIZE
    return 0;
}

void msq_print_basic_stats(msq_basic_stats_t *s)
{
    printf("MoTD: %s\n", s->MoTD);
    printf("gametype: %s\n", s->gametype);
    printf("map: %s\n", s->map);
    printf("num_players: %" PRIi32 "\n", s->num_players);
    printf("max_players: %" PRIi32 "\n", s->max_players);
    printf("hostport: %" PRIu16 "\n", s->hostport);
    printf("hostip: %s\n", s->hostip);
}

void msq_print_full_stats(msq_full_stats_t *s)
{
    printf("hostname: %s\n", s->hostname);
    printf("game_type: %s\n", s->game_type);
    printf("game_id: %s\n", s->game_id);
    printf("version: %s\n", s->version);
    printf("plugins: %s\n", s->plugins);
    printf("map: %s\n", s->map);
    printf("num_players: %" PRIi32 "\n", s->num_players);
    printf("max_players: %" PRIi32 "\n", s->max_players);
    printf("hostport: %" PRIu16 "\n", s->hostport);
    printf("hostip: %s\n", s->hostip);
    printf("players:\n");
    for (int i = 0; i < s->num_players; i++)
    {
        printf("\t%s\n", s->players[i]);
    }
}
void msq_free_full_stats(msq_full_stats_t *p)
{
    free(p->hostname);
    free(p->game_type);
    free(p->game_id);
    free(p->version);
    free(p->plugins);
    free(p->map);
    free(p->hostip);
    for (int i = 0; i < p->num_players; i++)
    {
        free(p->players[i]);
    }
    free(p->players);
}

void msq_free_basic_stats(msq_basic_stats_t *p)
{
    free(p->MoTD);
    free(p->gametype);
    free(p->map);
    free(p->hostip);
}

void msq_free()
{
    p_socket_address_free(addr);
    p_socket_close(sock, NULL);
    p_socket_free(sock);
    p_libsys_shutdown();
}