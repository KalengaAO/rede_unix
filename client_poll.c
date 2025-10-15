#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE	4096
#define POLLFD		2

int	main(int argc, char *argv[])
{
	int		lstfd;

	lstfd = ft_server(argv[1]);


	return (0);
}
