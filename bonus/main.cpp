#include <climits>
#include <cstdlib>
#include "Bot.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		cerr << "error: wrong number of arguments\r" << endl;
		return EXIT_FAILURE;
	}

	int _port = atoi(argv[2]);

	if (_port < 1024 || _port > USHRT_MAX)
	{
		cerr << "error: wrong port\r" << endl;
		return EXIT_FAILURE;
	}

	Bot _bot(_port, argv[1], argv[3]);

	try {
		_bot.Init();
	}
	catch (const exception& e)
	{
		cerr << "" << e.what() << "\r" << endl;
	}


	return EXIT_SUCCESS;
}