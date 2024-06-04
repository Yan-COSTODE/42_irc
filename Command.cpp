#include "Command.hpp"

void Command::Parse(string _data, Client* _client, Server* _server)
{
	if (!_client)
		return;

	string _cmd = ExtractCommand(_data);
	string _msg = "";

	cout << "\x1b[1;37m" << *_client << " Data: " << _data << "\x1b[0m" << flush;

	if (_cmd == "PASS")
		Pass(ExtractArgs(_data), _client, _server);
	else if (_cmd == "NICK")
		Nick(ExtractArgs(_data), _client, _server);
	else if (_cmd == "USER")
		User(ExtractArgs(_data), _client);
	else if (_client->GetNick().empty() || _client->GetUser().empty())
	{
		_msg = "\x1b[1;31mYou need to authentify yourself before executing any command\n\x1b[0m";
		_client->Broadcast(_msg);
	}
	else if (_cmd == "WHO")
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
	else if (_cmd == "KICK")
		Kick(ExtractArgs(_data), _client, _server);
	else if (_cmd == "MODE")
		Mode(ExtractArgs(_data), _client, _server);
	else if (_cmd == "TOPIC")
		Topic(ExtractArgs(_data), _client, _server);
	else
		NotFound(_cmd, _client);
}

vector<string> Command::SplitString(const string str)
{
	std::istringstream iss(str);
	std::vector<std::string> result;
	std::string word;

	while (getline(iss, word, ' '))
	{
		if (!word.empty())
			result.push_back(word);
	}
	return result;
}

void Command::Mode(string _data, Client *_client, Server *_server)
{
	(void)_data;
	(void)_client;
	(void)_server;
}

void Command::Topic(string _data, Client *_client, Server *_server)
{
	string _msg = "";
	if (_data.size() >= 1)
	{
		Channel *_channel = _server->GetChannel(&ExtractCommand(_data)[1]);
		if (!_channel)
		{
			_msg = "Channel " + ExtractCommand(_data) + " doesn't exist\n";
			_client->Broadcast(_msg);
		}
		else if (_channel->GetTopicAdmin())
		{
			if (_channel->CheckAdmin(*_client))
			{
				if (!ExtractMsg(_data).empty())
					_channel->SetTopic(ExtractMsg(_data));
				else
				{
					_msg = "Topic: " + _channel->GetTopic();
					_client->Broadcast(_msg);
				}
			}
			else
			{
				_msg = "Only the operator can set the topic at the moment\n";
				_client->Broadcast(_msg);
			}
		}
		else if (_channel->CheckUser(*_client))
		{
			if (!ExtractMsg(_data).empty())
				_channel->SetTopic(ExtractMsg(_data));
			else
			{
				_msg = "Topic: " + _channel->GetTopic();
				_client->Broadcast(_msg);
			}
		}
		else
		{
			_msg = "You are not part of " +  ExtractCommand(_data);
			_client->Broadcast(_msg);
		}
	}
	else
	{
		_msg = "Topic command needs 1 or 2 arguments\n";
		_client->Broadcast(_msg);
	}
}


void Command::Pass(string _data, Client *_client, Server *_server)
{
	if (_client->GetAuthentified() && !_client->GetNick().empty() && !_client->GetUser().empty())
		_client->Broadcast("\x1b[1;31mYou are already authentified\n\x1b[0m");
	else if (_client->GetAuthentified() && (_client->GetNick().empty() || _client->GetUser().empty()))
		_client->Broadcast("\x1b[1;31mPassword has been entered\n\x1b[0m");
	else if (_data == _server->GetPass())
	{
		_client->Authentify();
		_client->Broadcast("\x1b[1;32mPassword is correct\n\x1b[0m");
	}
	else
		_client->Broadcast("\x1b[1;31mPassword doesn't match\n\x1b[0m");
}

void Command::Nick(string _data, Client *_client, Server *_server)
{
	if (!_client->GetAuthentified())
		_client->Broadcast("\x1b[1;31mPassword hasn't been entered\n\x1b[0m");
	else if (!_client->GetNick().empty())
		_client->Broadcast("\x1b[1;31mNickname cannot be set multiple times\n\x1b[0m");
	else if (_data.empty())
		_client->Broadcast("\x1b[1;31mNickname cannot be left blank\n\x1b[0m");
	else if (!_server->IsNameAvailable(_data))
		_client->Broadcast("\x1b[1;31mThis nickname has already been picked\n\x1b[0m");
	else
	{
		_client->SetNick(_data);

		if (!_client->GetNick().empty() && !_client->GetUser().empty())
		{
			_client->Broadcast("\x1b[1;32mYou are now authentified\n\x1b[0m");
			cout << "\x1b[1;32m" << *_client << " authentified\x1b[0m" << endl;
		}
	}
}

void Command::User(string _data, Client *_client)
{
	if (!_client->GetAuthentified())
		_client->Broadcast("\x1b[1;31mPassword hasn't been entered\n\x1b[0m");
	else if (!_client->GetUser().empty())
		_client->Broadcast("\x1b[1;31mUsername cannot be set multiple times\n\x1b[0m");
	else if (_data.empty())
		_client->Broadcast("\x1b[1;31mUsername cannot be left blank\n\x1b[0m");
	else
	{
		_client->SetUser(_data);

		if (!_client->GetNick().empty() && !_client->GetUser().empty())
		{
			_client->Broadcast("\x1b[1;32mYou are now authentified\n\x1b[0m");
			cout << "\x1b[1;32m" << *_client << " authentified\x1b[0m" << endl;
		}
	}
}

void Command::Invite(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("\x1b[1;31mINVITE needs a channel in argument\n\x1b[0m");
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

void Command::Kick(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("\x1b[1;31mKICK needs a channel in argument\n\x1b[0m");
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

		_channel->Kick(*_client, *_dest, ExtractMsg(_data));
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
		_client->Broadcast("\x1b[1;31mJOIN needs a channel in argument\n\x1b[0m");
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
		_client->Broadcast("\x1b[1;31mPART needs a channel in argument\n\x1b[0m");
		return;
	}

	Channel *_channel = _server->GetChannel(&_data[1]);

	if (_channel)
	{
		_channel->QuitChannel(*_client, true);
		if (_channel->Users() == 0)
		{
			_server->AddToRemoveChannel(*_channel);
			_server->RemoveChannel();
		}
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
		_client->Broadcast("\x1b[1;31mQUIT don't needs any argument\n\x1b[0m");
		return;
	}

	_server->RemoveClient(*_client, true);
}

void Command::Msg(std::string _data, Client *_client, Server *_server)
{
	if (_data.size() < 0)
	{
		_client->Broadcast("\x1b[1;31mPRIVMSG needs a destination in argument\n\x1b[0m");
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
			cout << "\x1b[1;32m" << *_client << " Send Message To " << *_dest << "\x1b[0m" << endl;
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
		_client->Broadcast("\x1b[1;31mWHO needs a channel in argument\n\x1b[0m");
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