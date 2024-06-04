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
	if (admin.empty())
		return false;

	if (find(admin.begin(), admin.end(), _client) == admin.end())
		return false;
	return true;
}

bool Channel::CheckUser(Client _client)
{
	if (user.empty())
		return false;

	if (find(user.begin(), user.end(), _client) == user.end())
		return false;
	return true;
}

bool Channel::CheckInvited(Client _client)
{
	if (invited.empty())
		return false;

	if (find(invited.begin(), invited.end(), _client) == invited.end())
		return false;
	return true;
}

void Channel::RemoveFrom(vector<Client>* _vector, Client _client)
{
	_vector->erase(remove(_vector->begin(), _vector->end(), _client), _vector->end());
}

void Channel::JoinChannel(Client _client)
{
	if (inviteOnly && !CheckInvited(_client))
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
	RemoveFrom(&invited, _client);

	if (admin.size() == 0)
		admin.push_back(_client);

	string _msg = "\x1b[1;32mYou have joined " + name + "\x1b[0m\n";
	_client.Broadcast(_msg);
	cout << "\x1b[1;32m" << _client << " Joined " << *this << endl;
}

void Channel::Invite(Client _client, Client _dest)
{
	if (!CheckAdmin(_client))
	{
		_client.Broadcast("\x1b[1;31mYou aren't allowed to do that\n\x1b[0m");
		return;
	}

	if (CheckUser(_dest))
	{
		_client.Broadcast("\x1b[1;31mThis user is already in the channel\n\x1b[0m");
		return;
	}

	if (!CheckInvited(_client))
		invited.push_back(_client);

	string _msg = "\x1b[1;32mYou invited " + _dest.GetNick() + " to " + name + "\n\x1b[0m";
	_client.Broadcast(_msg);
	_msg = "\x1b[1;32mYou have been invited to " + name + " by " + _client.GetNick() + "\n\x1b[0m";
	_dest.Broadcast(_msg);
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
	cout << "\x1b[1;32m" << _client << " Left " << *this << endl;
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
}

void Channel::Broadcast(string _msg)
{
	for(size_t i = 0; i < user.size(); i++)
		user[i].Broadcast(_msg);
}

string Channel::Who()
{
	string _msg = "\x1b[1;37m--- " + name + " ---\n";

	for(size_t i = 0; i < admin.size(); i++)
	{
		Client _client = admin[i];
		_msg += _client.GetNick();
		_msg += ": operator\n";
	}

	for(size_t i = 0; i < user.size(); i++)
	{
		Client _client = user[i];
		_msg += _client.GetNick();
		_msg += ": operator\n";
	}

	for(size_t i = 0; i < invited.size(); i++)
	{
		Client _client = invited[i];
		_msg += _client.GetNick();
		_msg += ": invited\n";
	}

	_msg += "--- " + name + " ---\x1b[0m";
	return _msg;
}

string Channel::Name() const
{
	return name;
}

bool Channel::IsIn(Client _client)
{
	return CheckUser(_client);
}

ostream& operator<<(ostream& _os, const Channel& _channel)
{
	string _output = "Channel <";
	_output += _channel.Name();
	_output += ">";
	_os << _output;
	return _os;
}