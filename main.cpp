#include <climits>
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		Server::ThrowError("Error: Wrong Number of Arguments");

	unsigned short _port = std::atoi(argv[1]);

	if (_port < 1024 || _port > USHRT_MAX)
		Server::ThrowError("Error: Wrong Port");

	Server _server(_port, argv[2]);
}