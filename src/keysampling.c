/*
 *  key sampling
 */

#include "redis.h"
#include "keysampling.h"

const char *KH_DEFAULT_HOST = "127.0.0.1";
const int KH_DEFAULT_PORT = 12345;

void sendViaUdp(struct redisServer *srv, char *message) {
	sendto(srv->key_sampling_sock, message, strlen(message), 0,
			(struct sockaddr *)(srv->key_sampling_addr),
			sizeof(*(srv->key_sampling_addr)));
}

void setupUdpSocket(struct redisServer *srv, char *host, int port) {
	if (srv->key_sampling_connected)
		close(srv->key_sampling_sock);

	if (host == NULL)
		host = (char *)KH_DEFAULT_HOST;

	if (port == -1)
		port = KH_DEFAULT_PORT;


	struct sockaddr_in *servaddr;
	servaddr = zmalloc(sizeof(servaddr));
	bzero(servaddr, sizeof(servaddr));
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = inet_addr(host);
	servaddr->sin_port = htons(port);
	srv->key_sampling_sock = sock;
	srv->key_sampling_addr = servaddr;

	srv->key_sampling_connected = 1;
	printf("Setup UDP socket to %s:%d\n", host, port);
}

void emitKey(char *cmd, char *key, int hit) {
	//printf("emitKey(%s, %s, %d)\n", cmd, key, hit);
	char buf[128];  // XXX: how long can redis keys be?
	bzero(buf, 128);
	snprintf(buf, sizeof(buf)-1, "%s %s %d\n", cmd, key, hit);

	sendViaUdp(&server, buf);
}

