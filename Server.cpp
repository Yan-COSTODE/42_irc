#include "Server.hpp"

Server::Server(unsigned short _port, const std::string& _password) : serverAdrress()
{
	std::cout << "\x1b[1;32mServer Created\x1b[0m" << std::endl;
	port = _port;
	password = _password;
	serverSocket = -1;
	status = true;
	Start();
}

Server::~Server()
{
	std::cout << "\x1b[1;31mServer Closed\x1b[0m" << std::endl;
}

void Server::Start()
{
	//poll
}


int Server::PrintError(const std::string& msg)
{
	std::cerr << "\x1b[1;31m" << msg << "\x1b[0m" << std::endl;
	return (EXIT_FAILURE);
}