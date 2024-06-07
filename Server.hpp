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
#include "define.hpp"

using namespace std;

class Server {
	private:
		int port;
		int serverSocket;
		string password;
		vector<Client> bots;
		vector<Client> clients;
		vector<Channel> toRemoved;
		vector<struct pollfd> fds;
		map<int, string> buffer;
		map<string, Channel> channels;

		static bool Signal;

	private:
		Client* GetClient(int _fd);

	public:
		Server(int _port, string passwd);
		void ServerInit();
		void CloseFds();
		string GetPass();
		void ServerSocket();
		void RemoveChannel();
		void AcceptNewClient();
		void ClearClients(int _fd);
		void AddBot(Client _client);
		void Broadcast(string _msg);
		string List(Client _client);
		void ReceiveNewData(int _fd);
		Client* GetBot(string _name);
		Client* GetClient(string _name);
		Channel* GetChannel(string _name);
		Channel* AddChannel(string _name);
		bool IsNameAvailable(string _name);
		void TreatBuffer(int _fd, string _read);
		void AddToRemoveChannel(Channel _channel);
		void UpdateNick(string _old, string _new);
		void RemoveBot(Client _client, bool _sendMsg);
		void RemoveClient(Client _client, string _reason);

		static void SignalHandler(int _signum);
};

#endif
