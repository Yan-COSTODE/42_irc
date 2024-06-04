#include "Client.hpp"

Client::Client(int _fd, string _ipAddress)
{
	fd = _fd;
	ipAddress = _ipAddress;
	authentified = false;
	nick = "";
	user = "";
	bot = false;
}

int Client::GetFd() const
{
	return fd;
}

void Client::Broadcast(string _msg)
{
	send(fd, _msg.c_str(), _msg.size(), 0);
}

string Client::GetNick() const
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

void Client::SetBot(bool _status)
{
	bot = _status;
}

bool Client::GetBot() const
{
	return bot;
}

void Client::SetUser(string _user)
{
	user = _user;
	string _msg = "\x1b[1;32mYour username has been set to " + _user + "\n\x1b[0m";
	Broadcast(_msg);
}

void Client::SetNick(string _nick)
{
	nick = _nick;
	string _msg = "\x1b[1;32mYour nickname has been set to " + _nick + "\n\x1b[0m";
	Broadcast(_msg);
}

bool Client::operator==(const Client &_other) const
{
	return nick == _other.nick;
}

ostream& operator<<(ostream& _os, const Client& _client)
{
	string _output = "";

	if (_client.GetBot())
		_output = "Bot <";
	else
		_output = "Client <";

	if (_client.GetNick().empty())
	{
		ostringstream _oss;
		_oss << _client.GetFd();
		_output += _oss.str();
	}
	else
		_output += _client.GetNick();

	_output += ">";
	_os << _output;
	return _os;
}

