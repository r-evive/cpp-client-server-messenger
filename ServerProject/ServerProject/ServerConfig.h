#pragma once
#include <string>
#include <fstream>
using namespace std;



class ServerConfig
{
private:
	string ipAddress;
	int port;
	string configFilePath;
	string defaultIPAddress;
	int defaultPort;
public:
	ServerConfig();
	string GetIPAddress() { return this->ipAddress; }
	int GetPort() { return this->port; }
	void CreateConfigFile();
	void ReadDataFromConfigFile();
};

