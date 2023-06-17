#pragma once
#include "ClientINFO.h"

class Messages
{
private:
	ClientINFO * clientINFO;
public:
	Messages(ClientINFO * clientINFO);
	string MakeNewClientMessage(int id, string name);
	wchar_t* ConvertTextMessage(const char* text);
	string GetDisconnectMessage();
	string GetDisconnectInfo();
	void SetInitData(string command);
	string TranslateMessage(string message);
	string FindOrderType(string command);
	string CutMessage(string command);
};

