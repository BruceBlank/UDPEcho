//============================================================================
// Name        : UDPEchoServer.cpp
// Author      : Bruce
//============================================================================

#include <cstdlib>
#include <iostream>

void identify(char *exeName)
{
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "Executable name:  " << exeName << std::endl;
	std::cout << "Source file name: " << __FILE__ << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
}

int main(int argc, char* argv[]) {
	// output name
	identify(argv[0]);

	return EXIT_SUCCESS;
}
