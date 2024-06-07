#include "Command.hpp"

void Command::Parse(string _data, Client* _client, Server* _server)
{
	if (!_client)
		return;

	string _cmd = ExtractCommand(_data);

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
	else if (_cmd == "HELP")
		Help(ExtractArgs(_data), _client, _server);
	else
	{
		string _msg = ERR_UNKNOWNERROR(_client->GetNick(), _cmd, "Command not found");
		_client->Broadcast(_msg);
		return;
	}
}

int Command::GetFirstArg(vector<string>cmds)
{
	int i = 1;

	while (i < (int)cmds.size() && (cmds.at(i)[0] == '-' || cmds.at(i)[0] == '+'))
		i++;

	return i;
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

void Command::ModeI(string flag, Channel *_channel, Client *_client)
{
	if (flag == "+i")
	{
		if (!_channel->GetInviteOnly())
		{
			string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "+i");
			_channel->Broadcast(_msg);
			_channel->SetInviteOnly(true);
		}
		else
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "Invite only mode is already set");
			_client->Broadcast(_msg);
		}
	}
	else
	{
		if (_channel->GetInviteOnly())
		{
			string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "-i");
			_channel->Broadcast(_msg);
			_channel->SetInviteOnly(false);
		}
		else
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "Invite only mode is already unset");
			_client->Broadcast(_msg);
		}
	}
}

void Command::ModeT(string flag, Channel *_channel, Client *_client)
{
	if (flag == "+t")
	{
		if (!_channel->GetTopicAdmin())
		{
			string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "+t");
			_channel->Broadcast(_msg);
			_channel->SetTopicAdmin(true);
		}
		else
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "Topic operator mode is already set");
			_client->Broadcast(_msg);
		}
	}
	else
	{
		if (_channel->GetTopicAdmin())
		{
			string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "-t");
			_channel->Broadcast(_msg);
			_channel->SetTopicAdmin(false);
		}
		else
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "Topic operator mode is already unset");
			_client->Broadcast(_msg);
		}
	}
}

void Command::ModeK(string flag, Channel *_channel, Client *_client, string arg)
{
	if (flag == "-k")
	{
		(void)arg;
		if (_channel->GetPassword().empty())
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "Channel password is already unset");
			_client->Broadcast(_msg);
			return;
		}
		else
		{
			string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "-k");
			_channel->Broadcast(_msg);
			_channel->SetPassword("");
		}
	}
	else
	{
		string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "+k " + arg);
		_channel->Broadcast(_msg);
		_channel->SetPassword(arg);
	}
}

void Command::ModeL(string flag, Channel *_channel, Client *_client, string arg)
{
	if (flag == "+l")
	{
		if (atoi(arg.c_str()) <= 0)
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "User limit cannot be set under 1");
			_client->Broadcast(_msg);
			return;
		}

		string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "+l " + arg);
		_channel->Broadcast(_msg);
		_channel->SetUserLimit(atoi(arg.c_str()));
	}
	else
	{
		if (_channel->GetUserLimit() == -1)
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", "User limit is already unset");
			_client->Broadcast(_msg);
			return;
		}

		string _msg = RPL_MODE(_client->GetNick(), "#" + _channel->Name(), "-l");
		_channel->Broadcast(_msg);
		_channel->SetUserLimit(-1);
	}
}

void Command::ModeO(string flag, Channel *_channel, Client *_client, string arg, Server *_server)
{
	Client *_target = _server->GetClient(arg);

	if (!_target)
	{
		string _msg = ERR_NOSUCHNICK(_client->GetNick(), arg);
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckUser(*_target))
	{
		string _msg = ERR_USERNOTINCHANNEL(_client->GetNick(), _target->GetNick(), "#" + _channel->Name());
		_client->Broadcast(_msg);
		return;
	}

	if (flag == "+o")
	{
		if (_channel->CheckAdmin(*_target))
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", _target->GetNick() + " is already an operator");
			_client->Broadcast(_msg);
			return;
		}

		_channel->ToggleAdmin(*_client, *_target);
	}
	else
	{
		if (!_channel->CheckAdmin(*_target))
		{
			string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "MODE", _target->GetNick() + " is already not an operator");
			_client->Broadcast(_msg);
			return;
		}

		_channel->ToggleAdmin(*_client, *_target);
	}
}


int Command::IsFlag(string flag)
{
	string flags[6] = {"+i", "-i", "+t", "-t", "-k", "-l"};
	string ArgFlags[4] = {"+l", "+o", "-o", "+k"};

	for (int i = 0; i < 6; i++)
	{
		if (flag == flags[i])
			return 1;
	}

	for (int i = 0; i < 4; i++)
	{
		if (flag == ArgFlags[i])
			return 2;
	}

	return 0;
}

