//============================================================================
// Name        : UDPEchoClient.cpp
// Author      : Bruce
//============================================================================

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>

#define SERVER_PORT 1234

void identify(char *exeName) {

	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "Executable name:  " << exeName << std::endl;
	std::cout << "Source file name: " << __FILE__ << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
}

int send_cmdl_strings(int argc, char* argv[]) {
	int s, rc, i;
	struct sockaddr_in cliAddr, remoteServAddr;
	struct hostent *h;

	// check command line
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " <server> <data1> ... <dataN> " << std::endl;
		return EXIT_FAILURE;
	}

	// get IP address from host name
	h = gethostbyname(argv[1]);
	if (h == NULL) {
		std::cout << argv[0] << ": unknown host '" << argv[1] << "'" << std::endl;
		return EXIT_FAILURE;
	}

	// set port and address
	std::cout << argv[0] << ": send data to '" << h->h_name << "' (IP : "
			<< inet_ntoa(*(struct in_addr *) h->h_addr_list[0]) << ")" << std::endl;
	remoteServAddr.sin_family = h->h_addrtype;
	memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0],
			h->h_length);
	remoteServAddr.sin_port = htons(SERVER_PORT);

	// create socket
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		std::cout << argv[0] << ": cannot open socket (" << strerror(errno) << ")" << std::endl;
		return EXIT_FAILURE;
	}

	// bind any port
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliAddr.sin_port = htons(0);
	rc = bind(s, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
	if (rc < 0) {
		std::cout << argv[0] << ": cannot bind port (" << strerror(errno) << ")" << std::endl;
		return EXIT_FAILURE;
	}

	// send data
	for (i = 2; i < argc; i++) {
		rc = sendto(s, argv[i], strlen(argv[i]) + 1, 0,
				(struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
		if (rc < 0) {
			std::cout << argv[0] << ": cannot send data " << i-1 << std::endl;
			close(s);
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	// output name
	identify(argv[0]);

	// send strings from command line
	int ret = send_cmdl_strings(argc, argv);

	return ret;
}
