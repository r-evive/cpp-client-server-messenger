#include "pch.h"
#include "ServerConfig.h"

//Konstruktor domyœlny.
ServerConfig::ServerConfig()
{
	this->configFilePath = "config.ini";
	this->defaultIPAddress = "127.0.0.1";
	this->defaultPort = 6000;
}

//Utworzenie nowego pliku konfiguracyjnego i ustawienie w nim domyœlnych wartoœci adres IP i portu.
void ServerConfig::CreateConfigFile()
{
	ofstream file(this->configFilePath, ios::out | ios::trunc);

	try {
		this->ipAddress = this->defaultIPAddress;
		this->port = this->defaultPort;
		file << this->defaultIPAddress + "\n";
		file << this->defaultPort;
		file.close();
	}
	catch (exception e) {
		this->ipAddress = this->defaultIPAddress;
		this->port = this->defaultPort;
		file.close();
	}

}

//Funkcja ta odczytuje adres IP i port z pliku konfiguracyjnego. Je¿eli odczytany format jest niepoprawny, do inicjalizacja socketu
//jest u¿ywany domyœlny adres i port, a plik konfiguracyjny jest na nowo tworzony.
void ServerConfig::ReadDataFromConfigFile()
{
	ifstream file(this->configFilePath);

	if (file.good()) {
		try {
			string portString;
			getline(file, this->ipAddress);
			getline(file, portString);
			this->port = stoi(portString);
			file.close();
		}
		catch (exception e) {
			file.close();
			this->ipAddress = this->defaultIPAddress;
			this->port = this->defaultPort;
			CreateConfigFile();
		}
	}
	else {
		file.close();
		CreateConfigFile();
	}
}
