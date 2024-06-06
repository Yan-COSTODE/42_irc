#include "Command.hpp"

void Command::Parse(string _data, Client* _client, Server* _server)
{
	if (!_client)
		return;

	string _cmd = ExtractCommand(_data);
	string _msg = "";

	cout << "\x1b[1;37m" << *_client << " Data: " << _data << "\x1b[0m" << flush;

	if (_cmd == "CAP")
		return;
	else if (_cmd == "PASS")
		Pass(ExtractArgs(_data), _client, _server);
	else if (_cmd == "NICK")
		Nick(ExtractArgs(_data), _client, _server);
	else if (_cmd == "USER")
		User(ExtractArgs(_data), _client, _server);
	else if (_client->GetNick().empty() || _client->GetUser().empty())
		return;
	else if (_cmd == "PRIVMSG")
		Msg(ExtractArgs(_data), _client, _server);
	else if (_client->GetBot())
		Bot(_data, _client);
	else if (_cmd == "NAMES")
		Names(ExtractArgs(_data), _client, _server);
	else if (_cmd == "LIST")
		List(ExtractArgs(_data), _client, _server);
	else if (_cmd == "METEO")
		Meteo(ExtractArgs(_data), _client, _server);
	else if (_cmd == "JOIN")
		Join(ExtractArgs(_data), _client, _server);
	else if (_cmd == "PING")
		Ping(ExtractArgs(_data), _client, _server);
	else if (_cmd == "PART")
		Part(ExtractArgs(_data), _client, _server);
	else if (_cmd == "QUIT")
		Quit(ExtractArgs(_data), _client, _server);
	else if (_cmd == "INVITE")
		Invite(ExtractArgs(_data), _client, _server);
	else if (_cmd == "KICK")
		Kick(ExtractArgs(_data), _client, _server);
	else if (_cmd == "MODE")
		Mode(ExtractArgs(_data), _client, _server);
	else if (_cmd == "TOPIC")
		Topic(ExtractArgs(_data), _client, _server);
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
	if (_data.size() <= 1 || _data[0] != '#')
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "TOPIC");
		_client->Broadcast(_msg);
		return;
	}

	string _channelName = ExtractCommand(_data);
	Channel *_channel = _server->GetChannel(&_channelName[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckUser(*_client))
	{
		string _msg = ERR_NOTONCHANNEL(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (ExtractMsg(_data).empty())
	{
		_channel->BroadcastTopic(*_client);
		return;
	}

	if (_channel->GetTopicAdmin() && !_channel->CheckAdmin(*_client))
	{
		string _msg = ERR_CHANOPRIVSNEEDED(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	_channel->SetTopic(_client->GetNick(), ExtractArgs(_data));
}


void Command::Pass(string _data, Client *_client, Server *_server)
{
	if (_client->GetAuthentified())
	{
		string _msg = ERR_ALREADYREGISTERED(_client->GetNick());
		_client->Broadcast(_msg);
		return;
	}

	if (_data.empty())
	{
		string _msg = ERR_NONICKNAMEGIVEN(_client->GetNick());
		_client->Broadcast(_msg);
		return;
	}

	if (_data != _server->GetPass())
	{
		string _msg = ERR_PASSWDMISMATCH(_client->GetNick());
		_client->Broadcast(_msg);
		return;
	}

	_client->Authentify();
}

void Command::Nick(string _data, Client *_client, Server *_server)
{
	if (!_client->GetAuthentified())
		return;

	if (_data.empty())
	{
		string _msg = ERR_NONICKNAMEGIVEN(_client->GetNick());
		_client->Broadcast(_msg);
		return;
	}

	if (_data[0] == '#' || _data[0] == ':')
	{
		string _msg = ERR_ERROREUSNICKNAME(_client->GetNick(), _data);
		_client->Broadcast(_msg);
		return;
	}

	for (int i = 0; i < (int)_data.size(); i++)
	{
		if (isspace(_data[i]))
		{
			string _msg = ERR_ERROREUSNICKNAME(_client->GetNick(), _data);
			_client->Broadcast(_msg);
			return;
		}
	}

	if (!_server->IsNameAvailable(_data) && !_client->GetBot())
	{
		string _msg = ERR_NICKNAMEINUSE(_client->GetNick(), _data);
		_client->Broadcast(_msg);
		return;
	}

	if (!_client->GetNick().empty())
	{
		_server->UpdateNick(_client->GetNick(), _data);
		string _msg = RPL_NICK(_client->GetNick(), _data);
		_server->Broadcast(_msg);
	}

	if (_client->GetNick().empty() && !_client->GetUser().empty())
	{
		string _msg = RPL_WELCOME(_data);
		_client->Broadcast(_msg);
		cout << "\x1b[1;32m" << *_client << " authentified\x1b[0m" << endl;
	}

	_client->SetNick(_data);
}

bool Command::ParseUser(std::string _data)
{
	istringstream  _iss(_data);
	string _word;
	int _count = 0;

	while (_iss >> _word)
	{
		_count++;

		switch (_count)
		{
			case 2: if (_word != "0")
				return false;
				break;
			case 3: if (_word != "*")
				return false;
				break;
			case 4: if (_word[0] != ':')
				return false;
				break;
		}
	}

	if (_count < 3)
		return false;

	return true;
}

void Command::User(string _data, Client *_client, Server *_server)
{
	if (!_client->GetAuthentified())
		return;

	if (!ParseUser(_data))
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "USER");
		_client->Broadcast(_msg);
		return;
	}

	if (!_client->GetUser().empty())
	{
		string _msg = ERR_ALREADYREGISTERED(_client->GetNick());
		_client->Broadcast(_msg);
		return;
	}

	_client->SetUser(_data);

	if (_data == "<bot> 0 * :<bot>")
		_server->AddBot(*_client);

	if (!_client->GetNick().empty() && !_client->GetUser().empty())
	{
		string _msg = RPL_WELCOME(_client->GetNick());
		_client->Broadcast(_msg);
		cout << "\x1b[1;32m" << *_client << " authentified\x1b[0m" << endl;
	}
}

void Command::Meteo(string _data, Client *_client, Server *_server)
{
	Client* _bot = _server->GetBot("Meteosaurus");

	if (!_bot)
	{
		_client->Broadcast("Meteosaurus isn't on the server\r\n");
		return;
	}

	if (_data.empty())
	{
		_client->Broadcast("METEO needs an argument\r\n");
		return;
	}

	string _msg = _client->GetNick() + "<meteo_data>" + _data;
	_bot->Broadcast(_msg);
}

void Command::Invite(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1)
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "INVITE");
		_client->Broadcast(_msg);
		return;
	}

	Client *_dest =  _server->GetClient(ExtractCommand(_data));
	string _channelName = ExtractArgs(_data);
	Channel *_channel = _server->GetChannel(&_channelName[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckUser(*_client))
	{
		string _msg = ERR_NOTONCHANNEL(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (_channel->GetInviteOnly() && !_channel->CheckAdmin(*_client))
	{
		string _msg = ERR_CHANOPRIVSNEEDED(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (_dest && _channel->CheckUser(*_dest))
	{
		string _msg = ERR_USERONCHANNEL(_client->GetNick(), _dest->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	string _msg = RPL_INVITING(_client->GetNick(), ExtractCommand(_data), _channelName);
	_client->Broadcast(_msg);

	if (_dest)
	{
		_channel->Invite(*_dest);
		_msg = RPL_INVITED(_client->GetNick(), _dest->GetNick(), _channelName);
		_dest->Broadcast(_msg);
	}
}

void Command::Kick(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "KICK");
		_client->Broadcast(_msg);
		return;
	}

	string _channelName = ExtractCommand(_data);
	Channel *_channel = _server->GetChannel(&_channelName[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckUser(*_client))
	{
		string _msg = ERR_NOTONCHANNEL(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckAdmin(*_client))
	{
		string _msg = ERR_CHANOPRIVSNEEDED(_client->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	Client *_dest =  _server->GetClient(ExtractArgs(_data));

	if (_dest && !_channel->CheckUser(*_dest))
	{
		string _msg = ERR_USERNOTINCHANNEL(_client->GetNick(), _dest->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	_channel->Kick(*_client, *_dest, ExtractArgs(_data));
}

void Command::Bot(std::string _data, Client *_client)
{
	string _msg = "" + _data + " isn't a valid command for a bot\r\n";
	_client->Broadcast(_msg);
}

void Command::Join(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "JOIN");
		_client->Broadcast(_msg);
		return;
	}

	Channel *_channel = _server->AddChannel(&_data[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), _data);
		_client->Broadcast(_msg);
		return;
	}

	_channel->JoinChannel(*_client, ExtractCommand(_data));
}

void Command::Part(string _data, Client *_client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "JOIN");
		_client->Broadcast(_msg);
		return;
	}

	Channel *_channel = _server->GetChannel(&ExtractCommand(_data)[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), ExtractCommand(_data));
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckUser(*_client))
	{
		string _msg = ERR_NOTONCHANNEL(_client->GetNick(), ExtractCommand(_data));
		_client->Broadcast(_msg);
		return;
	}

	_channel->QuitChannel(*_client, &ExtractMsg(_data)[1]);

	if (_channel->Users() == 0)
	{
		_server->AddToRemoveChannel(*_channel);
		_server->RemoveChannel();
	}
}

void Command::Quit(string _data, Client *_client, Server *_server)
{
	string _reason = ExtractCommand(_data);

	if (_reason[0] == ':')
		_reason.erase(0, 1);

	_server->RemoveClient(*_client, _reason);
}

void Command::Ping(std::string _data, Client *_client, Server *_server)
{
	if (_data.empty())
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "PING");
		_client->Broadcast(_msg);
		return;
	}

	(void)_server;
	string _msg = RPL_PONG(ExtractArgs(_data));
	_client->Broadcast(_msg);
}

void Command::Msg(std::string _data, Client *_client, Server *_server)
{
	string _destName = ExtractCommand(_data);

	if (_destName.empty())
	{
		string _msg = ERR_NORECIPIENT(_client->GetNick(), "PRIVMSG");
		_client->Broadcast(_msg);
		return;
	}

	Client* _destClient = _server->GetClient(_destName);
	Channel* _destChannel = _server->GetChannel(&_destName[1]);

	if (!_destClient && !_destChannel)
	{
		string _msg = ERR_NOSUCHNICK(_client->GetNick(), _destName);
		_client->Broadcast(_msg);
		return;
	}

	if (_destChannel && !_destChannel->CheckUser(*_client))
	{
		string _msg = ERR_CANNOTSENDTOCHAN(_client->GetNick(), _destName);
		_client->Broadcast(_msg);
		return;
	}

	if (ExtractMsg(_data).empty())
	{
		string _msg = ERR_NOTEXTTOSEND(_client->GetNick());
		_client->Broadcast(_msg);
		return;
	}

	if (_destChannel)
	{
		string _msg = RPL_PRIVMSG(_client->GetNick(), "#" + _destChannel->Name(), ExtractMsg(_data));
		_destChannel->Broadcast(_msg);
		cout << "\x1b[1;37m" << *_client << " Send Message To " << *_destChannel << "\x1b[0m" << endl;
	}
	else
	{
		string _msg = RPL_PRIVMSG(_client->GetNick(), _destClient->GetNick(), ExtractMsg(_data));
		_destClient->Broadcast(_msg);
		cout << "\x1b[1;37m" << *_client << " Send Message To " << *_destClient << "\x1b[0m" << endl;
	}

}

void Command::Names(string _data, Client* _client, Server *_server)
{
	if (_data.size() <= 1 || _data[0] != '#')
	{
		_client->Broadcast("NAMES needs a channel in argument");
		return;
	}

	Channel *_channel = _server->GetChannel(&_data[1]);

	if (_channel)
		_client->Broadcast(_channel->Names(*_client));
}

string Command::ExtractCommand(string _data)
{
	size_t _pos = _data.find_first_of(" \t\n\r");

	if (_pos == string::npos)
		return _data;

	return _data.substr(0, _pos);
}

string Command::ExtractArgs(string _data)
{
	size_t _firstNoSpace = _data.find_first_not_of(" \t\n\r");

	if (_firstNoSpace == string::npos)
		return "";

	size_t _endFirst = _data.find_first_of(" \t\n\r", _firstNoSpace);

	if (_endFirst == string::npos)
		return "";

	size_t _firstNoSpaceEnd = _data.find_first_not_of(" \t\n\r", _endFirst);

	if (_firstNoSpaceEnd == string::npos)
		return "";

	int _offset = 0;

	if (_data[_data.size() - 1 - _offset] == '\n')
		_offset++;

	if (_data[_data.size() - 1 - _offset] == '\r')
		_offset++;

	return _data.substr(_firstNoSpaceEnd, _data.size() - _firstNoSpaceEnd - _offset);
}

string Command::ExtractMsg(string _data)
{
	size_t _firstNoSpace = _data.find_first_not_of(" \t\n\r");

	if (_firstNoSpace == string::npos)
		return "";

	size_t _endFirst = _data.find_first_of(" \t\n\r", _firstNoSpace);

	if (_endFirst == string::npos)
		return "";

	size_t _firstNoSpaceEnd = _data.find_first_not_of(" \t\n\r", _endFirst);

	if (_firstNoSpaceEnd == string::npos)
		return "";

	int _offset = 0;

	if (_data[_data.size() - 1 - _offset] == '\r')
		_offset++;

	return _data.substr(_firstNoSpaceEnd, _data.size() - _firstNoSpaceEnd - _offset) + "\n";
}

void Command::List(string _data, Client *_client, Server *_server)
{
	(void)_data;
	_client->Broadcast(_server->List(*_client));
}
