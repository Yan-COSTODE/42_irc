#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "define.hpp"

using namespace std;

class Client;

class Channel {
	private:
		bool inviteOnly;
		bool topicAdmin;
		string name;
		string password;
		string topic;
		string topicTime;
		string topicNick;
		int userLimit;
		int adminNumber;
		int userNumber;
		int invitedNumber;
		vector<Client> admin;
		vector<Client> user;
		vector<Client> invited;

	private:
		void RemoveUser(Client _client);
		void RemoveAdmin(Client _client);
		void RemoveInvited(Client _client);

	public:
		Channel();
		Channel(string _name);
		int Users();
		int GetUserLimit();
		string Name() const;
		string GetPassword();
		bool GetInviteOnly();
		string GetTopic() const;
		void Invite(Client _dest);
		bool IsIn(Client _client);
		bool GetTopicAdmin() const;
		void Broadcast(string _msg);
		string Names(Client _client);
		void SetUserLimit(int _limit);
		bool CheckUser(Client _client);
		bool CheckAdmin(Client _client);
		void SetInviteOnly(bool _status);
		void SetTopicAdmin(bool _status);
		bool CheckInvited(Client _client);
		void SetPassword(string _password);
		void BroadcastTopic(Client _client);
		void UpdateNick(string _old, string _new);
		void SetTopic(string _nick, string _topic);
		void ToggleAdmin(Client _admin, Client _client);
		void JoinChannel(Client _client, string _password);
		void QuitChannel(Client _client, string _reason);
		void Kick(Client _admin, Client _client, string _reason);
};

ostream& operator<<(ostream& _os, const Channel& _channel);

#endif
