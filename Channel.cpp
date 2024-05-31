#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel()
{
	name = "Channel";
	inviteOnly = false;
	topicAdmin = true;
	password = "";
	topic = "";
	userLimit = -1;
}

Channel::Channel(std::string _name)
{
	name = _name;
	inviteOnly = false;
	topicAdmin = true;
	password = "";
	topic = "";
	userLimit = -1;
}

bool Channel::CheckAdmin(Client _client)
{
	if (find(admin.begin(), admin.end(), _client) == admin.end())
		return false;
	return true;
}

void Channel::JoinChannel(Client _client)
{
	if (userLimit == -1 && user.size() == (size_t)userLimit)
		return;

	user.push_back(_client);

	if (admin.size() == 0)
		admin.push_back(_client);

	string _msg = "\x1b[1;37mYou have joined ";
	_msg += name;
	_msg += "\x1b[0m\n";
	_client.Broadcast(_msg);
}

void Channel::ToggleAdmin(Client _admin, Client _client)
{
	if (!CheckAdmin(_admin))
	{
		_admin.Broadcast("\x1b[1;31mYou aren't allowed to do that\n\x1b[0m");
		return;
	}

	if (_admin == _client)
	{
		_admin.Broadcast("\x1b[1;31mYou can't change your own operator mode\n\x1b[0m");
		return;
	}

	if (CheckAdmin(_client))
	{
		for(size_t i = 0; i < admin.size(); i++)
		{
			if (admin[i].Nickname() == _client.Nickname())
			{
				admin.erase(admin.begin() + i);
				break;
			}
		}
		_admin.Broadcast("\x1b[1;31mYou aren't allowed to do that\n\x1b[0m");
		return;
	}
	else
	{
		admin.push_back(_client);
		string _msg = "\x1b[1;37mYou made ";
		_msg += _client.Nickname();
		_msg += "an operator\x1b[0m\n";
		_admin.Broadcast(_msg);
		_msg = "\x1b[1;37m";
		_msg += _admin.Nickname();
		_msg += "made you an operator\x1b[0m\n";
		_client.Broadcast(_msg);
		return;
	}
	(void)_client;
}

void Channel::Broadcast(std::string _msg)
{
	for(size_t i = 0; i < user.size(); i++)
		user[i].Broadcast(_msg);
}