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

string Client::GetNick()
{
	return nick;
}

string Client::GetUser()
{
	return user;
}

bool Client::GetAuthentified() const
{
	return authentified;
}

void Client::Authentify()
{
	authentified = true;
}

void Client::SetUser(string _user)
{
	user = _user;
}

void Client::SetNick(string _nick)
{
	nick = _nick;
}



bool Client::operator==(const Client &_other) const
{
	return nick == _other.nick;
}
