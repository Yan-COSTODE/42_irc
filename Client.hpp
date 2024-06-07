#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <string>
#include <sstream>
#include "Channel.hpp"

using namespace std;

class Client {
	private:
		int fd;
		string ipAddress;
		string nick;
		string user;
		bool authentified;
		bool bot;

	public:
		Client(int _fd, string _ipAddress);
		string GetUser();
		int GetFd() const;
		void Authentify();
		bool GetBot() const;
		string GetNick() const;
		void SetBot(bool _status);
		void SetUser(string _user);
		void SetNick(string _nick);
		void Broadcast(string _msg);
		bool GetAuthentified() const;
		bool operator==(const Client& _other) const;
};

ostream& operator<<(ostream& _os, const Client& _client);

#endif
