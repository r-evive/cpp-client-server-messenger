#include "pch.h"
#include "ClientINFO.h"

//Funkcja ta ustawia unikalne ID oraz przygotowuje poprawn� wiadomo�� inicjalizacyjn�.
string ClientINFO::GetInitData()
{
	this->id = reinterpret_cast<uint32_t>(this);
	string initHeader = "<INIT>";
	string initEnd = "</INIT>";
	string initContent = "NAME:" + this->name + ";" + "ID:" + to_string(this->id) + ";";

	return initHeader + initContent + initEnd;
}
