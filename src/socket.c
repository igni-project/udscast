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

int create_sock_arr(sock_arr *arr)
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

int add_new_client(sock_arr *arr, int fd)
{
	/* The reallocated address is audited before use */
	int *new_addr;

	if (arr->count >= arr->max)
	{
		arr->max *= 2;
		new_addr = realloc(arr->fds, arr->max);

		if (!new_addr)
		{
			perror("add_new_client() failed: could not allocate memory");
			return -1;
		}

		arr->fds = new_addr;
	}

	arr->fds[arr->count] = fd;

	++arr->count;

	return 0;
}

int remove_client(sock_arr *arr, int idx)
{
	/* The reallocated address is audited before use */
	int *new_addr;

	--arr->count;

    memcpy(
        &arr->fds[idx],
        &arr->fds[idx + 1],
        (arr->count - idx) * sizeof(int)
    );

	if (arr->count < arr->max / 2)
	{
		arr->max /= 2;
		new_addr = realloc(arr->fds, arr->max);

		if (!new_addr)
		{
			perror("remove_client() failed: could not allocate memory");
			return -1;
		}

		arr->fds = new_addr;
	}

	return 0;
}

int proc_msg(sock_arr *arr, int idx, char echoback)
{
	/* counter */
	int i;

	/* Socket that received message to process */
	int client_fd = arr->fds[idx];

	int data_sz;
	data_sz = recv(client_fd, buffer, MSG_BUFFER_SIZE, 0);

	if (data_sz == -1)
	{
		perror("An error occurred while receiving data");
		remove_client(arr, idx);
		return 0;
	}

	/* A message size of 0 means client has disconnected */
	if (!data_sz)
	{
		remove_client(arr, idx);
		return 0;
	}

	i = 0;

	while (i < idx)
	{
		if (send(arr->fds[i], buffer, data_sz, 0) == -1)
		{
			perror("Failed to send message");
		}

		++i;
	}

	if (echoback)
	{
		if (send(arr->fds[i], buffer, data_sz, 0) == -1)
		{
			perror("Failed to send message");
		}
	}

	++i;

	while (i < arr->count)
	{
		if (send(arr->fds[i], buffer, data_sz, 0) == -1)
		{
			perror("Failed to send message");
		}

		++i;
	}

	return 0;
}

