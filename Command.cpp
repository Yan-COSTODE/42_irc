#include "Command.hpp"

void Command::Parse(string _data, Client* _client, Server* _server)
{
	if (!_client)
		return;

	cout << "\x1b[1;37m" << *_client << " Data: " << _data << "\x1b[0m" << flush;

	string _cmd = ExtractCommand(_data);

	if (_cmd == "WHO")
		Who(ExtractArgs(_data), _client, _server);
	else if (_cmd == "JOIN")
		Join(ExtractArgs(_data), _client, _server);
	else if (_cmd == "PART")
		Part(ExtractArgs(_data), _client, _server);
	else if (_cmd == "QUIT")
		Quit(ExtractArgs(_data), _client, _server);
	else if (_cmd == "PRIVMSG")
		Msg(ExtractArgs(_data), _client, _server);
	else if (_cmd == "INVITE")
		Invite(ExtractArgs(_data), _client, _server);
	else
		NotFound(_cmd, _client);
}

void Command::Invite(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("\x1b[1;31mINVITE need a channel in argument\n\x1b[0m");
		return;
	}

	Channel *_channel = _server->GetChannel(&ExtractCommand(_data)[1]);

	if (_channel)
	{
		_data += "\n";
		Client* _dest = _server->GetClient(ExtractArgs(_data));

		if (!_dest)
		{
			string _msg = "\x1b[1;31m" + ExtractArgs(_data) + " isn't a valid destination\n\x1b[0m";
			_client->Broadcast(_msg);
			return;
		}

		_channel->Invite(*_client, *_dest);
	}
	else
	{
		string _msg = "\x1b[1;31m" + ExtractCommand(_data) + " isn't a valid channel\n\x1b[0m";
		_client->Broadcast(_msg);
	}
}


void Command::NotFound(string _data, Client *_client)
{
	string _msg = "\x1b[1;31m" + _data + " isn't a valid command\n\x1b[0m";
	_client->Broadcast(_msg);
}

void Command::Join(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("\x1b[1;31mJOIN need a channel in argument\n\x1b[0m");
		return;
	}

	Channel *_channel = _server->AddChannel(&_data[1]);

	if (_channel)
		_channel->JoinChannel(*_client);
	else
	{
		string _msg = "\x1b[1;31m" + _data + " isn't a valid channel\n\x1b[0m";
		_client->Broadcast(_msg);
	}
}

void Command::Part(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("\x1b[1;31mPART need a channel in argument\n\x1b[0m");
		return;
	}

	Channel *_channel = _server->GetChannel(&_data[1]);

	if (_channel)
	{
		_channel->QuitChannel(*_client);
		if (_channel->Users() == 0)
			_server->RemoveChannel(_channel->Name());
	}
	else
	{
		string _msg = "\x1b[1;31m" + _data + " isn't a valid channel\n\x1b[0m";
		_client->Broadcast(_msg);
	}
}

void Command::Quit(string _data, Client *_client, Server *_server)
{
	if (_data.size() > 0)
	{
		_client->Broadcast("\x1b[1;31mQUIT don't need any argument\n\x1b[0m");
		return;
	}

	_server->RemoveClient(*_client);
}

void Command::Msg(std::string _data, Client *_client, Server *_server)
{
	if (_data.size() < 0)
	{
		_client->Broadcast("\x1b[1;31mPRIVMSG need a destination in argument\n\x1b[0m");
		return;
	}

	string _destName = ExtractCommand(_data);

	if (_data.size() <= 1 || _data[0] != '#')
	{
		Client* _dest = _server->GetClient(_destName);

		if (!_dest)
		{
			string _msg = "\x1b[1;31m" + _destName + " isn't a valid destination\n\x1b[0m";
			_client->Broadcast(_msg);
			return;
		}
		else
		{
			string _msg = _client->GetNick() + ": " + ExtractMsg(_data);
			_dest->Broadcast(_msg);
			_msg = "\x1b[1;32mYour message has been send to " + _destName + "\n\x1b[0m";
			_client->Broadcast(_msg);
			cout << "\x1b[1;32m" << *_client << " Send Message To" << *_dest << "\n\x1b[0m" << endl;
		}
	}
	else
	{
		Channel* _dest = _server->GetChannel(&_destName[1]);

		if (!_dest || !_dest->IsIn(*_client))
		{
			string _msg = "\x1b[1;31m" + _destName + " isn't a valid destination\n\x1b[0m";
			_client->Broadcast(_msg);
			return;
		}
		else
		{
			string _msg = _client->GetNick() + ": " + ExtractMsg(_data);
			_dest->Broadcast(_msg);
			_msg = "\x1b[1;32mYour message has been send to " + _destName + "\n\x1b[0m";
			_client->Broadcast(_msg);
			cout << "\x1b[1;32m" << *_client << " Send Message To" << *_dest << "\n\x1b[0m" << endl;
		}
	}
}

void Command::Who(string _data, Client* _client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("\x1b[1;31mWHO need a channel in argument\n\x1b[0m");
		return;
	}

	Channel *_channel = _server->GetChannel(&_data[1]);

	if (_channel)
		_client->Broadcast(_channel->Who());
	else
	{
		string _msg = "\x1b[1;31m" + _data + " isn't a valid channel\n\x1b[0m";
		_client->Broadcast(_msg);
	}
}

string Command::ExtractCommand(string _data)
{
	size_t _pos = _data.find_first_of(" \t\n");

	if (_pos == string::npos)
		return _data;

	return _data.substr(0, _pos);
}

string Command::ExtractArgs(string _data)
{
	size_t _firstNoSpace = _data.find_first_not_of(" \t\n");

	if (_firstNoSpace == string::npos)
		return "";

	size_t _endFirst = _data.find_first_of(" \t\n", _firstNoSpace);

	if (_endFirst == string::npos)
		return "";

	size_t _firstNoSpaceEnd = _data.find_first_not_of(" \t\n", _endFirst);

	if (_firstNoSpaceEnd == string::npos)
		return "";

	return _data.substr(_firstNoSpaceEnd, _data.size() - _firstNoSpaceEnd - 1);
}

string Command::ExtractMsg(string _data)
{
	size_t _firstNoSpace = _data.find_first_not_of(" \t\n");

	if (_firstNoSpace == string::npos)
		return "";

	size_t _endFirst = _data.find_first_of(" \t\n", _firstNoSpace);

	if (_endFirst == string::npos)
		return "";

	size_t _firstNoSpaceEnd = _data.find_first_not_of(" \t\n", _endFirst);

	if (_firstNoSpaceEnd == string::npos)
		return "";

	return _data.substr(_firstNoSpaceEnd, _data.size() - _firstNoSpaceEnd) + "\n";
}