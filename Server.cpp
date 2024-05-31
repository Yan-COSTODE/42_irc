#include "Server.hpp"
#include "Command.hpp"

bool Server::Signal = false;

Server::Server(int _port)
{
	port = _port;
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
	cout << "\x1b[1;32mClient <" << _clientFd << "> Connected\x1b[0m" << endl;
}

void Server::ReceiveNewData(int _fd)
{
	char _buffer[1024];
	memset(_buffer, 0, sizeof(_buffer));
	ssize_t _bytes = recv(_fd, _buffer, sizeof(_buffer) - 1, 0);

	if (_bytes <= 0)
	{
		cout << "\x1b[1;31mClient <" << _fd << "> Disconnected\x1b[0m" << endl;
		ClearClients(_fd);
		close(_fd);
	}
	else
	{
		_buffer[_bytes] = '\0';
		Command::Parse(_buffer, NULL, this);
	}
}

void Server::CloseFds()
{
	for(size_t i = 0; i < clients.size(); i++)
	{
		cout << "\x1b[1;31mClient <" << clients[i].GetFd() << "> Disconnected\x1b[0m" << endl;
		close(clients[i].GetFd());
	}

	if (serverSocket != -1)
	{
		cout <<"\x1b[1;31mServer <" << serverSocket << "> Disconnected\x1b[0m" << endl;
		close(serverSocket);
	}
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
}

void Server::SignalHandler(int _signum)
{
	(void)_signum;
	cout << endl << "\x1b[1;37mSignal Received !\x1b[0m" << endl;
	Server::Signal = true;
}