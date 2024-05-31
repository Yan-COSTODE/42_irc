#include "Command.hpp"

void Command::Parse(std::string _data, Client* _client, Server* _server)
{
	if (!_client)
		return;

	(void)_server;
	cout << "\x1b[1;37mClient <" << _client->Nickname() << "> Data: " << _data << "\x1b[0m" << flush;
}