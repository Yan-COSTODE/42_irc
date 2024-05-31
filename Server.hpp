#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <cstring>
#include "Client.hpp"

using namespace std;

class Server {
	private:
		int port;
		int serverSocket;
		static bool Signal;
		vector<Client> clients;
		vector<struct pollfd> fds;

	public:
		Server(int _port);
		void ServerInit();
		void ServerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int _fd);
		void CloseFds();
		void ClearClients(int _fd);

		static void SignalHandler(int _signum);
};

#endif
