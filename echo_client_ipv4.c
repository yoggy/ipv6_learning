#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE	1024

int main(int argc, char *argv[])
{
	int rv;
	int s;
	struct sockaddr_in addr;
	char buf[BUF_SIZE];

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket() failed...");
		return -1;
	}

	// for IPv4
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port        = htons(12345);

	rv = connect(s, (struct sockaddr*)&addr, sizeof(addr));
	if (rv < 0) {
		perror("connect() failed...");
		return -2;
	}

	// send string
	snprintf(buf, BUF_SIZE, "echo_client_ipv4.c\r\n");
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

	shutdown(s, 2);
	close(s);

	return 0;
}
