#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include <algorithm>

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

	private:
		bool CheckAdmin(Client _client);
		bool CheckUser(Client _client);
		void RemoveFrom(vector<Client>* _vector, Client _client);

public:
		Channel();
		Channel(string _name);
		void JoinChannel(Client _client);
		void QuitChannel(Client _client);
		void ToggleAdmin(Client _admin, Client _client);
		void Broadcast(string _msg);
		string Name();
		string Who();
		int Users();
};

#endif
