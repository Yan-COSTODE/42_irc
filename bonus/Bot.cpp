#include "Bot.hpp"

Bot::Bot(int _port, string _address, string _password)
{
	port = _port;
	address = _address;
	password = _password;
	pattern = "<meteo_data>";
}

void Bot::Init()
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in _address;
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = INADDR_ANY;

	if (connect(fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
		throw(runtime_error("error: failed to connect to server\r"));

	Connect();
	Loop();
	close(fd);
}

void Bot::Loop()
{
	while (true)
	{
		string _msg = Receive();
		size_t _pos = _msg.find(pattern);

		if (_pos != string::npos)
		{
			string _user = _msg.substr(0, _pos);
			string _meteo = GetMeteo(_msg.substr(_pos));
			string _final = "PRIVMSG " + _user + " " + _meteo;
			Send(_final);
		}

		usleep(100000);
	}
}

void Bot::Connect()
{
	Send("PASS " + password + "\n");
	Send("USER <bot> 0 * :<bot>\n");
	Send("NICK Meteosaurus\n");
	usleep(100000);
	Receive();
}

string Bot::GetWeatherData(string _location)
{
	for (int i = 0; i < (int)_location.size(); ++i) {
		if (isspace(_location[i]))
			_location.replace(i, 1, "%20");
	}
	
	string _host = "api.openweathermap.org";
	string _apiKey = "a879f58f7219cf3b00b8781546f35596";
	string _path = "/data/2.5/weather?q=" + _location + "&appid=" + _apiKey + "&units=metric";

	struct addrinfo _hints, *_result;
	int _fd;
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_UNSPEC;
	_hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(_host.c_str(), "80", &_hints, &_result) != 0)
		return "";

	_fd = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol);

	if (_fd == -1)
	{
		freeaddrinfo(_result);
		return "";
	}

	if (connect(_fd, _result->ai_addr, _result->ai_addrlen) == -1)
	{
		close(_fd);
		freeaddrinfo(_result);
		return "";
	}

	ostringstream _request;
	_request << "GET " << _path << " HTTP/1.1\r\n";
	_request << "Host: " << _host << "\r\n";
	_request << "Connection: close\r\n\r\n";
	string _requestStr = _request.str();
	send(_fd, _requestStr.c_str(), _requestStr.size(), 0);
	char _buffer[4096];
	string _response;
	ssize_t _bytesRead;

	while ((_bytesRead = recv(_fd, _buffer, sizeof(_buffer), 0)) > 0) {
		_response.append(_buffer, _bytesRead);
	}

	close(_fd);
	freeaddrinfo(_result);
	size_t _headerEnd = _response.find("\r\n\r\n");

	if (_headerEnd != string::npos)
		return _response.substr(_headerEnd + 4);
	else
		return "";
}

string Bot::ParseWeatherData(string _json)
{
	string _desc = "\"description\":\"";
	string _temp = "\"temp\":";
	size_t _weatherPos = _json.find(_desc);
	size_t _tempPos = _json.find(_temp);

	if (_weatherPos == string::npos || _tempPos == string::npos)
		return "";

	size_t _weatherStart = _weatherPos + _desc.length();
	size_t _weatherEnd = _json.find("\"", _weatherStart);
	string _weatherDescription = _json.substr(_weatherStart, _weatherEnd - _weatherStart);
	size_t _tempStart = _tempPos + _temp.length();
	size_t _tempEnd = _json.find(",", _tempStart);
	string _tempStr = _json.substr(_tempStart, _tempEnd - _tempStart);
	return _weatherDescription + ", " + _tempStr + "°C";
}

string Bot::CleanLocation(string _data)
{
	do
	{
		_data.erase(_data.size() - 2);
	} while (isspace(_data[_data.size() - 1]));

	string _final = _data;
	return _final;
}

string Bot::GetMeteo(string _msg)
{
	string _location = CleanLocation(_msg.substr(pattern.size()));
	cout << "\x1b[1;37mChecking weather data in " << _location << "\x1b[0m" << endl;
	string _json = GetWeatherData(_location);
	string _meteo = ParseWeatherData(_json);

	if (_meteo.empty())
		return "No data available for " + _location + "\n";

	return "The meteo in " + _location + " is " + _meteo + "\n";
}

void Bot::Send(string _msg)
{
	send(fd, _msg.c_str(), strlen(_msg.c_str()), 0);
}

string Bot::Receive()
{
	char _buffer[1024];
	memset(_buffer, 0, sizeof(_buffer));
	ssize_t _bytes = recv(fd, _buffer, sizeof(_buffer) - 1, 0);

	if (_bytes <= 0)
		throw(runtime_error("You have been disconnected from the server\n"));
	else
	{
		_buffer[_bytes] = '\0';
		return _buffer;
	}
}