#include "Command.hpp"

void Command::Parse(string _data, Client* _client, Server* _server)
{
	string _cmd = ExtractCommand(_data);
	string _msg = "";

	if (_cmd == "PASS")
	{
		string data = ExtractArgs(_data);
		if (data == _server->GetPass())
			_client->Authentify();
		else
		{
			_msg = "Password does not match\n";
			_client->Broadcast(_msg);
		}
		return ;
	}
	if (_cmd == "USER")
	{
		if (_cmd.empty())
		{
			_msg = "Username cannot be left blank\n";
			_client->Broadcast(_msg);
		}
		else
			_client->SetUser(_cmd);
		return ;
	}
	if (_cmd == "NICK")
	{
		if (_cmd.empty())
		{
			_msg = "Nickname cannot be left blank\n";
			_client->Broadcast(_msg);
		}
		else if (!_server->IsNameAvailable(_cmd))
		{
			_msg = "This nickname has already been picked\n";
			_client->Broadcast(_msg);
			return ;
		}
		else
			_client->SetNick(_cmd);
		return ;
	}

	else if (_client->GetNick().empty() || _client->GetUser().empty())
	{
		_msg = "\x1b[1;31mYou need to authentify yourself before executing any command\n\x1b[0m";
		_client->Broadcast(_msg);
	}

	cout << "\x1b[1;37mClient <" << _client->GetNick() << "> Data: " << _data << "\x1b[0m" << flush;

	if (_cmd == "WHO")
		Who(ExtractArgs(_data), _client, _server);
	else if (_cmd == "JOIN")
		Join(ExtractArgs(_data), _client, _server);
	else if (_cmd == "PART")
		Part(ExtractArgs(_data), _client, _server);
	else if (_cmd == "INVITE")
		Invite(ExtractArgs(_data), _client, _server);
	else
		NotFound(_cmd, _client);
}

void Command::Invite(string _data, Client *_client, Server *_server)
{
	Channel *channel = _server->GetChannel(&_data[1]);
	if (!channel->CheckAdmin(*_client))
	{
		_client->Broadcast("\x1b[1;31mYou aren't allowed to do that\n\x1b[0m");
		return ;
	}
	if (channel->CheckUser(*_client))
	{
		_client->Broadcast("\x1b[1;31mThis user is already in the channel\n\x1b[0m");
		return ;
	}
	channel->AddInvited(*_client);
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
		return ;
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
		return ;
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