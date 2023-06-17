#include "pch.h"
#include "Logs.h"

//Konstruktor parametrowy.
Logs::Logs(string logsFilePath)
{
    this->logsFilePath = logsFilePath;
    try {
        file = new ofstream(logsFilePath, ios::app);
        file->close();
    }
    catch (exception e) {
        file->close();
    }
}

//Dekonstruktor.
Logs::~Logs()
{
    delete this->file;
}

//Funkcja zwraca aktualn¹ date serwera w formacie ROK-MIESI¥C-DZIEN GODZINA:MINUTY:SEKUNDY.
string Logs::GetCurrentTimeHeader()
{
    time_t currentTime = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&currentTime);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    string timeString(buf);
    timeString = "[" + timeString + "]";
    return timeString;
}

//Dodanie wiadomoœci do pliku logów.
void Logs::AppendToFile(string message)
{
    try {
        message.erase(remove(message.begin(), message.end(), '\n'), message.end());
        message = GetCurrentTimeHeader() + " " + message;
        file->open(logsFilePath, ios::app);
        file->write(message.c_str(), message.size());
        file->close();
    }
    catch (exception e) {
        file->close();
    }
}
