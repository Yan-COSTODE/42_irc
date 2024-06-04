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
		bool operator==(const Client& _other) const;
		string GetNick() const;
		Client(int _fd, string _ipAddress);
		int GetFd() const;
		string GetUser();
		bool GetBot() const;
		bool GetAuthentified() const;
		void Authentify();
		void SetUser(string _user);
		void SetNick(string _nick);
		void Broadcast(string _msg);
		void SetBot(bool _status);
};

ostream& operator<<(ostream& _os, const Client& _client);

#endif
