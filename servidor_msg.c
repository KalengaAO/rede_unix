#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	LISTENQ		10
#define	MAX_LINE	4096
#define MAX_FD		1024

void	ft_chat(int connfd)
{
	int		br;
	char	buff_rd[MAX_LINE];
	char	buff_wr[MAX_LINE];
	
	write(connfd, "\033[32mEste é um canal privado sem sha256\033[0m\n", 46);
	while ((br = read(connfd, buff_rd, MAX_LINE)))
	{
		write(STDOUT_FILENO, buff_rd, br);
		if (fgets(buff_wr, sizeof(buff_wr), stdin) != NULL)
			write(connfd, buff_wr, strlen(buff_wr));
	}
}

int	main(int argc, char *argv[])
{
	if (argc != 2)
	{
		write(STDERR_FILENO, "\033[31mmodo de uso do >_Daemon: ./servidor [porta]\033[0m\n", 54);
		exit(EXIT_FAILURE);
	}
	(void)argv;
	int					listfd;
	int					connfd;
	char				buff[MAX_LINE];
	struct sockaddr_in	servaddr;
	struct sockaddr_in	clieaddr;
	pid_t				child;
	socklen_t			lenc;

	listfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));
	bind(listfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listfd, LISTENQ);
	while (1)
	{
		lenc = sizeof(clieaddr);
		connfd = accept(listfd, (struct sockaddr *)&clieaddr, &lenc);
		if ((child = fork()) == 0)
		{
			close(listfd);
			write(STDOUT_FILENO, "\033[33mEste é um canal privado sem sha256\033[0m\n", 46);
			write(STDOUT_FILENO, "\033[33mLimite de caracter por linha [ 4096 ]\033[0m\n", 48);
			printf("\033[33mconectado em ip: %s, porta: %d\033[0m\n", 
			inet_ntop(AF_INET, &clieaddr.sin_addr, buff, sizeof(buff)),
			ntohs(clieaddr.sin_port));
			ft_chat(connfd);
			close(connfd);
			exit(EXIT_SUCCESS);
		}
		wait(NULL);
	}
	return (0);
}
