
#include "pch.h"
#include "framework.h"
#include "ServerProject.h"
#include <thread>
#include "Socket.h"
#include <atlstr.h>
#include "Controls.h"

using namespace std;

//Zmienne globalne.
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];
Socket* sock;

//Utworzenie potrzebnych komponentów (przyciski, pola tekstowe, etykiety itp.)
HWND mainWindowHandle;
HWND messagesTextbox;
HWND inputTextbox;
HWND sendButton;
HWND connectedClientsLabel;
HWND clientsListBox;
HWND infoLabel;
HWND clientNameLabel;
HWND clientIDLabel;
HWND clientConnectionDateLabel;
HFONT hFont;
LOGFONT lf;

//Prototypy funkcji
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void StartSocket();
void CreateSocketThread();
void WaitForClients();
void AppendMessage(HWND hWndEdit, LPCTSTR pszText);

//Zdefiniowanie zmiennej typu Socket i rozpoczęcie nasłuchiwania.
void StartSocket() {
    sock = new Socket(mainWindowHandle);
    sock->CreateSocket();
    sock->BindSocket();
    sock->StartListening();
    sock->StartAcceptingLoop();

}

//Utworzenie nowego wątku, na którym będą obsługiwane połączenia.
void CreateSocketThread() {
    thread socketThread(StartSocket);
    socketThread.detach();
}

//Oczekiwanie na rozłączenie się wszystkich klientów.
void WaitForClients() {
    while (sock->CountConnectedClients() > 0) {
        this_thread::sleep_for(16ms);
    }
}

//Dodanie wiadomości do pola tekstowego.
void AppendMessage(HWND hWndEdit, LPCTSTR pszText)
{
    int nLength = GetWindowTextLength(hWndEdit);
    SendMessage(hWndEdit, EM_SETSEL, (WPARAM)nLength, (LPARAM)nLength);
    SendMessage(hWndEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)pszText);
}

//Główna funkcja Main.
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    setlocale(LC_CTYPE, "Polish");
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVERPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERPROJECT));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//Rejestracja klasy okna, potrzebna do utworzenia nowego okna (określa jego strukturę i wygląd).
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVERPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//Inicjalizacja instancji okna - zdefiniowanie okna, kontrolek oraz czcionek.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   mainWindowHandle = CreateWindowW(szWindowClass, L"Serwer", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
      CW_USEDEFAULT, 0, 600, 390, nullptr, nullptr, hInstance, nullptr);

   messagesTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
       ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 10, 10, 400, 300, mainWindowHandle, (HMENU)ID_MESSAGES_TEXTBOX, hInstance, NULL);

   inputTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE |
       ES_LEFT | ES_AUTOVSCROLL, 10, 315, 300, 25, mainWindowHandle, (HMENU)ID_INPUT_TEXTBOX, hInstance, NULL);

   sendButton = CreateWindow(L"BUTTON", L"Wyślij", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 320, 315, 90, 25, mainWindowHandle, (HMENU)ID_SEND, hInstance, NULL);

   connectedClientsLabel = CreateWindowW(L"static", L"Podłączeni klienci: 0", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 20, 150, 20, mainWindowHandle, nullptr, hInstance, NULL);

   clientsListBox = CreateWindowW(L"LISTBOX", L"LISTBOX", WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY, 420, 50, 150, 100, mainWindowHandle ,(HMENU)ID_CLIENTS_LISTBOX , hInstance, NULL);

    infoLabel = CreateWindowW(L"static", L"Informacje:", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 160, 150, 20, mainWindowHandle, nullptr, hInstance, NULL);
    clientNameLabel = CreateWindowW(L"static", L"Nazwa:", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 180, 150, 20, mainWindowHandle, (HMENU) ID_NAME_LABEL, hInstance, NULL);
    clientIDLabel = CreateWindowW(L"static", L"ID:", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 200, 150, 20, mainWindowHandle, (HMENU)ID_ID_LABEL, hInstance, NULL);
    clientConnectionDateLabel = CreateWindowW(L"static", L"Podłączony o:", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 220, 150, 20, mainWindowHandle, (HMENU)ID_DATE_LABEL, hInstance, NULL);

   GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
   hFont = CreateFont(lf.lfHeight * 1.1, lf.lfWidth * 1.1,
       lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
       lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
       lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
       lf.lfPitchAndFamily, lf.lfFaceName);

   SendMessage(inputTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(messagesTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(sendButton, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(connectedClientsLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(clientsListBox, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(infoLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(clientNameLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(clientIDLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
   SendMessage(clientConnectionDateLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

   SendMessage(messagesTextbox, EM_SETREADONLY, 1, 0);

   if (!mainWindowHandle)
      return FALSE;

   ShowWindow(mainWindowHandle, nCmdShow);
   UpdateWindow(mainWindowHandle);
   CreateSocketThread();
   return TRUE;
}

//Obsługa komunikatów przez okno.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:  
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case ID_SEND:
                sock->HandleServerMessage();
            break;
        }

        switch (HIWORD(wParam)) {
            case LBN_DBLCLK: {
                    int result = MessageBox(NULL, L"Czy napewno chcesz wyrzucić tego użytkownika?", L"Potwierdzenie", MB_OKCANCEL | MB_ICONWARNING);
                    switch (result) {
                        case IDOK:
                            sock->DisconnectSelectedClient();
                        break;
                    }
                }            
            break;
            case LBN_SELCHANGE:
                sock->GetDataAboutClient();
            break;
        }

        break;
    case WM_SETTEXT:
        switch (LOWORD(wParam)) {
            case ID_MESSAGES_TEXTBOX:
                AppendMessage(messagesTextbox, (LPCTSTR) lParam);
                break;
            case ID_CONNECTED_CLIENTS:
                SetWindowText(connectedClientsLabel, (LPCTSTR)lParam);
                break;
        }
        break;
    case WM_CLOSE:
        sock->Close();
        WaitForClients();
        DestroyWindow(mainWindowHandle);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
