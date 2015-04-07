/*
 * keyhit sampling header
 */

#ifndef __KEYSAMPLING_H__
#define __KEYSAMPLING_H__
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* Exported API */
void emitKey(char *cmd, char *key, int hit);
#endif
