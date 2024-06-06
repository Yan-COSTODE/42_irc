#include <climits>
#include <cstdlib>
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cerr << "error: wrong number of arguments\r" << endl;
		return EXIT_FAILURE;
	}

	int _port = atoi(argv[1]);

	if (_port < 1024 || _port > USHRT_MAX)
	{
		cerr << "error: wrong port\r" << endl;
		return EXIT_FAILURE;
	}

	Server _server(_port, argv[2]);

	try
	{
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		_server.ServerInit();
	}
	catch (const exception& e)
	{
			_server.CloseFds();
			cerr << "" << e.what() << "\r" << endl;
	}

	return EXIT_SUCCESS;
}