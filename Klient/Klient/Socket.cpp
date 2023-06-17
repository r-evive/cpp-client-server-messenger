#include "pch.h"
#include "Socket.h"

//Inicjalizacja biblioteki WinSock.
void Socket::InitWinsock()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR) {
		this->currentStatus = THROW_ERROR;
		exit(0);
	}
	else {
		this->currentStatus = IDLE;
	}
}

//Konstruktor parametrowy.
Socket::Socket(HWND & mainWindowHandle)
{
	InitWinsock();
	this->mainWindowHandle = mainWindowHandle;
	this->preConnection = new PreConnection(this->mainWindowHandle);
}

//Utworzenie nowego socketu.
void Socket::CreateSocket()
{
	this->mainSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->mainSocket == INVALID_SOCKET) {
		this->currentStatus = THROW_ERROR;
		ShowTextMessage("WYST¥PI£ B£¥D PODCZAS TWORZENIA NOWEGO GNIAZDA!\r\n");
		exit(0);
	}
	else {
		this->currentStatus = CREATING;
	}
}
//Przypisanie socketu do wybranego adresu IP.
void Socket::BindServerAddress(string ipAddress, int port)
{
	this->ipConf = new IPConfiguration(ipAddress, port);
}

//Próba po³¹czenia z serwerem na podanym przez u¿ytkownika adresie. Je¿eli po³¹czenie siê nie powiedzie
//to nastêpuje kolejna próba po³¹czenia.
void Socket::Connect() {
	if (!this->preConnection->GetConnectionStatus()) {
		SetWindowText(GetDlgItem(this->mainWindowHandle, ID_CONNECT), L"£¹czenie...");
		this->preConnection->SetConnectingStatus(true);
	}
	int result = connect(this->mainSocket, this->ipConf->GetSOCKADDR(), this->ipConf->GetSocketSize());
	if (result < 0)
	{
		if (this->preConnection->GetConnectionStatus()) {
			ShowTextMessage("Wyst¹pi³ b³¹d podczas ³¹czenia z serwerem...\r\n");
			Connect();
		}
	}
	else {
		SetWindowText(GetDlgItem(this->mainWindowHandle, ID_CONNECT), L"Roz³¹cz...");
		this->preConnection->SetConnectingStatus(false);
		this->preConnection->SetConnected(true);
		string message = "Pomyœlnie pod³¹czono do serwera o adresie: " + this->ipConf->GetIPAddress() + ":" + to_string(this->ipConf->GetPort()) + "\r\n";
		ShowTextMessage(message.c_str());
		currentClient = new Client(this->mainSocket, this->ipConf, this->mainWindowHandle);
		currentClient->GetClientInfo().SetName(this->preConnection->GetName());
		currentClient->InitData();
		thread newThread(&Client::Start, currentClient);
		newThread.detach();
	}
}

//Od³¹czenie od serwera.
void Socket::Disconnect()
{
	this->currentClient->Disconnect();
}

//Przygotowanie poprawnej wiadomoœci tekstowej, która jest wysy³ana do serwera.
string Socket::PrepareTextMessage(string message)
{
	string header = "<MESSAGE>";
	string name = "NAME:" + this->currentClient->GetClientInfo().GetName() + ";";
	string id = "ID:" + to_string(this->currentClient->GetClientInfo().GetID()) + ";";
	string content = "CONTENT:'" + message + "';";
	string ending = "</MESSAGE>";
	string propperMessage = header + name + id + content + ending;

	return propperMessage;
}

//Funkcja ta wysy³a wiadomoœæ typu MESSAGE do serwera.
void Socket::SendTextMessage(string message) {
	this->currentClient->SendData(PrepareTextMessage(message));
	message = "[" + this->currentClient->GetClientInfo().GetName() + "]: " + message + "\r\n";
	ShowTextMessage(message.c_str());

}

//Wyœwietlenie wiadomoœci tekstowej na ekranie.
void Socket::ShowTextMessage(const char* text)
{
	int len = strlen(text);
	int wlen = MultiByteToWideChar(CP_ACP, 0, text, len, NULL, 0);
	wchar_t* wtext = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_ACP, 0, text, len, wtext, wlen);
	wtext[wlen] = 0;
	SendMessage(this->mainWindowHandle, WM_SETTEXT, 0, (LPARAM)wtext);
	delete[] wtext;
}

//Obs³uga przycisku wysy³ania wiadomoœci
void Socket::HandleSendMessage()
{
	int length = GetWindowTextLengthW(GetDlgItem(this->mainWindowHandle, 500)) + 1;
	wchar_t* text = new wchar_t[length];
	GetWindowTextW(GetDlgItem(this->mainWindowHandle, 500), text, length);
	wstring ws(text);
	string str(ws.begin(), ws.end());
	if (str.size()) {
		SendTextMessage(str);
		SendMessage(GetDlgItem(this->mainWindowHandle, 500), WM_SETTEXT, 0, (LPARAM)L"");
	}

}

//Dekonstruktor
Socket::~Socket()
{
	delete this->ipConf;
	delete this->currentClient;
	delete this->preConnection;
}