void Command::Mode(string _data, Client *_client, Server *_server)
{
	Channel *_channel = _server->GetChannel(&ExtractCommand(_data)[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), "#" + &ExtractCommand(_data)[1]);
		_client->Broadcast(_msg);
		return ;
	}

	if (!_channel->CheckAdmin(*_client))
	{
		string _msg = ERR_CHANOPRIVSNEEDED(_client->GetNick(), "#" + &ExtractCommand(_data)[1]);
		_client->Broadcast(_msg);
		return ;
	}

	vector<string>cmds = SplitString(_data);

	int i = 1;
	int j = GetFirstArg(cmds);
	int k = j;

	while (i < (int)cmds.size() && IsFlag(cmds.at(i)))
	{
		char flag = cmds.at(i)[1];
		string arg = "";

		if (IsFlag(cmds.at(i)) == 2)
		{
			if (j < (int)cmds.size())
				arg = cmds.at(j++);
		}

		cout << "[" << flag << "]: " << arg << endl;
		switch (flag) {
			case 'i':
				ModeI(cmds.at(i), _channel, _client);
				break;
			case 't':
				ModeT(cmds.at(i), _channel, _client);
				break;
			case 'k':
				ModeK(cmds.at(i), _channel, _client, arg);
				break;
			case 'l':
				ModeL(cmds.at(i), _channel, _client, arg);
				break;
			case 'o':
				ModeO(cmds.at(i), _channel, _client, arg, _server);
				break;
			default:
				break;
		}
		i++;
	}

	if (i != k)
	{
		string _msg = ERR_UMODEUNKNOWNFLAG(_client->GetNick());
		_client->Broadcast(_msg);
	}
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
		string _msg = ERR_UNKNOWNERROR(_client->GetNick(), "METEO", "Meteosaurus isn't on the server");
		_client->Broadcast(_msg);
		return;
	}

	if (_data.empty())
	{
		string _msg = ERR_NEEDMOREPARAMS(_client->GetNick(), "METEO");
		_client->Broadcast(_msg);
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

	if (!_dest)
	{
		string _msg = ERR_NOSUCHNICK(_client->GetNick(), ExtractCommand(_data));
		_client->Broadcast(_msg);
		return;
	}

	if (_channel->CheckUser(*_dest))
	{
		string _msg = ERR_USERONCHANNEL(_client->GetNick(), _dest->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	string _msg = RPL_INVITING(_client->GetNick(), ExtractCommand(_data), _channelName);
	_client->Broadcast(_msg);
	_channel->Invite(*_dest);
	_msg = RPL_INVITED(_client->GetNick(), _dest->GetNick(), _channelName);
	_dest->Broadcast(_msg);
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

	if (!_dest)
	{
		string _msg = ERR_NOSUCHNICK(_client->GetNick(), ExtractArgs(_data));
		_client->Broadcast(_msg);
		return;
	}

	if (!_channel->CheckUser(*_dest))
	{
		string _msg = ERR_USERNOTINCHANNEL(_client->GetNick(), _dest->GetNick(), _channelName);
		_client->Broadcast(_msg);
		return;
	}

	_channel->Kick(*_client, *_dest, ExtractArgs(_data));
}

void Command::Bot(std::string _data, Client *_client)
{
	string _msg = _data + " isn't a valid command for a bot";
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

	Channel *_channel = _server->AddChannel(&ExtractCommand(_data)[1]);

	if (!_channel)
	{
		string _msg = ERR_NOSUCHCHANNEL(_client->GetNick(), _data);
		_client->Broadcast(_msg);
		return;
	}

	_channel->JoinChannel(*_client, ExtractArgs(_data));
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

void Command::Help(string _data, Client *_client, Server *_server)
{
	(void)_server;
	if (_data.empty() || _data == "HELP")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** HELP System **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "Try /HELP <command> for specific help,");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "/HELP USERCMDS to list available commands");
		_client->Broadcast(_msg);
		return;
	}

	if (_data == "USERCMDS")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** HELP System **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/HELP [<subject>]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/NICK <nickname>");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/PING <token>");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/QUIT [<reason>]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/JOIN <channel> [<key>]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/PART <channel> [<reason>]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/TOPIC <channel> [<topic>]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/NAMES <channel>");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/LIST");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/INVITE <nickname> <channel>");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/KICK <channel> <user> [:<reason>]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/MODE <target> <modestring> [<mode arguments>...]");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "/PRIVMSG <target> <text to be sent>");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "/METEO <location>");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "NICK")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The NICK command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /NICK command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to change your nickname on the server.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "PING")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The PING command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /PING command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to check your latency with the server.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "QUIT")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The QUIT command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /QUIT command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to disconnect from the server.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "JOIN")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The JOIN command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /JOIN command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to join a channel in the server.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "PART")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The PART command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /PART command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to quit a channel in the server.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "TOPIC")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The TOPIC command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /TOPIC command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to see or change the topic in a channel.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "NAMES")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The NAMES command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /NAMES command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to see all members of a channel.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "LIST")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The LIST command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /LIST command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to see all channel in the server.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "INVITE")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The INVITE command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /INVITE command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to invite someone in a channel.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "KICK")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The KICK command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /KICK command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to kick someone from a channel.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "MODE")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The MODE command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /MODE command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to change a channel configuration.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "PRIVMSG")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The PRIVMSG command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /PRIVMSG command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to send a message to someone.");
		_client->Broadcast(_msg);
		return;
	}
	else if (_data == "METEO")
	{
		string _msg = RPL_HELPSTART(_client->GetNick(), "*", "** The METEO command **");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "");
		_msg += RPL_HELPTXT(_client->GetNick(), "*", "The /METEO command is the main way");
		_msg += RPL_ENDOFHELP(_client->GetNick(), "*", "to see the current meteo in a location.");
		_client->Broadcast(_msg);
		return;
	}
	else
	{
		string _msg = ERR_HELPNOTFOUND(_client->GetNick(), "*");
		_client->Broadcast(_msg);
		return;
	}
}
