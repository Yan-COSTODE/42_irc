#include "Client.hpp"

Client::Client(int _fd, int _port, const string &_hostname)
{
	fd = _fd;
	port = _port;
	hostname = _hostname;
}

Client::~Client()
{
}

int Client::GetPort() {
	return port;
}

string Client::GetHostname() {
	return hostname;
}