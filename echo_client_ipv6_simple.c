#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE	1024

// struct in6_addr {
//         union {
//                 __u8            u6_addr8[16];
//                 __be16          u6_addr16[8];
//                 __be32          u6_addr32[4];
//         } in6_u;
// #define s6_addr                 in6_u.u6_addr8
// #define s6_addr16               in6_u.u6_addr16
// #define s6_addr32               in6_u.u6_addr32
// };
// 
// struct sockaddr_in6 {
//         unsigned short int      sin6_family;    /* AF_INET6 */
//         __be16                  sin6_port;      /* Transport layer port # */
//         __be32                  sin6_flowinfo;  /* IPv6 flow information */
//         struct in6_addr         sin6_addr;      /* IPv6 address */
//         __u32                   sin6_scope_id;  /* scope id (new in RFC2553) */
// };

int main(int argc, char *argv[])
{
	int rv;
	int s;
	struct sockaddr_in6 addr;
	char buf[BUF_SIZE];

	// for IPv6
	memset(&addr, 0, sizeof(addr));
	addr.sin6_len        = sizeof(addr);
	addr.sin6_family     = AF_INET6;
	addr.sin6_port       = htons(12345);
	memset(addr.sin6_addr.s6_addr, 0, sizeof(struct in6_addr));
	addr.sin6_addr.s6_addr[15] = 1; // ::1

	s = socket(AF_INET6, SOCK_STREAM, 0);
	rv = connect(s, (struct sockaddr*)&addr, sizeof(addr));
	if (rv < 0) {
		perror("connect failed...");
		return -1;
	}

	// send string
	snprintf(buf, BUF_SIZE, "echo_client_ipv6.c\r\n");
	printf("send() : %s\n", buf);
	rv = send(s, buf, strlen(buf), 0);
	if (rv <= 0) {
	        perror("send() failed...");
	        close(s);
	        return -3;
	}
	
	// recv string
	memset(buf, 0, sizeof(buf));
	rv = recv(s, buf, BUF_SIZE, 0);
	if (rv < 0) {
		perror("recv() failed...");
		close(s);
		return -4;
	}
	printf("recv: %s(size=%d)\n", buf, rv);

	// close
	close(s);

	return 0;
}
