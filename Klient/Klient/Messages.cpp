#include "pch.h"
#include "Messages.h"

//Konstruktor
Messages::Messages(ClientINFO* clientINFO)
{
	this->clientINFO = clientINFO;
}

//Funkcja ta zwraca jakim typem wiadomoœci jest otrzymana wiadomoœæ np. MESSAGE, DISCONNECT, INFO
string Messages::FindOrderType(string message)
{
	try {
		string type = message.substr(message.find("<") + 1, message.substr(message.find("<") + 1).find(">") - message.find("<"));
		return type;
	}
	catch (exception e) {
		return "NULL";
	}
}

//Funkcja ta przygotowuje i zwraca poprawn¹ wiadomoœæ o od³¹czeniu od serwera.
string Messages::GetDisconnectMessage()
{
	string header = "<DISCONNECT>";
	string content = "NAME:" + this->clientINFO->GetName() + ";ID:" + to_string(this->clientINFO->GetID()) + ";";
	string ending = "</DISCONNECT>";
	return header + content + ending;
}

//Funkcja ta destrukturyzuje wiadomoœæ i pobiera wartoœci wymaganych zmiennych.
void Messages::GetDataAndContent(string message, string& name, int& id, string& content)
{
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

}
