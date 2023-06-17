#pragma once
#include <string>
#include "Controls.h"

using namespace std;


class PreConnection
{
private:
	bool connecting;
	bool alreadyConnected;
	string ipAddress;
	string name;
	int port;
	HWND mainWindowHandle;
public:
	PreConnection(HWND& mainWindowHandle);
	bool GetConnectionStatus() { return this->connecting; }
	bool IsConnected() { return this->alreadyConnected; }
	string GetIPAddress() { return this->ipAddress; }
	string GetName() { return this->name; }
	int GetPort() { return this->port; }
	void SetConnectingStatus(bool status) { this->connecting = status; }
	void SetConnected(bool connected) { this->alreadyConnected = connected; }
	void SetIPAddress(string ipAddress) { this->ipAddress = ipAddress; }
	void SetName(string name) { this->name = name; }
	void SetPort(int port) { this->port = port; }
	void HandleIPAddress();
	void HandleName();
	void HandlePort();
};

