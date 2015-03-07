/*
 *  keyhit sampling ... blargh
 */

#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "keyhit.h"

const char *KH_DEFAULT_HOST = "127.0.0.1";
const int KH_DEFAULT_PORT = 12345;

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

void emitKeyhit(char *key, int hit) {
	char buf[128];  // XXX: how long can redis keys be?
	bzero(buf, 128);
	snprintf(buf, sizeof(buf)-1, "%s\n", key);
    // XXX: use `hit` value? configurable whether we send it along?
	sendViaUdp(buf, (char *)KH_DEFAULT_HOST, KH_DEFAULT_PORT);
}

