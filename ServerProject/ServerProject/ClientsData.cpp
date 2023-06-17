#include "pch.h"
#include "ClientsData.h"

ClientsData::ClientsData(vector<Client*>* clients, HWND & mainWindowHandle)
{
	this->clients = clients;
	this->mainWindowHandle = mainWindowHandle;
}

//Funkcja zwraca numer ID wybranego z ListBoxu klienta.
int ClientsData::GetSelectedClientID()
{
	DWORD currentSelect = SendMessage(GetDlgItem(this->mainWindowHandle, ID_CLIENTS_LISTBOX), LB_GETCURSEL, 0, 0);
	wchar_t clientData[256] = { 0 };
	SendMessage(GetDlgItem(this->mainWindowHandle, ID_CLIENTS_LISTBOX), LB_GETTEXT, (WPARAM)(int)(currentSelect), (LPARAM)(LPCTSTR)(clientData));
	wchar_t charID;
	wstring convertedData(clientData);
	convertedData = convertedData.substr(convertedData.find_first_of(L"[") + 1, convertedData.find_first_of(L"]") - convertedData.find_first_of(L"[") - 1);
	int clientId = stoi(convertedData);
	return clientId;
}

//Funkcja przeszukuje pod³¹czonych klientów i zwraca wskaŸnik do klienta o takim samym ID.
Client* ClientsData::FindClientByID(int id)
{
	vector<Client*>::iterator iter;
	for (iter = clients->begin(); iter != clients->end(); ++iter)
		if ((*iter)->GetClientINFO()->GetID() == id)
			return *iter;
	return nullptr;
}

//Od³¹czenie wybranego klienta od serwera.
void ClientsData::DisconnectSelectedClient()
{
	int clientId = GetSelectedClientID();
	Client* foundClient = FindClientByID(clientId);
	foundClient->Disconnect();
}

//Wyœwietla informacjê na temat klienta w bocznym pasku, poni¿ej ListBoxa.
void ClientsData::GetDataAboutClient()
{
	Client* selectedClient = FindClientByID(GetSelectedClientID());
	selectedClient->GetDataAboutClient();
}

//Dekonstruktor.
ClientsData::~ClientsData()
{
	delete clients;
}

