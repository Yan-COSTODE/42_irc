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

void Channel::JoinChannel(Client _client, string _password)
{
	if (!password.empty() && password != _password)
	{
		string _msg = ERR_BADCHANNELKEY(_client.GetNick(), "#" + name);
		_client.Broadcast(_msg);
		return ;
	}

	if (userLimit != -1 && user.size() == (size_t)userLimit)
	{
		string _msg = ERR_CHANNELISFULL(_client.GetNick(), "#" + name);
		_client.Broadcast(_msg);
		return ;
	}

	if (inviteOnly && !CheckInvited(_client))
	{
		string _msg = ERR_INVITEONLYCHAN(_client.GetNick(), "#" + name);
		_client.Broadcast(_msg);
		return ;
	}

	if (CheckUser(_client))
		return ;

	user.push_back(_client);
	userNumber++;

	if (CheckInvited(_client))
		RemoveInvited(_client);

	if (admin.size() == 0)
	{
		password = _password;
		admin.push_back(_client);
		adminNumber++;
	}

	string _msg;
	_msg = RPL_JOIN(_client.GetNick(), "#" + name);
	Broadcast(_msg);

	BroadcastTopic(_client);

	_client.Broadcast(Names(_client));
	cout << "\x1b[1;32m" << _client << " Joined " << *this << "\x1b[0m" << endl;
}

void Channel::Invite(Client _dest)
{
	if (!CheckInvited(_dest))
	{
		invited.push_back(_dest);
		invitedNumber++;
	}
}

int Channel::Users()
{
	return user.size();
}

void Channel::QuitChannel(Client _client, string _reason)
{
	string _msg = RPL_PART(_client.GetNick(), "#" + name, _reason);
	Broadcast(_msg);
	RemoveUser(_client);

	if (CheckAdmin(_client))
		RemoveAdmin(_client);

	cout << "\x1b[1;31m" << _client << " Left " << *this << "\x1b[0m" << endl;
}

void Channel::ToggleAdmin(Client _admin, Client _client)
{
	if (!CheckAdmin(_admin))
		return;

	if (_admin == _client)
	{
		string _msg = ERR_UNKNOWNERROR(_admin.GetNick(), "MODE", "You can't change your own operator mode");
		_admin.Broadcast(_msg);
		return;
	}

	if (CheckAdmin(_client))
	{
		string _msg = RPL_MODE(_admin.GetNick(), "#" + name, "-o " + _client.GetNick());
		Broadcast(_msg);
		RemoveAdmin(_client);
		return;
	}
	else
	{
		string _msg = RPL_MODE(_admin.GetNick(), "#" + name, "+o " + _client.GetNick());
		Broadcast(_msg);
		admin.push_back(_client);
		adminNumber++;
		return;
	}
}

void Channel::Kick(Client _admin, Client _client, string _reason)
{
	if (_admin == _client)
		return;

	string _msg = RPL_KICK(_admin.GetNick(), "#" +  name, _client.GetNick(), _reason);
	Broadcast(_msg);
	RemoveUser(_client);

	if (CheckAdmin(_client))
		RemoveAdmin(_client);

	cout << "\x1b[1;32m" << _client << " Left " << *this  << "\x1b[0m" << endl;
	return;
}

bool Channel::GetTopicAdmin() const
{
	return topicAdmin;
}

void Channel::SetTopic(string _nick, string _topic)
{
	time_t _time;
	time(&_time);
	stringstream _ss;
	_ss << _time;
	topicTime = _ss.str();
	topicNick = _nick;
	topic = _topic;

	for (int i = 0; i < (int)user.size(); ++i)
		BroadcastTopic(user[i]);
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

string Channel::Names(Client _client)
{
	string _msg;

	for(int i = 0; i < adminNumber; i++)
	{
		Client _admin = admin[i];
		_msg += RPL_NAMEREPLY(_client.GetNick(), "=", "#" + name, "@", _admin.GetNick()) + "\n";
	}

	for(int i = 0; i < userNumber; i++)
	{
		Client _user = user[i];

		if(CheckAdmin(_client))
			continue;

		_msg += RPL_NAMEREPLY(_client.GetNick(), "=", "#" + name, "", _user.GetNick()) + "\n";
	}

	_msg += RPL_ENDOFNAMES(_client.GetNick(), "#" + name);
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

bool Channel::GetInviteOnly() {
	return inviteOnly;
}

void Channel::BroadcastTopic(Client _client)
{
	string _msg;

	if (topic.empty())
	{
		_msg = RPL_NOTOPIC(_client.GetNick(), "#" + name);
		_client.Broadcast(_msg);
	}
	else
	{
		_msg = RPL_TOPIC(_client.GetNick(), "#" + name, topic) + "\n";
		_msg += RPL_TOPICWHOTIME(_client.GetNick(), "#" + name, topicNick, topicTime);
		_client.Broadcast(_msg);
	}
}

void Channel::UpdateNick(string _old, string _new)
{
	for(int i = 0; i < adminNumber; i++)
	{
		if (admin[i].GetNick() == _old)
			admin[i].SetNick(_new);

	}

	for(int i = 0; i < userNumber; i++)
	{
		if (user[i].GetNick() == _old)
			user[i].SetNick(_new);

	}

	for(int i = 0; i < invitedNumber; i++)
	{
		if (invited[i].GetNick() == _old)
			invited[i].SetNick(_new);

	}
}

void Channel::SetInviteOnly(bool _status)
{
	inviteOnly = _status;
}

void Channel::SetTopicAdmin(bool _status)
{
	topicAdmin = _status;
}

string Channel::GetPassword()
{
	return password;
}

void Channel::SetPassword(string _password)
{
	password = _password;
}

void Channel::SetUserLimit(int _limit)
{
	userLimit = _limit;
}

int Channel::GetUserLimit() {
	return userLimit;
}

ostream& operator<<(ostream& _os, const Channel& _channel)
{
	string _output = "Channel <";
	_output += _channel.Name();
	_output += ">";
	_os << _output;
	return _os;
}