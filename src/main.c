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
	int srv_fd;

	/* Variables for accept() */
	struct sockaddr_un tmp_addr = {0};
	socklen_t tmp_addr_sz = 0;
	int new_fd;

	/* Set of all sockets */
	fd_set read_fds;
	int max_fd;

	/* Client sockets */
	sock_arr clients;
	int client_fd;

	/* Counter */
	int i;

	/* Socket activity */
	int activity;

	/* Parse command line arguments */
	cmdline_parser(argc, argv, &args_info);

	srv_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (srv_fd == -1)
	{
		perror("Failed to create server socket");
		return -1;
	}

	sv_addr.sun_family = AF_UNIX;
	strncpy(sv_addr.sun_path, args_info.domain_arg, sizeof(sv_addr.sun_path));
	if (bind(srv_fd, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) == -1)
	{
		perror("Failed to bind server socket");
		return -1;
	}

	if (listen(srv_fd, 10) == -1)
	{
		perror("Failed to open server socket for listening");
		return -1;
	}

	if (create_sock_arr(&clients) == -1)
	{
		printf("Failed to create socket array.\n");
		return -1;
	}

	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(srv_fd, &read_fds);
		max_fd = srv_fd;

		i = 0;
		while (i < clients.count)
		{
			client_fd = clients.fds[i];

			if (client_fd > 0) FD_SET(client_fd, &read_fds);

			if (client_fd > max_fd) max_fd = client_fd;

			++i;
		}

		/* Wait until a socket is active */
		activity = select(max_fd + 1, &read_fds, 0, 0, 0);

		if (activity == -1 && errno != EINTR)
		{
			perror("Socket select failed");
		}

		/* Activity on the server socket means a new connection */
		if (FD_ISSET(srv_fd, &read_fds))
		{
			new_fd = accept(srv_fd, (struct sockaddr*)&tmp_addr, &tmp_addr_sz);
			if (new_fd == -1)
			{
				perror("Failed to accept client");
			}
			else
			{
				add_new_client(&clients, new_fd);
			}
		}

		i = clients.count - 1;
		while (i != -1)
		{
			client_fd = clients.fds[i];
			
			if (FD_ISSET(client_fd, &read_fds))
			{
				proc_msg(&clients, i);
			}

			--i;
		}
	}

	/* Closing the server socket out of courtesy */
	close(srv_fd);

	return 0;
}

