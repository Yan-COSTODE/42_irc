#include "Server.hpp"

Server::Server(unsigned short _port, const string& _password)
{
	cout << "\x1b[1;32mServer Created\x1b[0m" << endl;
	port = _port;
	password = _password;
	serverSocket = -1;
	status = true;
	Start();
}

Server::~Server()
{
	cout << "\x1b[1;31mServer Closed\x1b[0m" << endl;
}

void Server::Start()
{
	pollfd _pollFd = {serverSocket, POLLIN, 0};
	pollFds.push_back(_pollFd);
	cout << "\x1b[1;32mServer Started\x1b[0m" << endl;

	while (status)
	{
		if (poll(pollFds.begin().base(), pollFds.size(), -1) < 0)
			ThrowError("Error: polling from fd");

		for (vector<pollfd>::iterator _iterator = pollFds.begin(); _iterator != pollFds.end(); _iterator++)
		{
			cout << "Poll called" <<endl;

			if (_iterator->revents == 0)
				continue;

			if ((_iterator->revents & POLLHUP) == POLLHUP)
			{
				DisconnectClient(_iterator->fd);
				break;
			}

			if ((_iterator->revents & POLLIN) == POLLIN)
			{
				if (_iterator->fd == serverSocket)
				{
					ConnectClient();
					break;
				}

				MessageClient(_iterator->fd);
			}
		}
	}
}

void Server::DisconnectClient(int _fd)
{
	(void)_fd;
}

void Server::ConnectClient()
{
	int _fd;
	sockaddr_in _addr;
	socklen_t _size = sizeof(_addr);

	_fd = accept(serverSocket, (sockaddr *)&_addr, &_size);

	if (_fd < 0)
		ThrowError("Error: Accepting Client");

	pollfd _pollFd = {_fd, POLLIN, 0};
	pollFds.push_back(_pollFd);
	char _hostname[NI_MAXHOST];
	int _res = getnameinfo((sockaddr *)&_addr, sizeof(_addr), _hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);

	if (_res != 0)
		ThrowError("Error: Hostname Client");

	Client* _client = new Client(_fd, ntohs(_addr.sin_port),_hostname);
	clients.insert(make_pair(_fd, _client));
	char _msg[1000];
	sprintf(_msg, "\x1b[1;37m%s:%d has connected.\x1b[0m", _client->GetHostname().c_str(), _client->GetPort());
	cout << "\x1b[1;37m" << _msg << "\x1b[0m" << endl;
}

void Server::MessageClient(int _fd)
{
	try
	{
		cout << ReadMessage(_fd);
	}
	catch (const exception& _e)
	{
		cout << "\x1b[1;31mError: Message client: " << _e.what() << "\x1b[0m" << endl;
	}
}

string Server::ReadMessage(int _fd)
{
	string _msg;
	char _buffer[BUFFER_SIZE];

	do
	{
		bzero(_buffer, BUFFER_SIZE);

		if ((recv(_fd, _buffer, 100, 0) < 0) and (errno != EWOULDBLOCK))
			ThrowError("Error: Reading Buffer");

		_msg.append(_buffer);
	} while (!strstr(_buffer, "\n"));

	return _msg;
}

void Server::ThrowError(const string& msg)
{
	string _final = "\x1b[1;31m" + msg + "\x1b[0m";
	throw runtime_error(_final);
}