#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <cstring>
#include <cerrno>
#include <netdb.h>
#include <cstdio>
#include "Client.hpp"

#define BUFFER_SIZE 100

using namespace std;

class Server {
	private:
		unsigned short port;
		string password;
		int serverSocket;
		bool status;
		vector<pollfd> pollFds;
		//vector<Channel*> channels;
		map<int, Client*> clients;

	private:
		void Start();
		void ConnectClient();
		void DisconnectClient(int _fd);
		void MessageClient(int _fd);
		string ReadMessage(int _fd);

	public:
		~Server();
		Server(unsigned short _port, const string& _password);
		static void ThrowError(const string& msg);
};

#endif
