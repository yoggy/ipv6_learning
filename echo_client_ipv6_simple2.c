#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
	int s;
	struct sockaddr_in6 addr;
	char buf[BUF_SIZE];

	printf("sizeof(sockaddr_in6) = %d\n", sizeof(addr)); // 28
	printf("sin6_family offset = %d\n", offsetof(struct sockaddr_in6, sin6_family)); // 1
	printf("sin6_port   offset = %d\n", offsetof(struct sockaddr_in6, sin6_port));   // 2
	printf("sin6_addr   offset = %d\n", offsetof(struct sockaddr_in6, sin6_addr));   // 8
	printf("AF_INET6 = %d\n", AF_INET6);       // 30
	printf("SOCK_STREAM = %d\n", SOCK_STREAM); // 1

	// for IPv6
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family     = 30;
	addr.sin6_port       = htons(12345);
	addr.sin6_addr.s6_addr[15] = 1; // ::1

	s = socket(30, 1, 0);
	connect(s, (struct sockaddr*)&addr, sizeof(addr));

	snprintf(buf, BUF_SIZE, "this is test string...\r\n");

	printf("send() : %s\n", buf);
	send(s, buf, strlen(buf), 0);
	
	// recv string
	memset(buf, 0, sizeof(buf));
	recv(s, buf, BUF_SIZE, 0);
	printf("recv: %s\n", buf);

	return 0;
}
