#include "pch.h"
#include "Client.h"

//Konstruktor domy�lny.
Client::Client() {
	this->clients = NULL;
	this->clientSocket = NULL;
	this->mainSocket = NULL;
	this->mainWindowHandle = NULL;
	this->ipConf = new IPConfiguration();
	this->clientINFO = new ClientINFO();
	this->messages = new Messages(this->clientINFO);
	this->logs = new Logs("logs.txt");
}

//Konstruktor parametrowy.
Client::Client(SOCKET * mainSocket, SOCKET& clientSocket, HWND & mainWindowHandle) {
	this->clients = NULL;
	this->mainSocket = mainSocket;
	this->mainWindowHandle = mainWindowHandle;
	this->clientSocket = clientSocket;
	this->ipConf = new IPConfiguration();
	this->clientINFO = new ClientINFO();
	this->messages = new Messages(this->clientINFO);
	this->logs = new Logs("logs.txt");
}

//Funkcja od��cza klienta od serwera. Wy�wietlana i wysy�ana jest informacja o od��czeniu klienta.
//Klient jest usuwany z ListBoxa oraz uruchomiony dla tego klienta w�tek jest zamykany.
void Client::Disconnect()
{
	this->threadActive = false;
	SendMessageToClient(this->messages->GetDisconnectMessage());
	string message = this->clientINFO->GetName() + " roz��czy� si�...\r\n";
	ShowTextMessage(message.c_str());
	EraseClient();
	RemoveClientFromListbox();
	UpdateConnectedClients();
	closesocket(this->clientSocket);
}

//Wysy�anie wiadomo�ci do klienta.
void Client::SendMessageToClient(string message)
{
	send(this->clientSocket, message.c_str(), message.size(), 0);
}

//Inicjalizacja klienta, ustawienie warto�ci p�l.
void Client::InitializeClientSocket(SOCKET* mainSocket, SOCKET& clientSocket, HWND & mainWindowHandle)
{
	this->clientSocket = clientSocket;
	this->mainWindowHandle = mainWindowHandle;
}

//Nas�uchiwanie i odbi�r danych od klienta.
void Client::Listen() {
	while (this->threadActive) {
		vector<char> dataReceived(10000);
		int bytes = recv(clientSocket, dataReceived.data(), dataReceived.size(), 0);
		if (bytes > 0) {
			string message = "";
			for (int i = 0; i < bytes; i++) {
				message += dataReceived[i];
			}
			ExecuteCommand(message, this->messages->FindOrderType(message), this->messages->CutMessage(message));
		}
		this_thread::sleep_for(50ms);
	}
}

//Rozpocz�nie nas�uchiwania.
void Client::Start() {
	Listen();
	delete this;
}
//Ustawienie wska�nika do zmiennej przechowuj�cej liste klient�w.
void Client::SetClientsVector(vector<Client*>* clients) {
	this->clients = clients;
}

//Dekonstruktor.
Client::~Client() {
	delete ipConf;
	delete clientINFO;
	delete logs;
}

//Usuwanie klienta z zmiennej przechowuj�cej list� klient�w.
void Client::EraseClient()
{
	try {
		this->clients->erase(remove(this->clients->begin(), this->clients->end(), this), this->clients->end());
	}
	catch (exception e) {

	}
}

//Ustawienie warto�ci p�l, znajduj�cych si� poni�ej ListBoxa, po klikni�ciu na wybranego klienta.
void Client::GetDataAboutClient()
{
	string nameLabel = "Nazwa: " + this->GetClientINFO()->GetName();
	SetWindowText(GetDlgItem(this->mainWindowHandle, ID_NAME_LABEL), this->GetMessagesHandle()->ConvertTextMessage(nameLabel.c_str()));

	string idLabel = "ID: " + to_string(this->GetClientINFO()->GetID());
	SetWindowText(GetDlgItem(this->mainWindowHandle, ID_ID_LABEL), this->GetMessagesHandle()->ConvertTextMessage(idLabel.c_str()));

	string dateLabel = "Pod��czony o: " + this->GetClientINFO()->GetConnectionDate();
	SetWindowText(GetDlgItem(this->mainWindowHandle, ID_DATE_LABEL), this->GetMessagesHandle()->ConvertTextMessage(dateLabel.c_str()));
}

