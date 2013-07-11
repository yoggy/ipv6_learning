#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
	int ss, cs;
	int rv;
	int flag;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *ai  = NULL;
	struct sockaddr_storage sa;
	socklen_t sa_len;

	char buf[256];
	int  buf_size = 256;
	int  read_size;

	memset((void*)(&hints), 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_INET6;  // can accept IPv4 address
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	rv = getaddrinfo(NULL, "12345", &hints, &res);
	if (rv != 0) {
		fprintf(stderr, "getaddrinfo() failed...%s\n", gai_strerror(rv));
		return -1;
	}

	ai = res;
	ss = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (ss < 0) {
		fprintf(stderr, "socket() failed...\n");
		return -1;
	}

	flag = 1;
	rv = setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
	if (rv < 0) {
		fprintf(stderr, "setsockopt() failed...\n");
		return -1;
	}
	
	rv = bind(ss, ai->ai_addr, ai->ai_addrlen);
	if (rv < 0) {
		fprintf(stderr, "bind() failed...\n");
		return -1;
	}

	rv = listen(ss, 5);
	if (rv < 0) {
		fprintf(stderr, "listen() failed...\n");
		return -1;
	}

	freeaddrinfo(res);

	sa_len = sizeof(struct sockaddr_storage);
	while(1) {
		cs = accept(ss, (struct sockaddr*) &sa, &sa_len);

		if (cs > 0) {
			fprintf(stdout, "accept\n");
			if (fork() == 0) {
				close(ss);

				read_size = read(cs, buf, buf_size);
				write(cs, buf, read_size);

				close(cs);
			}
			else {
				close(cs);
			}
		}
	}

	return 0;
}

	
