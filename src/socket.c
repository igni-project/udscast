#include "socket.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>

#define MSG_BUFFER_SIZE 1024

/* buffer for recv() to put received data in */
char buffer[MSG_BUFFER_SIZE];

int createSockArr(sockArr* arr)
{
	arr->count = 0;
	arr->max = 1;
	arr->fds = malloc(sizeof(int));
	if (!arr->fds)
	{
		perror("Failed to allocate space for socket descriptor array");
		return -1;
	}

	return 0;
}

int addNewClient(sockArr *arr, int fd)
{
	/* The reallocated address is audited before use */
	int *newAddr;

	if (arr->count >= arr->max)
	{
		arr->max *= 2;
		newAddr = realloc(arr->fds, arr->max);

		if (!newAddr)
		{
			perror("addNewClient() failed: could not allocate memory");
			return -1;
		}

		arr->fds = newAddr;
	}

	arr->fds[arr->count] = fd;

	++arr->count;

	return 0;
}

int removeClient(sockArr *arr, int idx)
{
	/* The reallocated address is audited before use */
	int *newAddr;

	--arr->count;

    memcpy(
        &arr->fds[idx],
        &arr->fds[idx + 1],
        (arr->count - idx) * sizeof(int)
    );

	if (arr->count < arr->max / 2)
	{
		arr->max /= 2;
		newAddr = realloc(arr->fds, arr->max);

		if (!newAddr)
		{
			perror("removeClient() failed: could not allocate memory");
			return -1;
		}

		arr->fds = newAddr;
	}

	return 0;
}

int procMsg(sockArr *arr, int idx)
{
	/* counter */
	int i;

	/* Socket that received message to process */
	int clientFd = arr->fds[idx];

	int recvDataSz;
	recvDataSz = recv(clientFd, buffer, MSG_BUFFER_SIZE, 0);

	if (recvDataSz == -1)
	{
		perror("An error occurred while receiving data");
		removeClient(arr, idx);
		return 0;
	}

	/* A message size of 0 means client has disconnected */
	if (!recvDataSz)
	{
		removeClient(arr, idx);
		return 0;
	}

	i = 0;
	while (i < arr->count)
	{
		if (send(arr->fds[i], buffer, recvDataSz, 0) == -1)
		{
			perror("Failed to send message");
		}

		++i;
	}

	return 0;
}

