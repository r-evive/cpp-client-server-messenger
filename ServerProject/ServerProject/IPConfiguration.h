#pragma once
#include <string>
#include <WinSock2.h>
#include <sys/types.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <string>

using namespace std;

class IPConfiguration {
private:
	struct sockaddr_in socketAddress;
	socklen_t socketLength = sizeof(this->socketAddress);
	char ipAddress[INET_ADDRSTRLEN];
	int port;
public:
	IPConfiguration();
	IPConfiguration(string ipAddress, int port);
	sockaddr_in& GetSocketAddress() { return this->socketAddress; }
	sockaddr* GetSOCKADDR() { return (sockaddr*)&this->socketAddress; }
	int GetSocketSize() { return sizeof(this->socketAddress); }
	socklen_t* GetSocketLenSize() { return (socklen_t*)&this->socketLength; }
	string GetIPAddress() { return this->ipAddress; }
	int GetPort() { return this->port; }
	void SetAddressData();

};