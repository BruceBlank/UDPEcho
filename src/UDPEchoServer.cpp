//============================================================================
// Name        : UDPEchoServer.cpp
// Author      : Bruce
//============================================================================

#include <cstdlib>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#define LOCAL_SERVER_PORT 1234
#define BUF 255

void identify(char *exeName)
{
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "Executable name:  " << exeName << std::endl;
	std::cout << "Source file name: " << __FILE__ << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
}

int echo_packages(int argc, char* argv[])
{
	int s, rc, n;
	unsigned int len;
	struct sockaddr_in cliAddr, servAddr;
	char puffer[BUF];
	time_t time1;
	char loctime[BUF];
	char *ptr;
	const int y = 1;

	// create socket
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		std::cout << argv[0] << ": cannot open socket ... ("
				<< strerror(errno) << ")" << std::endl;
		return EXIT_FAILURE;
	}

	// bind correct server port
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servAddr.sin_port = htons (LOCAL_SERVER_PORT);
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	rc = bind ( s, (struct sockaddr *) &servAddr, sizeof (servAddr));
	if (rc < 0) {
		std::cout << argv[0] << ": cannot bind port nummer " << LOCAL_SERVER_PORT << " ("
				<< strerror(errno) << ")" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << argv[0] << ": waiting for data on port (UDP) " << LOCAL_SERVER_PORT << std::endl;

	// Server loop
	while (1) {
		// initialize buffer
		memset (puffer, 0, BUF);
		// receive messages
		len = sizeof (cliAddr);
		n = recvfrom ( s, puffer, BUF, 0, (struct sockaddr *) &cliAddr, &len );
		if (n < 0) {
			std::cout << argv[0]  << ": cannot receive any data ..." << std::cout;
			continue;
		}
		// prepare time values
		time(&time1);
		strncpy(loctime, ctime(&time1), BUF);
		ptr = strchr(loctime, '\n' );
		*ptr = '\0';
		// output received messages
		std::cout << loctime << ": received data from " << inet_ntoa (cliAddr.sin_addr) << ":UDP"
				<< ntohs (cliAddr.sin_port) << " : " << puffer << std::endl;

	}
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	// output name
	identify(argv[0]);

	// send strings from command line
	int ret = echo_packages(argc, argv);

	return ret;
}
