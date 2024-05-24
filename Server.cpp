#include "Server.hpp"

Server::Server(unsigned short _port, std::string _password)
{
	port = _port;
	password = _password;
}

Server::~Server()
{
	std::cout << "\x1b[1;31mServer Closed.\x1b[0m" << std::endl;
}