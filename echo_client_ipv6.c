#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE	1024

// from netdb.h
//   struct addrinfo {
//       int     ai_flags;       /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
//       int     ai_family;      /* PF_xxx */
//       int     ai_socktype;    /* SOCK_xxx */
//       int     ai_protocol;    /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
//       socklen_t ai_addrlen;   /* length of ai_addr */
//       char    *ai_canonname;  /* canonical name for hostname */
//       struct  sockaddr *ai_addr;      /* binary address */
//       struct  addrinfo *ai_next;      /* next structure in linked list */
//   };

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
	int rv;
	int s;
	struct addrinfo hints, *results = NULL, *ai = NULL;
	char buf[BUF_SIZE];

	// for IPv6
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = PF_UNSPEC; // PF_UNSPEC, AF_INET, AF_INET6
    hints.ai_socktype = SOCK_STREAM;

    rv = getaddrinfo("localhost", "12345", &hints, &results);
	if (rv) {
		perror("getaddrinfo() failed...");
		freeaddrinfo(results); // <- don't forget to free addrinfo
		return -1;
	}

	// debug print...
	printf("getaddrinfo() results:\n");
	for (ai = results; ai != NULL; ai = ai->ai_next) {
		printf("	");
		print_addrinfo(ai);	
	}

	// connect
	for (ai = results; ai != NULL; ai = ai->ai_next) {
		printf("connect to ");
		print_addrinfo(ai);	

		s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (s < 0) continue;
		rv = connect(s, ai->ai_addr, ai->ai_addrlen);
		if (rv >= 0) {
			printf("connect() : success\n");
			break;
		}
		s = -1;
	}

	if (s < 0) {
		perror("connect() failed...");
		freeaddrinfo(results);
		return -2;
	}

	// send string
	snprintf(buf, BUF_SIZE, "echo_client_ipv6.c\r\n");
	printf("send() : %s\n", buf);
	rv = send(s, buf, strlen(buf), 0);
	if (rv <= 0) {
	        perror("send() failed...");
			freeaddrinfo(results);
	        close(s);
	        return -3;
	}
	
	// recv string
	memset(buf, 0, sizeof(buf));
	rv = recv(s, buf, BUF_SIZE, 0);
	if (rv < 0) {
		perror("recv() failed...");
		freeaddrinfo(results);
		close(s);
		return -4;
	}
	printf("recv: %s(size=%d)\n", buf, rv);

	// close
	freeaddrinfo(results); // <- don't forget to free addrinfo
	close(s);

	return 0;
}
