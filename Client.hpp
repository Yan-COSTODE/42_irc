#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Channel.hpp"

using namespace std;

class Client {
	private:
		int fd;
		string ipAddress;
		string nick;
		string user;
		bool connected;

	public:
		Client(int _fd, string _ipAddress);
		int GetFd();
};

#endif
