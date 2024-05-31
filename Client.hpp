#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

using namespace std;

class Client {
	private:
		int fd;
		string ipAddress;

	public:
		Client(int _fd, string _ipAddress);
		int GetFd();
};

#endif
