#include "pch.h"
#include "IPConfiguration.h"

//Konstruktor
IPConfiguration::IPConfiguration()
{
	this->socketAddress.sin_family = AF_INET;
	this->ipAddress[0] = '\0';
	this->port = 0;
}

//Konstruktor
IPConfiguration::IPConfiguration(string ipAddress, int port) {
	this->port = port;
	this->socketAddress.sin_family = AF_INET;
	std::wstring widestr = std::wstring(ipAddress.begin(), ipAddress.end());
	InetPton(AF_INET, widestr.c_str(), &this->socketAddress.sin_addr);
	inet_ntop(AF_INET, &this->socketAddress.sin_addr, this->ipAddress, INET_ADDRSTRLEN);
	this->socketAddress.sin_port = htons(port);
}

//Funkcja ta ustawia wartoœci pó³ ipAddres oraz port (string i integer) w celu ³atwego odczytu.
void IPConfiguration::SetAddressData() {
	inet_ntop(AF_INET, &this->socketAddress.sin_addr, this->ipAddress, INET_ADDRSTRLEN);
	this->port = htons(this->socketAddress.sin_port);
}
