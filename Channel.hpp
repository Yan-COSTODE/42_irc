#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

class Client;

class Channel {
	private:
		string name;
		bool inviteOnly;
		bool topicAdmin;
		string password;
		string topic;
		int userLimit;
		int adminNumber;
		int userNumber;
		int invitedNumber;
		vector<Client> admin;
		vector<Client> user;
		vector<Client> invited;

	private:
		void RemoveAdmin(Client _client);
		void RemoveUser(Client _client);
		void RemoveInvited(Client _client);

public:
		Channel();
		Channel(string _name);
		void JoinChannel(Client _client);
		void Invite(Client _client, Client _dest);
		void QuitChannel(Client _client, bool _sendMsg);
		void ToggleAdmin(Client _admin, Client _client);
		void Kick(Client _admin, Client _client, string _reason);
		void Broadcast(string _msg);
		bool GetTopicAdmin() const;
		bool IsIn(Client _client);
		string GetTopic() const;
		string Name() const;
		string Who();
		int Users();
		bool CheckAdmin(Client _client);
		bool CheckUser(Client _client);
		bool CheckInvited(Client _client);
		void SetTopic(string _topic);
};

ostream& operator<<(ostream& _os, const Channel& _channel);

#endif
