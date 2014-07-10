//============================================================================
// Name        : UDPEchoServer.cpp
// Author      : Bruce
//============================================================================

#include <cstdlib>
#include <iostream>
#include <string>

#include <netdb.h>
#include <string.h>
#include <errno.h>

void identify(char *exeName)
{
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "Executable name:  " << exeName << std::endl;
	std::cout << "Source file name: " << __FILE__ << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
}

class UDPServer {
private:
	static const short unsigned int BufferSize = 255;
	uint16_t m_port;
	char m_buffer[BufferSize];
	int m_socket;
	bool m_alreadyBound;

	// clear the data buffer
	void clearBuffer(){
		memset(m_buffer, 0, BufferSize);
	}
public:
	// a class for error-exceptions
	class Error {
	private:
		std::string m_errstr;
		int m_errno;
	public:
		Error(std::string errstr, int e) : m_errstr(errstr), m_errno(e) {}
		void print(){
			std::cout << m_errstr << " (" << m_errno << ")" << std::endl;
		}
	};

	// specify a port to bind in constructor
	UDPServer(uint16_t port = 1234) : m_port(port), m_socket(-1), m_alreadyBound(false){}
	// no copy constructor
	UDPServer(const UDPServer &) = delete;
	// no assignment
	UDPServer &operator=(const UDPServer &) = delete;

	// bind server to port for any address
	void bind();
	// receive data and provided it in internal buffer
	const char *receive();
};

void UDPServer::bind()
{
	struct sockaddr_in serverAddress;
	const int y = 1;

	if(m_alreadyBound){
		return;
	}

	// create socket
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_socket < 0){
		throw Error(std::string("ERROR: cannot open socket"), errno);
	}

	// bind correct server port
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl (INADDR_ANY);
	serverAddress.sin_port = htons(m_port);
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	int rc = ::bind(m_socket, (struct sockaddr *) &serverAddress, sizeof (serverAddress));
	if(rc != 0){
		throw Error(std::string("ERROR: cannot bind port number"), errno);
	}

	// no error => server is bound
	m_alreadyBound = true;
}

const char *UDPServer::receive()
{
	// throw exception, if server not yet bound
	if(!m_alreadyBound){
		throw Error(std::string("ERROR: server not yet bound"), errno);
	}

	struct sockaddr_in clientAddress;

	// clear buffer and receive messages
	clearBuffer();
	unsigned int len = sizeof (clientAddress);
	int n = recvfrom(m_socket, m_buffer, BufferSize, 0, (struct sockaddr *) &clientAddress, &len );
	if (n < 0) {
		// received dirty data
		return nullptr;
	}
	return m_buffer;
}

int main(int argc, char* argv[]) {
	// output name
	identify(argv[0]);

	// parse command line
	unsigned short int port = 5908;
	if(argc > 1){
		port = atoi(argv[1]);
	}

	UDPServer server(port);
	try{
		server.bind();
		while(1){
			const char *buffer = server.receive();
			if(buffer){
				std::cout << "Received data :|" << buffer << "|" << std::endl;
			}else{
				std::cout << "Received dirty data" << std::endl;
			}
		}
	}catch(UDPServer::Error &err){
		err.print();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
