#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void print_addrinfo(struct addrinfo *ai)
{
	char str[INET6_ADDRSTRLEN];

	switch(ai->ai_family) {
		case AF_INET:
			inet_ntop(ai->ai_family,
					&((struct sockaddr_in *)ai->ai_addr)->sin_addr,
					str,
					INET_ADDRSTRLEN);
			printf("AF_INET(IPv4): %s\n", str);
			break;
		case AF_INET6:
			inet_ntop(ai->ai_family,
					&((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr,
					str,
					INET6_ADDRSTRLEN);
			printf("AF_INET6(IPv6): %s\n", str);
			break;
	}
}

int main(int argc, char *argv[])
{
	int ss, cs;
	int rv;
	int flag;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *ai  = NULL;
	struct addrinfo *ai_inet  = NULL;
	struct addrinfo *ai_inet6  = NULL;
	struct sockaddr_storage sa;
	socklen_t sa_len;

	char buf[256];
	int  buf_size = 256;
	int  read_size;

	memset((void*)(&hints), 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	rv = getaddrinfo(NULL, "12345", &hints, &res);
	if (rv == EAI_FAMILY) {
		// fallback ipv4
		hints.ai_family   = AF_INET;
		rv = getaddrinfo(NULL, "12345", &hints, &res);
	}
	else if (rv == EAI_SYSTEM || rv != 0) {
		fprintf(stderr, "getaddrinfo() failed...%s\n", gai_strerror(rv));
		return -1;
	}

	for (ai = res; ai != NULL; ai = ai->ai_next) {
		if (ai->ai_family == AF_INET ) ai_inet  = ai;
		if (ai->ai_family == AF_INET6) ai_inet6 = ai;
	}

	if (ai_inet == NULL && ai_inet6 == NULL) {
		fprintf(stderr, "getaddrinfo() return null results...\n");
		return -1;
	}

	// socket (try ipv6 before ipv4)
	ss = -1;
	if (ai_inet6 != NULL) {
		ss = socket(ai_inet6->ai_family, ai_inet6->ai_socktype, ai_inet6->ai_protocol);
		ai = ai_inet6;
		print_addrinfo(ai);
	}
	if (ai_inet != NULL && ss < -1) {
		ss = socket(ai_inet->ai_family, ai_inet->ai_socktype, ai_inet->ai_protocol);
		ai = ai_inet;
		print_addrinfo(ai);
	}
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

	
