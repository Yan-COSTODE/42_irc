#pragma once
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"

class Command {
	public:
		static void Parse(string _data, Client* _client, Server* _server);
		static void NotFound(string _data, Client* _client);
		static void Who(string _data, Client* _client, Server* _server);
		static void Join(string _data, Client* _client, Server* _server);
		static void Part(string _data, Client* _client, Server* _server);
		static string ExtractCommand(string _data);
		static string ExtractArgs(string _data);
};

#endif
