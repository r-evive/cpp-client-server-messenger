#include "pch.h"
#include "Messages.h"

//Konstruktor paramterowy
Messages::Messages(ClientINFO* clientINFO)
{
	this->clientINFO = clientINFO;
}

//Funkcja zwraca przygotowan� wiadomo��, kt�ra informuje o do��czeniu nowego klienta do serwera.
string Messages::MakeNewClientMessage(int id, string name)
{
	string header = "<NEWCLIENT>";
	string message = "NAME:" + name + ";ID:" + to_string(id) + ";";
	string ending = "</NEWCLIENT>";

	return header + message + ending;
}

//Funkcja konwertuje wiadomo�� na d�ugi wska�nik do stringa (LPWSTR), kt�ry jest wykorzystywany do wy�wietlania na ekranie.
wchar_t* Messages::ConvertTextMessage(const char* text)
{
	int len = strlen(text);
	int wlen = MultiByteToWideChar(CP_ACP, 0, text, len, NULL, 0);
	wchar_t* wtext = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_ACP, 0, text, len, wtext, wlen);
	wtext[wlen] = 0;
	return wtext;
}

//Funkcja zwraca przygotowan� wiadomo��, kt�ra jest odpowiedzialna za od��czenie klienta od serwera.
string Messages::GetDisconnectMessage()
{
	string header = "<DISCONNECT>";
	string content = "NAME:" + this->clientINFO->GetName() + ";ID:" + to_string(this->clientINFO->GetID()) + ";";
	string ending = "</DISCONNECT>";

	return header + content + ending;
}

//Funkcja zwraca przygotowan� wiadomo��, kt�ra informuje �e dany klient od��czy� si� od serwera.
string Messages::GetDisconnectInfo()
{
	string header = "<INFO>";
	string content = "NAME:" + this->clientINFO->GetName() + ";ID:" + to_string(this->clientINFO->GetID()) + ";";
	string ending = "</INFO>";

	return header + content + ending;
}

//Funkcja ustawia nazw� oraz ID klienta, kt�ry dopiero co po��czy� si� z serwerem.
void Messages::SetInitData(string command)
{
	string name;
	int id;

	try {
		name = command.substr(command.find_first_of(":") + 1, command.find_first_of(";") - command.find_first_of(":") - 1);
	}
	catch (exception e) {
		name = "";
	}

	this->clientINFO->SetName(name);

	command = command.substr(command.find_first_of(";") + 1);
	try {
		id = stoi(command.substr(command.find_first_of(":") + 1, command.find_first_of(";") - command.find_first_of(":") - 1));
	}
	catch (exception e) {
		id = 0;
	}

	this->clientINFO->SetID(id);
}

//Funkcja dekoduje wiadomo�� tekstow� (typ MESSAGE) i zwraca j� w okre�lonym formacie (do wy�wietlenia na ekranie).
string Messages::TranslateMessage(string message)
{
	string name, content;
	int id;

	try {
		name = message.substr(message.find("NAME:") + 5, message.find_first_of(";") - message.find("NAME:") - 5);
		message = message.substr(message.find_first_of(";") + 1);
	}
	catch (exception e) {
		name = "";
	}

	try {
		id = stoi(message.substr(message.find("ID:") + 3, message.find_first_of(";") - message.find("ID:") - 3));
		message = message.substr(message.find_first_of(";") + 1);
	}
	catch (exception e) {
		id = 0;
	}

	try {
		content = message.substr(message.find("CONTENT:'") + 9, message.find("';") - message.find("CONTENT:'") - 9);
	}
	catch (exception e) {
		content = "";
	}
	return "[" + name + " - " + to_string(id) + "]: " + content + "\r\n";
}

//Funkcja znajduje i zwraca typ otrzymanej wiadomo��i (np. MESSAGE, INIT, INFO itp.).
string Messages::FindOrderType(string command)
{
	try {
		string type = command.substr(command.find("<") + 1, command.substr(command.find("<") + 1).find(">") - command.find("<"));
		return type;
	}
	catch (exception e) {
		return "NULL";
	}
}

//Funkcja ta usuwa typ wiadomo�ci z otrzymanej wiadomo�ci.
string Messages::CutMessage(string command)
{
	command = command.substr(command.find(">") + 1);
	command = command.substr(0, command.find("<"));
	return command;
}
