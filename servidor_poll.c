#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE	4096
#define LISTENQ		10
#define MAX_CLIENT	1024
#define	TIMEOUT		-1
#define RED			"\033[1;31m"
#define	GREEN		"\033[1;32m"
#define YELLOW		"\033[1;33m"
#define RESET		"\033[0m"

int	ft_server(char const *port)
{
	int						listenfd;
	struct sockaddr_in		server;

	listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(atoi(port));
	bind(listenfd, (struct sockaddr *)&server, sizeof(server));
	listen(listenfd, LISTENQ);
	return (listenfd);
}

void	ft_new_client(struct pollfd *client, int *max, int listenfd)
{
	struct	sockaddr_in		serv_client;
	char					buff[MAX_LINE];
	int						connfd;
	socklen_t				len;
	int						it;

	connfd = accept(listenfd, (struct sockaddr *)&serv_client, &len);
	printf(YELLOW "Nova conexão: ip: %s, porta %d\n" RESET,
			inet_ntop(AF_INET, &serv_client.sin_addr, buff, sizeof(buff)),
			ntohs(serv_client.sin_port));
	it = 2;
	while (it < MAX_LINE)
	{
		if (client[it].fd < 0)
		{
			client[it].fd = connfd;
			client[it].events = POLLIN;
			if (it > (*max))
				(*max) = it;
			return ;
		}
		++it;
	}
	fprintf(stderr, RED "%s\n" RESET, "limite maximo de cliente atingido");
	close(connfd);
}

void	ft_handle_stdin(struct pollfd *client, int max)
{
	char	buff[MAX_LINE];
	int		it;

	it = 0;
	if (fgets(buff, sizeof(buff), stdin))
	{
		while (it <= max)
		{
			if (client[it].fd >= 0)
				write(client[it].fd, buff, strlen(buff));
			++it;
		}
	}
}

void	ft_handle_client(struct pollfd *client, int it)
{
	char	buff[MAX_LINE];
	int		rbytes;

	rbytes = read(client[it].fd, buff, sizeof(buff));
	if (rbytes == 0)
	{
		printf(RED "Cliente %d desconectado\n" RESET, client[it].fd);
		close(client[it].fd);
		client[it].fd = -1;
		return ;
	}
	buff[rbytes] = '\0';
	write(STDOUT_FILENO, buff, strlen(buff));
}

int	main(int argc, char *argv[])
{
	int						listenfd;
	int						max;
	int						it;
	typedef struct	pollfd	t_pollfd;
	t_pollfd	client[MAX_CLIENT];

	if (argc != 2)
	{
		fprintf(stderr, RED "Uso %s [porta]\n" RESET, argv[0]);
		exit(EXIT_FAILURE);
	}
	listenfd = ft_server(argv[1]);
	max = 0;
	it = 0;
	while (it < MAX_CLIENT)
	{
		client[it].fd = -1;
		++it;
	}
	client[0].fd = listenfd;
	client[0].events = POLLIN;
	client[1].fd = STDIN_FILENO;
	client[1].events = POLLIN;
	max = 1;
	while (true)
	{
		if (poll(client, max + 1, TIMEOUT) < 0)
		{
			perror("error in poll: ");
			exit(EXIT_FAILURE);
		}
		if (client[0].revents & POLLIN)
			ft_new_client(client, &max, listenfd);
		if (client[1].revents & POLLIN)
			ft_handle_stdin(client, max);
		it = 0;
		while (it <= max)
		{
			if (client[it].fd < 0)
				continue ;
			if (client[it].revents & POLLIN)
				ft_handle_client(client, it);
			++it;
		}
	}
	return (0);	
}
