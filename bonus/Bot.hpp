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
		void Loop();
		void Connect();
		string Receive();
		void Send(string _msg);
		string GetMeteo(string _msg);
		string CleanLocation(string _data);
		string ParseWeatherData(string _json);
		string GetWeatherData(string _location);

	public:
		Bot(int _port, string _address, string _password);
		void Init();
};

#endif
