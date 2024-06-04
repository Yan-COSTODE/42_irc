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
	userNumber = 0;
	adminNumber = 0;
	invitedNumber = 0;
}

Channel::Channel(string _name)
{
	name = _name;
	inviteOnly = false;
	topicAdmin = true;
	password = "";
	topic = "";
	userLimit = -1;
	userNumber = 0;
	adminNumber = 0;
	invitedNumber = 0;
}

bool Channel::CheckAdmin(Client _client)
{
	if (adminNumber == 0)
		return false;

	if (find(admin.begin(), admin.end(), _client) == admin.end())
		return false;
	return true;
}

bool Channel::CheckUser(Client _client)
{
	if (userNumber == 0)
		return false;

	if (find(user.begin(), user.end(), _client) == user.end())
		return false;
	return true;
}

bool Channel::CheckInvited(Client _client)
{
	if (invitedNumber == 0)
		return false;

	if (find(invited.begin(), invited.end(), _client) == invited.end())
		return false;

	return true;
}

void Channel::RemoveAdmin(Client _client)
{
	if (adminNumber == 0)
		return;

	admin.erase(remove(admin.begin(), admin.end(), _client), admin.end());
	adminNumber--;
}

void Channel::RemoveUser(Client _client)
{
	if (userNumber == 0)
		return;

	user.erase(remove(user.begin(), user.end(), _client), user.end());
	userNumber--;
}

void Channel::RemoveInvited(Client _client)
{
	if (invitedNumber == 0)
		return;

	invited.erase(remove(invited.begin(), invited.end(), _client), invited.end());
	invitedNumber--;
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
	userNumber++;

	if (CheckInvited(_client))
		RemoveInvited(_client);

	if (admin.size() == 0)
	{
		admin.push_back(_client);
		adminNumber++;
	}

	string _msg = "\x1b[1;32mYou have joined " + name + "\x1b[0m\n";
	_client.Broadcast(_msg);
	cout << "\x1b[1;32m" << _client << " Joined " << *this << endl;
	_msg = "\x1b[1;32m" + _client.GetNick() + " joined the channel " + name + "\n\x1b[0m";
	Broadcast(_msg);
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

	if (!CheckInvited(_dest))
	{
		invited.push_back(_dest);
		invitedNumber++;
	}

	string _msg = "\x1b[1;32mYou invited " + _dest.GetNick() + " to " + name + "\n\x1b[0m";
	_client.Broadcast(_msg);
	_msg = "\x1b[1;32mYou have been invited to " + name + " by " + _client.GetNick() + "\n\x1b[0m";
	_dest.Broadcast(_msg);
}

int Channel::Users()
{
	return user.size();
}

void Channel::QuitChannel(Client _client, bool _sendMsg)
{
	if (!CheckUser(_client))
	{
		string _msg = "\x1b[1;31mYou are not in " + name + "\x1b[0m\n";
		_client.Broadcast(_msg);
		return;
	}

	RemoveUser(_client);

	if (CheckAdmin(_client))
		RemoveAdmin(_client);

	if (_sendMsg)
	{
		string _msg = "\x1b[1;32mYou quit " + name + "\x1b[0m\n";
		_client.Broadcast(_msg);
	}

	cout << "\x1b[1;32m" << _client << " Left " << *this << endl;
	string _msg = "\x1b[1;31m" + _client.GetNick() + " left the channel " + name + "\n\x1b[0m";
	Broadcast(_msg);
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
		RemoveAdmin(_client);
		_admin.Broadcast("\x1b[1;31mYou oui\n\x1b[0m");
		return;
	}
	else
	{
		admin.push_back(_client);
		adminNumber++;
		string _msg = "\x1b[1;32mYou made " + _client.GetNick() + "an operator\x1b[0m\n";
		_admin.Broadcast(_msg);
		_msg = "\x1b[1;32m" + _admin.GetNick() + "made you an operator\x1b[0m\n";
		_client.Broadcast(_msg);
		return;
	}
}

void Channel::Kick(Client _admin, Client _client, string _reason)
{
	if (!CheckAdmin(_admin))
	{
		_admin.Broadcast("\x1b[1;31mYou aren't allowed to do that\n\x1b[0m");
		return;
	}

	if (_admin == _client)
	{
		_admin.Broadcast("\x1b[1;31mYou can't kick yourself\n\x1b[0m");
		return;
	}

	if (!CheckUser(_client))
	{
		string _msg = "\x1b[1;31mNo " + _client.GetNick() + " found in " + name + "\x1b[0m\n";
		_admin.Broadcast(_msg);
		return;
	}

	RemoveUser(_client);

	if (CheckAdmin(_client))
		RemoveAdmin(_client);

	string _msg = "\x1b[1;32mYou kicked " + _client.GetNick() + " from " + name + "\n\x1b[0m";
	_admin.Broadcast(_msg);
	_msg = "\x1b[1;31m" + _admin.GetNick() + " kicked you from" + name;

	if (_reason.empty())
		_msg += "\x1b[0m\n";
	else
		_msg += " because " + _reason + "\x1b[0m\n";

	_client.Broadcast(_msg);
	cout << "\x1b[1;32m" << _client << " Left " << *this << endl;
	_msg = "\x1b[1;31m" + _client.GetNick() + " has been kick from the channel " + name;

	if (_reason.empty())
		_msg += "\x1b[0m\n";
	else
		_msg += " because " + _reason + "\x1b[0m\n";

	Broadcast(_msg);
	return;
}

bool Channel::GetTopicAdmin() const
{
	return topicAdmin;
}

void Channel::SetTopic(string _topic)
{
	topic = _topic;
}

string Channel::GetTopic() const
{
	return topic;
}

void Channel::Broadcast(string _msg)
{
	for(size_t i = 0; i < user.size(); i++)
		user[i].Broadcast(_msg);
}

string Channel::Who()
{
	string _msg = "\x1b[1;37m--- " + name + " ---\n";

	for(int i = 0; i < adminNumber; i++)
	{
		Client _client = admin[i];
		_msg += _client.GetNick();
		_msg += ": operator\n";
	}

	for(int i = 0; i < userNumber; i++)
	{
		Client _client = user[i];

		if(CheckAdmin(_client))
			continue;

		_msg += _client.GetNick();
		_msg += "\n";
	}

	for(int i = 0; i < invitedNumber; i++)
	{
		Client _client = invited[i];
		_msg += _client.GetNick();
		_msg += ": invited\n";
	}

	_msg += "--- " + name + " ---\n\x1b[0m";
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