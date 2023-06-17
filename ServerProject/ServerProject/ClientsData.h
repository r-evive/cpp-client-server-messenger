#pragma once
#include "Client.h"

class ClientsData
{
private:
	vector<Client*>* clients;
	HWND mainWindowHandle;
public:
	ClientsData(vector<Client*>* clients, HWND & mainWindowHandle);
	int GetSelectedClientID();
	Client* FindClientByID(int id);
	void DisconnectSelectedClient();
	void GetDataAboutClient();
	~ClientsData();
};

