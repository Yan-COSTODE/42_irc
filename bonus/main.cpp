#include <climits>
#include <cstdlib>
#include "Bot.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		cerr << "\x1b[1;31merror: wrong number of arguments\x1b[0m" << endl;
		return EXIT_FAILURE;
	}

	int _port = atoi(argv[2]);

	if (_port < 1024 || _port > USHRT_MAX)
	{
		cerr << "\x1b[1;31merror: wrong port\x1b[0m" << endl;
		return EXIT_FAILURE;
	}

	Bot _bot(_port, argv[1], argv[3]);

	try {
		_bot.Init();
	}
	catch (const exception& e)
	{
		cerr << "\x1b[1;31m" << e.what() << "\x1b[0m" << endl;
	}


	return EXIT_SUCCESS;
}