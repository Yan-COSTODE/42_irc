#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>

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
		vector<Client> admin;
		vector<Client> user;

	public:
		Channel(string _name);
		void JoinChannel(Client _client);
		void SetAdminStatus(Client _client, bool _status);
};

#endif
