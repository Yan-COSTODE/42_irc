#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(std::string _name)
{
	name = _name;
	inviteOnly = false;
	topicAdmin = true;
	password = "";
	topic = "";
	userLimit = -1;
}

void Channel::JoinChannel(Client _client)
{
	(void)_client;
}

void Channel::SetAdminStatus(Client _client, bool _status)
{
	(void)_client;
	(void)_status;
}