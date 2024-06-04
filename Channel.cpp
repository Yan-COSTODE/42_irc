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

Channel::Channel(string _name)
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

bool Channel::CheckUser(Client _client)
{
	if (find(admin.begin(), admin.end(), _client) == admin.end())
		return false;
	return true;
}

void Channel::RemoveFrom(vector<Client>* _vector, Client _client)
{
	_vector->erase(remove(_vector->begin(), _vector->end(), _client), _vector->end());
}

void Channel::JoinChannel(Client _client)
{
	if (inviteOnly && find(invited.begin(), invited.end(), _client) == invited.end())
	{
		string _msg = "\x1b[1;31mYou have not been invited to " + name + "\x1b[0m\n";
		_client.Broadcast(_msg);
		return ;
	}
	if (CheckUser(_client))
	{
		string _msg = "\x1b[1;31mYou already have joined " + name + "\x1b[0m\n";
		_client.Broadcast(_msg);
		return ;
	}

	if (userLimit == -1 && user.size() == (size_t)userLimit)
		return ;

	user.push_back(_client);

	if (admin.size() == 0)
		admin.push_back(_client);

	string _msg = "\x1b[1;32mYou have joined " + name + "\x1b[0m\n";
	_client.Broadcast(_msg);
}

void Channel::AddInvited(Client _client)
{
	invited.push_back(_client);
}


int Channel::Users()
{
	return user.size();
}

void Channel::QuitChannel(Client _client)
{
	if (!CheckUser(_client))
	{
		string _msg = "\x1b[1;31mYou are not in " + name + "\x1b[0m\n";
		_client.Broadcast(_msg);
		return;
	}

	RemoveFrom(&user, _client);
	RemoveFrom(&admin, _client);

	string _msg = "\x1b[1;32mYou quit " + name + "\x1b[0m\n";
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
		RemoveFrom(&admin, _client);
		_admin.Broadcast("\x1b[1;31mYou aren't allowed to do that\n\x1b[0m");
		return;
	}
	else
	{
		admin.push_back(_client);
		string _msg = "\x1b[1;32mYou made " + _client.GetNick() + "an operator\x1b[0m\n";
		_admin.Broadcast(_msg);
		_msg = "\x1b[1;32m" + _admin.GetNick() + "made you an operator\x1b[0m\n";
		_client.Broadcast(_msg);
		return;
	}
	(void)_client;
}

void Channel::Broadcast(string _msg)
{
	for(size_t i = 0; i < user.size(); i++)
		user[i].Broadcast(_msg);
}

string Channel::Who()
{
	string _msg = "\x1b[1;37m--- " + name + " ---\n";

	for(size_t i = 0; i < user.size(); i++)
	{
		Client _client = user[i];

		if (CheckAdmin(_client))
		{
			_msg += _client.GetNick();
			_msg += ": operator\n";
		}
		else
		{
			_msg += _client.GetNick();
			_msg += "\n";
		}
	}

	_msg += "\x1b[0m";
	return _msg;
}

string Channel::Name()
{
	return name;
}