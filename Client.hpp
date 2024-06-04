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

	public:
		Client(int _fd, string _ipAddress);
		bool operator==(const Client& _other) const;
		string Nickname() const;
		int GetFd() const;
		void Broadcast(string _msg);
};

ostream& operator<<(ostream& _os, const Client& _client);

#endif
