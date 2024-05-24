#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>

class Server {
	private:
		unsigned short port;
		std::string password;

	public:
		~Server();
		Server(unsigned short _port, std::string _password);
};

#endif
