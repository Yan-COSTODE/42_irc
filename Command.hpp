#pragma once
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"

class Command {
	public:
		static int IsFlag(string flag);
		static bool ParseUser(string _data);
		static string ExtractMsg(string _data);
		static string ExtractArgs(string _data);
		static string ExtractCommand(string _data);
		static int GetFirstArg(vector<string>cmds);
		static void Bot(string _data, Client* _client);
		static vector<string> SplitString(const string str);
		static void Msg(string _data, Client* _client, Server* _server);
		static void List(string _data, Client* _client, Server* _server);
		static void Join(string _data, Client* _client, Server* _server);
		static void Part(string _data, Client* _client, Server* _server);
		static void Quit(string _data, Client* _client, Server* _server);
		static void Pass(string _data, Client *_client, Server *_server);
		static void Nick(string _data, Client *_client, Server *_server);
		static void User(string _data, Client *_client, Server *_server);
		static void Kick(string _data, Client *_client, Server *_server);
		static void Mode(string _data, Client *_client, Server *_server);
		static void Ping(string _data, Client *_client, Server *_server);
		static void Help(string _data, Client *_client, Server *_server);
		static void Parse(string _data, Client* _client, Server* _server);
		static void Names(string _data, Client* _client, Server* _server);
		static void Topic(string _data, Client *_client, Server *_server);
		static void Meteo(string _data, Client *_client, Server *_server);
		static void Invite(string _data, Client *_client, Server *_server);
		static void ModeT(string flag, Channel *_channel, Client *_client);
		static void ModeI(string flag, Channel *_channel, Client *_client);
		static void ModeL(string flag, Channel *_channel, Client *_client, string arg);
		static void ModeK(string flag, Channel *_channel, Client *_client, string arg);
		static void ModeO(string flag, Channel *_channel, Client *_client, string arg, Server *_server);
};

#endif