//Destukturyzacja otrzymanej wiadomo�ci i wykonanie akcji, na podstawie jej rodzaju (orderType).
void Client::ExecuteCommand(string originalMessage, string orderType, string command) {
	if (orderType == "INIT") {
		this->messages->SetInitData(command);
		string message = "[" + to_string(this->clientINFO->GetID()) + "] " + this->clientINFO->GetName() + " do��czy� do sesji...\r\n";
		this->clientINFO->SetConnectionDate();
		ShowTextMessage(message.c_str());
		SendToAll(this->messages->MakeNewClientMessage(this->clientINFO->GetID(), this->clientINFO->GetName()));
		AddClientToListbox();
		UpdateConnectedClients();
	}
	else if (orderType == "MESSAGE") {
		SendToAll(originalMessage);
		ShowTextMessage(this->messages->TranslateMessage(originalMessage).c_str());
	}
	else if (orderType == "DISCONNECT") {
		SendToAll(this->messages->GetDisconnectInfo());
		if(threadActive)
			Disconnect();
	}
	else {
		ShowTextMessage("Otrzynano niezdefiniowan� wiadomo��...\r\n");
	}
}

//Wy�wietlenie wiadomo�ci tekstowej na ekranie.
void Client::ShowTextMessage(const char* text) {
	int len = strlen(text);
	int wlen = MultiByteToWideChar(CP_ACP, 0, text, len, NULL, 0);
	wchar_t* wtext = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_ACP, 0, text, len, wtext, wlen);
	wtext[wlen] = 0;
	this->logs->AppendToFile(text);
	SendMessage(this->mainWindowHandle, WM_SETTEXT, (LPARAM)ID_MESSAGES_TEXTBOX, (LPARAM)wtext);
	delete[] wtext;
}

//Aktualizacja etykiety informuj�cej o ilo�ci pod��czonych klient�w.
void Client::UpdateConnectedClients()
{
	string connectedClientsText = "Pod��czeni klienci: " + to_string(this->clients->size());
	SendMessage(this->mainWindowHandle, WM_SETTEXT, (WPARAM)ID_CONNECTED_CLIENTS, (LPARAM)this->messages->ConvertTextMessage(connectedClientsText.c_str()));
}

//Wysy�anie wiadomo�ci do wszystkich zainicjalizowanych klient�w.
void Client::SendToAll(string message)
{
	vector<Client*>::iterator iter;
	for (iter = this->clients->begin(); iter != this->clients->end(); iter++)
	{
		if((*iter)->clientINFO->GetID() != this->clientINFO->GetID() && (*iter)->clientINFO->GetName().size()>0)
			(*iter)->SendMessageToClient(message);
	}
}

//Dodanie klienta do ListBoxa.
void Client::AddClientToListbox()
{
	string value = "["+  to_string(this->clientINFO->GetID()) + "] " + this->clientINFO->GetName();
	SendMessage(GetDlgItem(this->mainWindowHandle, ID_CLIENTS_LISTBOX), LB_ADDSTRING, (LPARAM) ID_CLIENTS_LISTBOX, (LPARAM)this->messages->ConvertTextMessage(value.c_str()));
	if (this->clients->size() == 1) {
		SendMessage(GetDlgItem(this->mainWindowHandle, ID_CLIENTS_LISTBOX), LB_SETCURSEL, (WPARAM)0, 0);
		GetDataAboutClient();
	}
}

//Usuni�cie klienta z ListBoxa.
void Client::RemoveClientFromListbox()
{
	string value = "[" + to_string(this->clientINFO->GetID()) + "] " + this->clientINFO->GetName();
	LRESULT findingResult = SendMessageW(GetDlgItem(this->mainWindowHandle, ID_CLIENTS_LISTBOX), LB_FINDSTRINGEXACT, (LPARAM)ID_CLIENTS_LISTBOX, (LPARAM)this->messages->ConvertTextMessage(value.c_str()));
	if(findingResult >= 0)
		SendMessage(GetDlgItem(this->mainWindowHandle, ID_CLIENTS_LISTBOX), LB_DELETESTRING, (WPARAM) findingResult, 0);

	if (this->clients->size() == 0) {
		SetWindowText(GetDlgItem(this->mainWindowHandle, ID_NAME_LABEL), L"Nazwa:");
		SetWindowText(GetDlgItem(this->mainWindowHandle, ID_ID_LABEL), L"ID:");
		SetWindowText(GetDlgItem(this->mainWindowHandle, ID_DATE_LABEL), L"Pod��czony o:");
	}
}
