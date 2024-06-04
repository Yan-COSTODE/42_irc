#include "Client.hpp"

Client::Client(int _fd, string _ipAddress)
{
	fd = _fd;
	ipAddress = _ipAddress;
	authentified = false;
	nick = "";
	user = "";
}

int Client::GetFd() const
{
	return fd;
}

void Client::Broadcast(string _msg)
{
	send(fd, _msg.c_str(), _msg.size(), 0);
}

string Client::Nickname() const
{
	return nick;
}

bool Client::operator==(const Client &_other) const
{
	return nick == _other.nick;
}

ostream& operator<<(ostream& _os, const Client& _client)
{
	string _output = "Client <";

	if (_client.Nickname().empty())
	{
		ostringstream _oss;
		_oss << _client.GetFd();
		_output += _oss.str();
	}
	else
		_output += _client.Nickname();

	_output += ">";
	_os << _output;
	return _os;
}

