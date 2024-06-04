#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <string>
#include "Channel.hpp"

using namespace std;

class Client {
	private:
		int fd;
		string ipAddress;
		string nick;
		string user;
		bool authentified;

	public:
		bool operator==(const Client& _other) const;
		string GetNick();
		Client(int _fd, string _ipAddress);
		int GetFd();
		string GetUser();
		void Authentify();
		void SetUser(string _user);
		void SetNick(string _nick);
		void Broadcast(string _msg);
};

#endif
