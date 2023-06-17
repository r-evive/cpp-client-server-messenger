#include "pch.h"
#include "ClientINFO.h"

//Funkcja pobiera aktualny czas serwera.
string ClientINFO::GetCurrentDate() {
    time_t currentTime = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&currentTime);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}

//Ustawienie daty po³¹czenia na aktualny czas.
void ClientINFO::SetConnectionDate()
{
	this->connectionDate = GetCurrentDate();
}
