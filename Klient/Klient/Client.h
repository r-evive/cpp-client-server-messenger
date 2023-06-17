#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include "IPConfiguration.h"
#include "ClientINFO.h"
#include "Messages.h"

using namespace std;

class Client {
private:
	SOCKET mainSocket;
	IPConfiguration* ipConf;
	Messages* messages;
	HWND mainWindowHandle;
	ClientINFO clientINFO;
	bool threadActive = true;
public:
	Client(SOCKET& mainSocket, IPConfiguration* ipConf, HWND & mainWindowHandle);
	IPConfiguration* GetIPConfiguration() { return this->ipConf; }
	ClientINFO& GetClientInfo() { return this->clientINFO; }
	void Start();
	void Listen();
	void Disconnect();
	void SendData(string message);
	void InitData();
	void TranslateMessage(string message);
	void ShowTextMessage(const char* text);
	~Client();
};
