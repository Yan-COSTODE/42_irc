#include "Client.hpp"

Client::Client(int _fd, string _ipAddress)
{
	fd = _fd;
	ipAddress = _ipAddress;
	connected = false;
	nick = "";
	user = "";
}

int Client::GetFd()
{
	return fd;
}
