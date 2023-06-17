#include "pch.h"
#include "Client.h"

//Konstruktor
Client::Client(SOCKET& mainSocket, IPConfiguration* ipConf, HWND & mainWindowHandle) {
	this->mainSocket = mainSocket;
	this->mainWindowHandle = mainWindowHandle;
	this->ipConf = ipConf;
	this->messages = new Messages(&this->clientINFO);
}

//Rozpoczyna nas³uchiwanie wiadomoœci od serwera.
void Client::Start() {
	Listen();
	delete this;
}

//Nas³uchuje wiadomoœci pochodz¹ce od serwera.
void Client::Listen() {
	while (this->threadActive) {
		vector<char> dataReceived(5000);
		int bytes = recv(mainSocket, dataReceived.data(), dataReceived.size(), 0);
		if (bytes > 0) {
			string message = "";
			for (int i = 0; i < bytes; i++) {
				message += dataReceived[i];
			}
			if (message.size() > 0) {
				TranslateMessage(message);
			}
		}
		this_thread::sleep_for(16ms);
	}
}

//Zatrzymuje aktualny w¹tek i roz³¹cza po³aczenie z socketem.
void Client::Disconnect()
{
	this->threadActive = false;
	SendData(this->messages->GetDisconnectMessage());
	ShowTextMessage("Roz³¹czono...\r\n");
	closesocket(this->mainSocket);
}

//Wysy³a wiadomoœæ do serwera.
void Client::SendData(string message) {
	
	send(this->mainSocket, message.c_str(), message.size(), 0);
}

//Dekonstruktor
Client::~Client() {
	delete ipConf;
}

//Funkcja ta wysy³a do serwera dane inicjalizacyjne (czyli nazwe klienta oraz jego unikalne ID)
void Client::InitData() {
	SendData(this->clientINFO.GetInitData());
}

//Funkcja ta otrzymuje "surow¹" wiadomoœ, po czy pobiera z niej wymagane informacje i wykonuje okreœlone na podstawie OrderType akcje.
void Client::TranslateMessage(string message)
{
	string orderType = this->messages->FindOrderType(message);
	string name, content;
	int id;

	this->messages->GetDataAndContent(message, name, id, content);

	if (orderType == "MESSAGE")
	{

		string outputMessage = "[" + name + "]: " + content + "\r\n";
		ShowTextMessage(outputMessage.c_str());
	}
	else if (orderType == "DISCONNECT") {
		if (this->threadActive) {
			Disconnect();
			SendMessage(mainWindowHandle, WM_NOTIFY, 0, 0);
		}
	}
	else if (orderType == "INFO")
	{
		string outputMessage = name + " roz³¹czy³ siê...\r\n";
		ShowTextMessage(outputMessage.c_str());
	}
	else if (orderType == "NEWCLIENT")
	{
		string outputMessage = name + " do³¹czy³ do sesji...\r\n";
		ShowTextMessage(outputMessage.c_str());
	}
}

//Funkcja ta wyœwietla wiadomoœæ na ekranie.
void Client::ShowTextMessage(const char* text)
{
	int len = strlen(text);
	int wlen = MultiByteToWideChar(CP_ACP, 0, text, len, NULL, 0);
	wchar_t* wtext = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_ACP, 0, text, len, wtext, wlen);
	wtext[wlen] = 0;
	SendMessage(this->mainWindowHandle, WM_SETTEXT, 0, (LPARAM)wtext);
	delete[] wtext;
}
