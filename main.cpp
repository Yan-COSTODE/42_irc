#include <climits>
#include "Server.hpp"

int printerr(const std::string& msg)
{
	std::cerr << "\x1b[1;31m" << msg << "\x1b[0m" << std::endl;
	return (EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return printerr("Error: Wrong Number of Arguments.");

	unsigned short _port = std::atoi(argv[1]);
	if (_port < 1024 || _port > USHRT_MAX)
		return printerr("Error: Wrong Port.");
	Server _server(_port, argv[2]);
}