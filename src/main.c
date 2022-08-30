#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include "plibsys.h"
#include "minecraft_server_query_api.h"

#define SWAP_32(x) ((((x) >> 24) & 0xff) | (((x) << 8) & 0xff0000) | (((x) >> 8) & 0xff00) | (((x) << 24) & 0xff000000))

PSocketAddress *addr = NULL;
PSocket *sock = NULL;
int init = 0;

uint32_t challenge_token = 0;

static const uint32_t SESSION_ID = (0xDEAD & 0x0F0F0F0F);

void __handshake();

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
    __handshake();
    return 0;
}

void __handshake()
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
        printf("Could not send handshake\n");
        return;
    }
    if (p_socket_receive(sock, (pchar *)(&handshake_response), sizeof(handshake_response), NULL) == -1)
    {
        printf("Could not receive handshake\n");
        return;
    }
    sscanf(handshake_response.challenge_token_str, "%" SCNu32, &challenge_token);
    if (check_for_endianness() == 0)
    {
        challenge_token = SWAP_32(challenge_token);
    }
    else
    {
        handshake_response.session_id = SWAP_32(handshake_response.session_id);
    }
    printf("type: %" PRIu8 "\nsession_id: 0x%" PRIX32 "\nchallenge_token_str: %s -> 0x%" PRIX32 "\n", handshake_response.type, handshake_response.session_id, handshake_response.challenge_token_str, challenge_token);
}

int msq_get()
{

}

void msq_free()
{
    p_socket_address_free(addr);
    p_socket_close(sock, NULL);
    p_socket_free(sock);
    p_libsys_shutdown();
}