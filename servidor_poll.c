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

void	new_client(struct pollfd *canal, int lstfd, int *max)
{
	int					connfd;
	socklen_t			len;
	int					it;
	char				buff[MAX_LINE];
	struct sockaddr_in	cliaddr;

	len = sizeof(cliaddr);
	connfd = accept(lstfd, (struct sockaddr *)&cliaddr, &len);
	printf("Nova conexão - ip: %s |  porta %d\n",
			inet_ntop(AF_INET, (struct sockaddr *)&cliaddr.sin_addr, buff, sizeof(buff)),
			ntohs(cliaddr.sin_port));
	it = 0;
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

void	send_msg(struct pollfd *canal, int max)
{
	int		it;
	int		connfd;
	char	buff[MAX_LINE];

	it = 2;
	connfd = -1;
	while (it <= max)
	{
		if (canal[it].revents & POLLIN)
			connfd = canal[it].fd;
		++it;
	}
	if (connfd != -1)
	{
		printf("use id %d: ", connfd);
		if (fgets(buff, sizeof(buff), stdin))
			write(connfd, buff, strlen(buff));
	}
}

bool	read_msg(int connfd)
{
	int		rdbytes;
	char	buff[MAX_LINE];

	if ((rdbytes = read(connfd, buff, MAX_LINE)) == 0)
	{
		printf("cliente desconectado %d\n", connfd);
		close(connfd);
		return false;
	}
	buff[rdbytes] = '\0';
	printf("user: %d ", connfd);
	write(STDOUT_FILENO, buff, strlen(buff));
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
	canal[1].fd = STDIN_FILENO;
	canal[1].events = POLLIN;
	max = 1;
	while (true)
	{
		poll(canal, max + 1, -1);
		if (canal[0].revents & POLLIN)
			new_client(canal, lstfd, &max);
		if (canal[1].revents & POLLIN)
			send_msg(canal, max);
		it = 2;
		while (it <= max)
		{
			if (canal[it].fd > 0)
			{
				if (canal[it].revents & POLLIN)
					if (!read_msg(canal[it].fd))
						canal[it].fd = -1;
			}
			++it;
		}
		canal[it].revents = 0;
	}	
	return (0);
}
