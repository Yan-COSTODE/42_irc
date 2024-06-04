#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
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
		string password;
		vector<Client> clients;
		vector<struct pollfd> fds;
		map<string, Channel> channels;
		vector<Channel> toRemoved;

	private:
		Client* GetClient(int _fd);

	public:
		Server(int _port, string passwd);
		void ServerInit();
		void ServerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int _fd);
		void CloseFds();
		void RemoveClient(Client _client, bool _sendMsg);
		void ClearClients(int _fd);
		Channel* GetChannel(string _name);
		Channel* AddChannel(string _name);
		void RemoveChannel();
		void AddToRemoveChannel(Channel _channel);
		Client* GetClient(string _name);
		string GetPass();
		bool IsNameAvailable(string _name);

		static void SignalHandler(int _signum);
};

#endif
