#pragma once
#include "ClientINFO.h"

class Messages
{
private:
	ClientINFO* clientINFO;
public:
	Messages(ClientINFO* clientINFO);
	string FindOrderType(string message);
	string GetDisconnectMessage();
	void GetDataAndContent(string message, string & name, int & id, string & content);
};

