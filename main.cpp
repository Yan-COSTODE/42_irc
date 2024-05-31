#include <climits>
#include <cstdlib>
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cerr << "\x1b[1;31merror: wrong number of arguments\x1b[0m" << endl;
		return EXIT_FAILURE;
	}

	int _port = atoi(argv[1]);

	if (_port < 1024 || _port > USHRT_MAX)
	{
		cerr << "\x1b[1;31merror: wrong port\x1b[0m" << endl;
		return EXIT_FAILURE;
	}

	Server _server(_port);

	try
	{
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		_server.ServerInit();
	}
	catch (const exception& e)
	{
			_server.CloseFds();
			cerr << "\x1b[1;31m" << e.what() << "\x1b[0m" << endl;
	}

	return EXIT_SUCCESS;
}