#include <stddef.h>
#include "pmacros.h"
#include "pmain.h"
#include "psocket.h"
#include "minecraft_server_query_api.h"

PSocketAddress *addr = NULL;
PSocket *sock = NULL;
int init = 0;

void __handshake();

int mcq_init(char *ip, int port)
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
    if (!p_socket_bind(sock, addr, FALSE, NULL))
    {
        p_socket_address_free(addr);
        p_socket_free(sock);
        return 4;
    }
    __handshake();
}

void __handshake()
{
    static const pchar hndshke[] = {0xFE, 0xFD, 0x09, 0x00, 0x00, 0x00, 0x01};
    static const int hndshke_length = sizeof(hndshke);
    if(p_socket_send(sock, hndshke, hndshke_length, NULL)==-1)
    {
        printf("Could not send handshake\n");
    }
    p_socket_receive
}

void mcq_free()
{
    p_socket_address_free(addr);
    p_socket_close(sock, NULL);
    p_socket_free(sock);
    p_libsys_shutdown();
}