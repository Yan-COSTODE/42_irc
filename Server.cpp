#include "Server.hpp"
#include "Command.hpp"

bool Server::Signal = false;

Server::Server(int _port, string passwd)
{
	port = _port;
	password = passwd;
	serverSocket = -1;
}

void Server::ServerInit()
{
	ServerSocket();

	cout << "\x1b[1;32mServer <" << serverSocket << "> Connected\x1b[0m" << endl;

	while (!Server::Signal)
	{
		if ((poll(&fds[0], fds.size(), -1) == -1) && !Server::Signal)
			throw (runtime_error("\x1b[1;31merror: poll() failed\x1b[0m"));

		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == serverSocket)
					AcceptNewClient();
				else
					ReceiveNewData(fds[i].fd);
			}
		}
	}

	CloseFds();
}

void Server::ServerSocket()
{
	struct sockaddr_in _address;
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = INADDR_ANY;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocket == -1)
		throw (runtime_error("\x1b[1;31merror: failed to create socket\x1b[0m"));

	int _en = 1;

	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &_en, sizeof(_en)) == -1)
		throw(runtime_error("\x1b[1;31merror: failed to set option (SO_REUSEADDR) on socket\x1b[0m"));

	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw(runtime_error("\x1b[1;31merror: failed to set option (O_NONBLOCK) on socket\x1b[0m"));

	if (bind(serverSocket, (struct sockaddr*)&_address, sizeof(_address)) == -1)
		throw(runtime_error("\x1b[1;31merror: failed to bind socket\x1b[0m"));

	if (listen(serverSocket, SOMAXCONN) == -1)
		throw(runtime_error("\x1b[1;31merror: listen() failed\x1b[0m"));

	struct pollfd _poll;
	_poll.fd = serverSocket;
	_poll.events = POLLIN;
	_poll.revents = 0;
	fds.push_back(_poll);
}

void Server::AcceptNewClient()
{
	struct sockaddr_in _address;
	socklen_t _len = sizeof(_address);

	int _clientFd = accept(serverSocket, (struct sockaddr*)&_address, &_len);

	if (_clientFd == -1)
	{
		cout << "\x1b[1;31maccept() failed" << endl;
		return;
	}

	if (fcntl(_clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		cout << "\x1b[1;31mfcntl() failed" << endl;
		return;
	}

	struct pollfd _poll;
	_poll.fd = _clientFd;
	_poll.events = POLLIN;
	_poll.revents = 0;
	Client _client(_clientFd, inet_ntoa(_address.sin_addr));
	clients.push_back(_client);
	fds.push_back(_poll);
	cout << "\x1b[1;32m" << _client << " Connected\x1b[0m" << endl;
	_client.Broadcast("\x1b[1;32mYou are connected to the server, you need to authentify yourself\n\x1b[0m");
}

void Server::ReceiveNewData(int _fd)
{
	char _buffer[1024];
	memset(_buffer, 0, sizeof(_buffer));
	ssize_t _bytes = recv(_fd, _buffer, sizeof(_buffer) - 1, 0);

	if (_bytes <= 0)
	{
		Client* _client = GetClient(_fd);

		if (_client)
		{
			if (_client->GetBot())
				RemoveBot(*_client, false);
			else
				RemoveClient(*_client, false);
		}
	}
	else
	{
		_buffer[_bytes] = '\0';
		Command::Parse(_buffer, GetClient(_fd), this);
	}
}

void Server::CloseFds()
{
	for(size_t i = 0; i < clients.size(); i++)
		RemoveClient(clients[i], true);

	for(size_t i = 0; i < bots.size(); i++)
		RemoveBot(bots[i], true);

	if (serverSocket != -1)
	{
		cout <<"\x1b[1;31mServer <" << serverSocket << "> Disconnected\x1b[0m" << endl;
		close(serverSocket);
	}
}

void Server::AddBot(Client _client)
{
	_client.SetBot(true);

	for(size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].GetFd() == _client.GetFd())
		{
			clients.erase(clients.begin() + i);
			break;
		}
	}

	bots.push_back(_client);
}

Client* Server::GetBot(std::string _name)
{
	for(size_t i = 0; i < bots.size(); i++)
	{
		if (bots[i].GetNick() == _name)
			return &bots[i];
	}

	return NULL;
}

void Server::AddToRemoveChannel(Channel _channel)
{
	toRemoved.push_back(_channel);
}

void Server::RemoveClient(Client _client, bool _sendMsg)
{
	cout << "\x1b[1;31m" << _client << " Disconnected\x1b[0m" << endl;

	for (map<string, Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		Channel& _channel = it->second;

		if (_channel.CheckUser(_client))
		{
			_channel.QuitChannel(_client, false);

			if (_channel.Users() == 0)
				AddToRemoveChannel(_channel);
		}
	}

	RemoveChannel();

	if (_sendMsg)
	{
		string _msg = "\x1b[1;31mYou have been disconnected from the server\n\x1b[0m";
		_client.Broadcast(_msg);
	}

	ClearClients(_client.GetFd());
	close(_client.GetFd());
}

void Server::RemoveBot(Client _client, bool _sendMsg)
{
	cout << "\x1b[1;31m" << _client << " Disconnected\x1b[0m" << endl;

	if (_sendMsg)
	{
		string _msg = "\x1b[1;31mYou have been disconnected from the server\n\x1b[0m";
		_client.Broadcast(_msg);
	}

	ClearClients(_client.GetFd());
	close(_client.GetFd());
}

void Server::ClearClients(int _fd)
{
	for(size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == _fd)
		{
			fds.erase(fds.begin() + i);
			break;
		}
	}

	for(size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].GetFd() == _fd)
		{
			clients.erase(clients.begin() + i);
			break;
		}
	}

	for(size_t i = 0; i < bots.size(); i++)
	{
		if (bots[i].GetFd() == _fd)
		{
			bots.erase(bots.begin() + i);
			break;
		}
	}
}

string Server::GetPass()
{
	return password;
}

Client* Server::GetClient(int _fd)
{
	for(size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].GetFd() == _fd)
			return &clients[i];
	}

	for(size_t i = 0; i < bots.size(); i++)
	{
		if (bots[i].GetFd() == _fd)
			return &bots[i];
	}

	return NULL;
}

Client* Server::GetClient(string _name)
{
	for(size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].GetNick() == _name)
			return &clients[i];
	}

	return NULL;
}

Channel *Server::GetChannel(string _name)
{
	if (channels.find(_name) != channels.end())
		return &channels[_name];
	return NULL;
}

Channel* Server::AddChannel(string _name)
{
	if (channels.find(_name) != channels.end())
		return &channels[_name];

	Channel _channel(_name);
	cout << "\x1b[1;32m" << _channel << " Created\x1b[0m" << endl;
	channels.insert(make_pair(_name, _channel));
	return &channels[_name];
}

void Server::RemoveChannel()
{
	for (size_t i = 0; i < toRemoved.size(); ++i)
	{
		Channel _channel = toRemoved[i];
		cout << "\x1b[1;32m" << _channel << " Erased\x1b[0m" << endl;
		channels.erase(_channel.Name());
	}

	toRemoved.clear();
}

bool Server::IsNameAvailable(string _name)
{
	for (int i = 0; i < (int)clients.size(); i++)
	{
		if (_name == clients[i].GetNick())
			return false;
	}
	return true;
}

void Server::SignalHandler(int _signum)
{
	(void)_signum;
	cout << endl << "\x1b[1;37mSignal Received !\x1b[0m" << endl;
	Server::Signal = true;
}