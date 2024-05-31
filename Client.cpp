#include "Client.hpp"

Client::Client(int _fd, string _ipAddress)
{
	fd = _fd;
	ipAddress = _ipAddress;
	authentified = false;
	nick = "";
	user = "";
}

int Client::GetFd()
{
	return fd;
}

void Client::Broadcast(string _msg)
{
	send(fd, _msg.c_str(), _msg.size(), 0);
}

string Client::Nickname()
{
	return nick;
}

bool Client::operator==(const Client &_other) const
{
	return nick == _other.nick;
}
