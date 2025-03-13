#ifndef UDSCAST_SOCKET_H
#define UDSCAST_SOCKET_H 1

#include <stddef.h>

typedef struct sockArr_t
{
	/* Socket descriptors */
	int *fds;
	/* Socket count */
	int count;
	/* Maximum number of sockets before reallocation needed */
	int max;
} sockArr;

int createSockArr(sockArr *arr);
int addNewClient(sockArr *arr, int fd);
int removeClient(sockArr *arr, int idx);

int procMsg(sockArr *arr, int idx);

#endif

