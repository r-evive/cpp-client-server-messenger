#include "pch.h"
#include "Socket.h"


//Konstruktor
Socket::Socket(HWND & mainWindowHandle) {
	InitWinsock();
	this->mainWindowHandle = mainWindowHandle;
	this->clients = new vector<Client*>();
	this->logs = new Logs("logs.txt");
	this->serverConfig = new ServerConfig();
	this->clientsData = new ClientsData(this->clients, this->mainWindowHandle);
}

//Inicjalizacja biblioteki Winsock.
void Socket::InitWinsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR) {
		this->currentStatus = THROW_ERROR; 
	}
	else {
		this->currentStatus = IDLE;
	}
	
}

//Utworzenie socketu IPv4.
void Socket::CreateSocket() {
	this->mainSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->mainSocket == INVALID_SOCKET) {
		this->currentStatus = THROW_ERROR;
		ShowTextMessage("WYST¥PI£ B£¥D PODCZAS TWORZENIA NOWEGO GNIAZDA!\r\n");
	}
	else {
		this->currentStatus = CREATING;
		ShowTextMessage("Gniazdo zosta³o utworzone pomyœlnie... \r\n");
	}
}

//Pobranie konfiguracji sieciowej z pliku konfiguracyjnego oraz przypisanie adresu do socketu.
void Socket::BindSocket() {
	this->serverConfig->ReadDataFromConfigFile();
	this->ipConf = new IPConfiguration(this->serverConfig->GetIPAddress(), this->serverConfig->GetPort());
	int result = bind(this->mainSocket, this->ipConf->GetSOCKADDR(), this->ipConf->GetSocketSize());
	if (result > 0) {
		this->currentStatus = THROW_ERROR;
		ShowTextMessage("Wyst¹pi³ b³¹d podczas bindowania adresu!\r\n");
	}
	else {
		this->currentStatus = BINDING;
	}
}

//Rozpoczênie nas³uchiwania na ustawionym adresie.
void Socket::StartListening() {
	if (this->currentStatus == BINDING) {
		int listeningResult = listen(this->mainSocket, this->backlogSize);
		if (listeningResult < 0) {
			this->currentStatus = THROW_ERROR;
			ShowTextMessage("WYST¥PI£ B£¥D PODCZAS NAS£UCHIWANIA!\r\n");
			ShowTextMessage("NIEPOPRAWNA KONFIGURACJA ADRESU IP SERWERA!\r\n");
		}
		else {
			string message = "Nas³uchujê na adresie " + this->ipConf->GetIPAddress() + ":" + to_string(this->ipConf->GetPort()) + "\r\n";
			this->currentStatus = LISTENING;
			ShowTextMessage(message.c_str());
			ShowTextMessage("------------------------------------------------------------------------------------------------------\r\n");
		}
	}
}

//Akceptacja nowego po³¹czenia z klientem oraz jego inicjalizacja. Tworzony jest nowy w¹tek, na którym bêd¹ wykonywane operacje klienta.
void Socket::AcceptNewConnection() {
	Client* newClient = new Client();
	SOCKET clientSocket = accept(this->mainSocket, newClient->GetIPConfiguration()->GetSOCKADDR(), newClient->GetIPConfiguration()->GetSocketLenSize());
	if (clientSocket < 0) {
		ShowTextMessage("Wyst¹pi³ b³¹d podczas pod³¹czania nowego klienta!\r\n");
		delete newClient;
	}
	else {
		newClient->GetIPConfiguration()->SetAddressData();
		newClient->InitializeClientSocket(&this->mainSocket, clientSocket, this->mainWindowHandle);
		newClient->SetClientsVector(this->clients);
		this->clients->push_back(newClient);
		thread newThread(&Client::Start, newClient);
		newThread.detach();
	}

}

//Rozpoczênie akceptacji po³¹czeñ przychodz¹cych na nas³uchiwanym adresie.
void Socket::StartAcceptingLoop() {
	if (this->currentStatus == LISTENING) {
		while (true) {
			this->AcceptNewConnection();
		}
	}
}

//Od³¹czenie wszystkich klientów od serwera.
void Socket::DisconnectAll()
{
	vector<Client*>::iterator iter;
	while (clients->size() > 0) {
		try {
			clients->front()->Disconnect();
		}
		catch (exception e) {

		}
		this_thread::sleep_for(16ms);
	}
}

//Od³¹czenie wszystkich klientów i zamkniêcie socketu.
void Socket::Close()
{
	DisconnectAll();
	closesocket(this->mainSocket);
}

//Wysy³anie wiadomoœci do wszystkich zainicjalizowanych klientów.
void Socket::SendToAll(string message)
{
	vector<Client*>::iterator iter;
	for (iter = this->clients->begin(); iter != this->clients->end(); iter++)
	{
		if((*iter)->GetClientINFO()->GetID() != 0)
			(*iter)->SendMessageToClient(message);
	}
}

//Dekonstruktor
Socket::~Socket() {
	delete this->ipConf;
	delete[] this->clients;
	delete this->logs;
	delete this->serverConfig;
	delete this->clientsData;
}

//Funkcja zwraca iloœæ pod³¹czonych klientów.
int Socket::CountConnectedClients()
{
	return this->clients->size();
}

//Konwersacja wiadomoœci na typ wskaŸnikowy i jej wyœwietlenie.
void Socket::ShowTextMessage(const char* text){
	int len = strlen(text);
	int wlen = MultiByteToWideChar(CP_ACP, 0, text, len, NULL, 0);
	wchar_t* wtext = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_ACP, 0, text, len, wtext, wlen);
	wtext[wlen] = 0;
	this->logs->AppendToFile(text);
	SendMessage(this->mainWindowHandle, WM_SETTEXT, (WPARAM)ID_MESSAGES_TEXTBOX, (LPARAM)wtext);
	delete [] wtext;
}

//Wyœwietla informacjê na temat klienta w bocznym pasku, poni¿ej ListBoxa.
void Socket::GetDataAboutClient()
{
	this->clientsData->GetDataAboutClient();
}

//Funkcja wykonywana po naciœniêciu przycisku "Wyœlij", pobiera tekst, wyœwietla go na ekranie i wysy³a do pod³¹czonych klientów.
void Socket::HandleServerMessage()
{
	int length = GetWindowTextLengthW(GetDlgItem(mainWindowHandle, ID_INPUT_TEXTBOX)) + 1;
	wchar_t* text = new wchar_t[length];
	GetWindowTextW(GetDlgItem(mainWindowHandle, ID_INPUT_TEXTBOX), text, length);
	wstring ws(text);
	string str(ws.begin(), ws.end());
	if (str.size()) {
		SendToAll(PrepareServerMessage(str).c_str());
		str = "[SERVER]: " + str + "\r\n";
		ShowTextMessage(str.c_str());
		SendMessage(GetDlgItem(mainWindowHandle, ID_INPUT_TEXTBOX), WM_SETTEXT, 0, (LPARAM)L"");
	}
}

//Przygotowanie poprawnej struktury wysy³anej wiadomoœci.
string Socket::PrepareServerMessage(string message)
{
	message = "<MESSAGE>NAME:SERVER;ID:0;CONTENT:'" + message + "';</MESSAGE>";
	return message;
}

//Od³¹czenie wybranego z ListBoxa klienta.
void Socket::DisconnectSelectedClient()
{
	this->clientsData->DisconnectSelectedClient();
}
