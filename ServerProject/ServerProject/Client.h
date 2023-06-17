#pragma once
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include "IPConfiguration.h"
#include "ClientINFO.h"
#include "Controls.h"
#include "Messages.h"
#include "Logs.h";
using namespace std;


class Client {
private:
	SOCKET * mainSocket;
	SOCKET clientSocket;
	IPConfiguration* ipConf;
	HWND mainWindowHandle;
	ClientINFO* clientINFO;
	vector<Client*>* clients;
	Messages* messages;
	Logs* logs;
	bool threadActive = true;

public:
	Client();
	Client(SOCKET * mainSocket, SOCKET& clientSocket, HWND & mainWindowHandle);
	IPConfiguration* GetIPConfiguration() { return this->ipConf; }
	ClientINFO* GetClientINFO() { return this->clientINFO; }
	Messages* GetMessagesHandle() { return this->messages; }
	void Start();
	void Listen();
	void InitializeClientSocket(SOCKET* mainSocket, SOCKET& clientSocket, HWND & mainWindowHandle);
	void Disconnect();
	void SendMessageToClient(string message);
	void SetClientsVector(vector<Client*>* clients);
	void ExecuteCommand(string originalMessage, string orderType, string command);
	void ShowTextMessage(const char* text);
	void UpdateConnectedClients();
	void SendToAll(string message);
	void GetDataAboutClient();
	void AddClientToListbox();
	void EraseClient();
	void RemoveClientFromListbox();
	~Client();
};