#include "main.h"
#include "cmd/cmdline.h"
#include "socket.h"

#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>

int main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	
	/* Server socket */
	struct sockaddr_un sv_addr;
	int srvFd;

	/* Variables for accept() */
	struct sockaddr_un tmpAddr = {0};
	socklen_t tmpAddrSz = 0;
	int newFd;

	/* Set of all sockets */
	fd_set readFds;
	int maxFd;

	/* Client sockets */
	sockArr clients;
	int clientFd;

	/* Counter */
	int i;

	/* Socket activity */
	int activity;

	/* Parse command line arguments */
	cmdline_parser(argc, argv, &args_info);

	srvFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (srvFd == -1)
	{
		perror("Failed to create server socket");
		return -1;
	}

	sv_addr.sun_family = AF_UNIX;
	strncpy(sv_addr.sun_path, args_info.domain_arg, sizeof(sv_addr.sun_path));
	if (bind(srvFd, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) == -1)
	{
		perror("Failed to bind server socket");
		return -1;
	}

	if (listen(srvFd, 10) == -1)
	{
		perror("Failed to open server socket for listening");
		return -1;
	}

	if (createSockArr(&clients) == -1)
	{
		printf("Failed to create socket array.\n");
		return -1;
	}

	while (1)
	{
		FD_ZERO(&readFds);
		FD_SET(srvFd, &readFds);

		i = 0;
		while (i < clients.count)
		{
			clientFd = clients.fds[i];

			if (clientFd > 0) FD_SET(clientFd, &readFds);

			if (clientFd > maxFd) maxFd = clientFd;

			++i;
		}

		/* Wait until a socket is active */
		activity = select(maxFd + 1, &readFds, 0, 0, 0);

		if (activity == -1 && errno != EINTR)
		{
			perror("Socket select failed");
		}

		/* Activity on the server socket means a new connection */
		if (FD_ISSET(srvFd, &readFds))
		{
			newFd = accept(srvFd, (struct sockaddr*)&tmpAddr, &tmpAddrSz);
			if (newFd == -1)
			{
				perror("Failed to accept client");
			}
			else
			{
				addNewClient(&clients, newFd);
			}
		}

		i = clients.count - 1;
		while (i != -1)
		{
			clientFd = clients.fds[i];
			
			if (FD_ISSET(clientFd, &readFds))
			{
				procMsg(&clients, i);
			}

			--i;
		}
	}

	/* Closing the server socket out of courtesy */
	close(srvFd);

	return 0;
}

