#pragma once
#include <vector>
#include <thread>
#include "IPConfiguration.h"
#include "Client.h"
#pragma comment(lib,"WS2_32")
#include "Controls.h"
#include "Logs.h"
#include "ServerConfig.h"
#include "ClientsData.h"
using namespace std;

enum Status { IDLE, CREATING, BINDING, LISTENING, THROW_ERROR };

class Socket {
private:
	SOCKET mainSocket;
	IPConfiguration* ipConf;
	int backlogSize = 5;
	Status currentStatus = IDLE;
	HWND mainWindowHandle;
	vector<Client*>* clients;
	Logs* logs;
	ServerConfig * serverConfig;
	ClientsData* clientsData;
	void InitThreads();
	void InitWinsock();
public:
	Socket(HWND & mainWindowHandle);
	void CreateSocket();
	void BindSocket();
	void StartListening();
	void AcceptNewConnection();
	void StartAcceptingLoop();
	void DisconnectAll();
	void Close();
	void SendToAll(string message);
	void ShowTextMessage(const char* text);
	void GetDataAboutClient();
	void DisconnectSelectedClient();
	void HandleServerMessage();
	int CountConnectedClients();
	string PrepareServerMessage(string message);
	~Socket();
};

