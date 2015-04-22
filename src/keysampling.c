/*
 *  key sampling
 */

#include "redis.h"
#include "keysampling.h"

const char *KH_DEFAULT_HOST = "127.0.0.1";
const int KH_DEFAULT_PORT = 12345;

const char *hitmiss[] = {"miss", "hit"};

void sendViaUdp(char *message, char *host, int port) {
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));

	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(host);
	servaddr.sin_port = htons(port);
	sendto(sock, message, strlen(message), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    close(sock);
}

void emitKey(char *cmd, char *key, int hit) {
  //printf("emitKey\n");
	char buf[128];  // XXX: how long can redis keys be?
	bzero(buf, 128);
	snprintf(buf, sizeof(buf)-1, "%s %s %s\n", cmd, key, hitmiss[hit]);
    char *host = (char *)KH_DEFAULT_HOST;
    int port = KH_DEFAULT_PORT;
    if (server.key_sampling_host != NULL)
        host = server.key_sampling_host;

    if (server.key_sampling_port != -1)
        port = server.key_sampling_port;

	sendViaUdp(buf, host, port);
}

