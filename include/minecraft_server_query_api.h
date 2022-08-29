#ifndef MINECRAFT_SERVER_QUERY_API_H
#define MINECRAFT_SERVER_QUERY_API_H

int mcq_init(char *ip, int port);
char *mcq_get_error(int c);
void mcq_free();

#endif // MINECRAFT_SERVER_QUERY_API_H
