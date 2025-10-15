#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define	MAX_LINE	4096
#define	LISTENQ		10
#define	MAX_CLIENT	1024
#define GREEN		"\033[1;32m"
#define RED		"\033[1;31m"
#define BLUE		"\033[1;34m"
#define RESET		"\033[0m"

void	new_client(struct pollfd *canal, int lstfd, int *max)
{
	int					connfd;
	socklen_t			len;
	int					it;
	char				buff[MAX_LINE];
	struct sockaddr_in	cliaddr;

	len = sizeof(cliaddr);
	connfd = accept(lstfd, (struct sockaddr *)&cliaddr, &len);
	printf( RED "Nova conexão - ip: %s |  porta %d\n" RESET,
			inet_ntop(AF_INET, (struct sockaddr *)&cliaddr.sin_addr, buff, sizeof(buff)),
			ntohs(cliaddr.sin_port));
	it = 1;
	while (it < MAX_CLIENT)
	{
		if (canal[it].fd < 0)
		{
			canal[it].fd = connfd;
			canal[it].events = POLLIN;
			if (it > (*max))
				(*max) = it;
			return ;
		}
		++it;
	}
}

void	send_msg(int connfd)
{
	char	buff[MAX_LINE];

	fprintf(stdout, GREEN "send user id %d: " RESET, connfd);
	if (fgets(buff, sizeof(buff), stdin))
		write(connfd, buff, strlen(buff));
}

bool	read_msg(int connfd)
{
	int		rdbytes;
	char	buff[MAX_LINE];

	if ((rdbytes = read(connfd, buff, MAX_LINE)) == 0)
	{
		printf(RED "cliente desconectado %d\n" RESET, connfd);
		close(connfd);
		return false;
	}
	buff[rdbytes] = '\0';
	fprintf(stdout, BLUE "receive user id: %d: %s" RESET, connfd, buff);
	send_msg(connfd);
	return (true);
}

int	ft_server(char *port)
{
	int						lstfd;
	struct sockaddr_in		server;

	lstfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi(port));
	bind(lstfd, (struct sockaddr *)&server, sizeof(server));
	listen(lstfd, LISTENQ);
	return (lstfd);
}

int	main(int argc, char *argv[])
{
	int					lstfd;
	int					max;
	int					it;
	struct	pollfd		canal[MAX_CLIENT];
	

	if (argc != 2)
	{
		fprintf(stderr, "%s\n", "error: use: ./servidor_poll [porta]");
		exit(EXIT_FAILURE);
	}
	lstfd = ft_server(argv[1]);
	it = 0;
	while (it < MAX_CLIENT)
	{
		canal[it].fd = -1;
		++it;
	}
	canal[0].fd = lstfd;
	canal[0].events = POLLIN;
	max = 0;
	while (true)
	{
		poll(canal, max + 1, -1);
		if (canal[0].revents & POLLIN)
			new_client(canal, lstfd, &max);
		it = 1;
		while (it <= max)
		{
			if ((canal[it].fd > 0) && (canal[it].revents & POLLIN))
			{
				if (!read_msg(canal[it].fd))
					canal[it].fd = -1;
			}
			++it;
		}
	}	
	return (0);
}
