#pragma once
#include <string>
#include "IPConfiguration.h"
#include "Client.h"
#include <thread>
#include "PreConnection.h"
#include "Controls.h"
#pragma comment(lib,"WS2_32")


using namespace std;


enum Status { IDLE, CREATING, BINDING, LISTENING, THROW_ERROR };

class Socket {
private:
	SOCKET mainSocket;
	IPConfiguration* ipConf;
	Status currentStatus = IDLE;
	Client* currentClient;
	HWND mainWindowHandle;
	PreConnection * preConnection;
	void InitWinsock();
public:
	Socket(HWND& mainWindowHandle);
	void CreateSocket();
	void BindServerAddress(string ipAddress, int port);
	void Connect();
	void Disconnect();
	PreConnection * GetPreConnection() { return this->preConnection; }
	string PrepareTextMessage(string message);
	void SendTextMessage(string message);
	void ShowTextMessage(const char* text);
	void HandleSendMessage();
	~Socket();
};

