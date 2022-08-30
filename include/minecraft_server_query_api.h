#ifndef MINECRAFT_SERVER_QUERY_API_H
#define MINECRAFT_SERVER_QUERY_API_H

int msq_init(char *ip, int port);
char *msq_get_error(int c);
void msq_free();

#endif // MINECRAFT_SERVER_QUERY_API_H
