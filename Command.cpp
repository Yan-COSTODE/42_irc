#include "Command.hpp"

void Command::Parse(std::string _data, Client* _client, Server* _server)
{
	(void)_server;
	cout << "\x1b[1;37mClient <" << _client << "> Data: " << _data << "\x1b[0m" << flush;
}