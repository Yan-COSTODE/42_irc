#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

class Server {
	private:
		unsigned short port;
		std::string password;
		int serverSocket;
		bool status;
		sockaddr_in serverAdrress;

	private:
		void Start();

	public:
		~Server();
		Server(unsigned short _port, const std::string& _password);
		static int PrintError(const std::string& msg);
};

#endif
