#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

using namespace std;

class Client {
	private:
		int fd;
		int port;
		string hostname;

	private:


	public:
		~Client();
		Client(int _fd, int _port, const string &_hostname);
		int GetPort();
		string GetHostname();
};

#endif
