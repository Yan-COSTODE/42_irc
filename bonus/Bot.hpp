#pragma once
#ifndef BOT_HPP
#define BOT_HPP

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sstream>

using namespace std;

class Bot {
	private:
		int fd;
		int port;
		string address;
		string password;
		string pattern;

	private:
		void Connect();
		void Loop();
		void Send(string _msg);
		string GetMeteo(string _msg);
		string GetWeatherData(string _location);
		string ParseWeatherData(string _json);
		string Receive();
		string CleanLocation(string _data);

	public:
		Bot(int _port, string _address, string _password);
		void Init();
};

#endif
