#include "pch.h"
#include "PreConnection.h"

//Konstruktor
PreConnection::PreConnection(HWND & mainWindowHandle) {
    this->mainWindowHandle = mainWindowHandle;
}
//Pobiera wartoœæ pola tekstowego IPAddress i przypisuje je do zmiennej.
void PreConnection::HandleIPAddress()
{
    SetIPAddress("");
    int length = GetWindowTextLengthW(GetDlgItem(this->mainWindowHandle, ID_ADDRESS_TEXTBOX)) + 1;
    wchar_t* text = new wchar_t[length];
    GetWindowTextW(GetDlgItem(this->mainWindowHandle, ID_ADDRESS_TEXTBOX), text, length);
    wstring ws(text);
    string str(ws.begin(), ws.end());
    if (str.size()) {
        SetIPAddress(str);
    }
}

//Pobiera wartoœæ pola tekstowego Name i przypisuje je do zmiennej.
void PreConnection::HandleName()
{
    SetName("");
    int length = GetWindowTextLengthW(GetDlgItem(this->mainWindowHandle, ID_NAME_TEXTBOX)) + 1;
    wchar_t* text = new wchar_t[length];
    GetWindowTextW(GetDlgItem(this->mainWindowHandle, ID_NAME_TEXTBOX), text, length);
    wstring ws(text);
    string str(ws.begin(), ws.end());
    if (str.size()) {
        SetName(str);
    }
}

//Pobiera wartoœæ pola tekstowego Port i przypisuje je do zmiennej.
void PreConnection::HandlePort()
{
    SetPort(0);
    int length = GetWindowTextLengthW(GetDlgItem(this->mainWindowHandle, ID_PORT_TEXTBOX)) + 1;
    wchar_t* text = new wchar_t[length];
    GetWindowTextW(GetDlgItem(this->mainWindowHandle, ID_PORT_TEXTBOX), text, length);
    wstring ws(text);
    string str(ws.begin(), ws.end());
    if (str.size()) {
        try {
            SetPort(stoi(str));
        }
        catch (exception e) {
        }
    }
}
