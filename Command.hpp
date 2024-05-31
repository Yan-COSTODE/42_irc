#pragma once
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"

class Command {
	public:
		static void Parse(string _data, Client* _client, Server* _server);
};

#endif
