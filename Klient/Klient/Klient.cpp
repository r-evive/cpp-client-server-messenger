#include "pch.h"
#include "framework.h"
#include "Klient.h"
#include "Socket.h"
#include "Controls.h"



//Zmienne globalne
HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];     
string connectionAddress;
string connectionName;
int connectionPort;

//Utworzenie potrzebnych komponentów (przyciski, pola tekstowe, etykiety itp.).
HWND mainWindowHandle;
HWND messagesTextbox;
HWND inputTextbox;
HWND sendButton;
HWND nameTextbox;
HWND addressTextbox;
HWND portTextbox;
HWND connectButton;
HWND nameLabel;
HWND addressLabel;
HWND portLabel;
Socket* sock;
HFONT hFont;
LOGFONT lf;


//Prototypu funkcji
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void AppendMessage(HWND hWndEdit, LPCTSTR pszText);
void ConnectToSocket();
void StartSocket();
void CreateSocketThread();

//Dodanie wiadomości do pola tekstowego.
void AppendMessage(HWND hWndEdit, LPCTSTR pszText)
{
    int nLength = GetWindowTextLength(hWndEdit);
    SendMessage(hWndEdit, EM_SETSEL, (WPARAM)nLength, (LPARAM)nLength);
    SendMessage(hWndEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)pszText);
}

//Inicjalizacja połączenia z serwerem.
void ConnectToSocket() {
    sock->BindServerAddress(sock->GetPreConnection()->GetIPAddress(), sock->GetPreConnection()->GetPort());
    sock->Connect();
}

//Utworzenie nowego socketu.
void StartSocket() {
    sock = new Socket(mainWindowHandle);
    sock->CreateSocket();
}

//Utworzenie nowego wątku, na którym będzie obsługiwane połączenie z serwerem.
void CreateSocketThread() {
    thread socketThread(StartSocket);
    socketThread.detach();
}

//Główna funkcja Main.
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KLIENT));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//Rejestracja klasy okna, potrzebna do utworzenia nowego okna (określa jego strukturę i wygląd).
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//Inicjalizacja instancji okna - zdefiniowanie okna, kontrolek oraz czcionek.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    mainWindowHandle = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, 600, 390, nullptr, nullptr, hInstance, nullptr);

    mainWindowHandle = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, 600, 390, nullptr, nullptr, hInstance, nullptr);

    messagesTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 10, 10, 400, 300, mainWindowHandle, (HMENU)ID_MESSAGES_TEXTBOX, hInstance, NULL);

    inputTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE |
        ES_LEFT | ES_AUTOVSCROLL, 10, 315, 300, 25, mainWindowHandle, (HMENU)ID_INPUT_TEXTBOX, hInstance, NULL);

    nameTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE |
        ES_LEFT | ES_AUTOVSCROLL, 420, 40, 150, 25, mainWindowHandle, (HMENU)ID_NAME_TEXTBOX, hInstance, NULL);

    addressTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE |
        ES_LEFT | ES_AUTOVSCROLL, 420, 100, 150, 25, mainWindowHandle, (HMENU)ID_ADDRESS_TEXTBOX, hInstance, NULL);

    portTextbox = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE |
        ES_LEFT | ES_AUTOVSCROLL, 420, 160, 150, 25, mainWindowHandle, (HMENU)ID_PORT_TEXTBOX, hInstance, NULL);

    sendButton = CreateWindow(L"BUTTON", L"Wyślij", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 320, 315, 90, 25, mainWindowHandle, (HMENU)ID_SEND, hInstance, NULL);

    connectButton = CreateWindow(L"BUTTON", L"Połącz", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 420, 200, 150, 25, mainWindowHandle, (HMENU)ID_CONNECT, hInstance, NULL);

    nameLabel = CreateWindow(L"static", L"Nazwa:",WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 20, 150, 20, mainWindowHandle, (HMENU)ID_NAME, hInstance, NULL);
    addressLabel = CreateWindow(L"static", L"Adres IP:",WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 80, 150, 20, mainWindowHandle, (HMENU)ID_ADDRESS, hInstance, NULL);
    portLabel = CreateWindow(L"static", L"Port:",WS_CHILD | WS_VISIBLE | WS_TABSTOP, 420, 140, 150, 20, mainWindowHandle, (HMENU)ID_PORT, hInstance, NULL);

    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    hFont = CreateFont(lf.lfHeight * 1.1, lf.lfWidth * 1.1,
        lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
        lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);

    SendMessage(inputTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(messagesTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(sendButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(nameTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(addressTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(portTextbox, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(connectButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(nameLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(addressLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(portLabel, WM_SETFONT, (WPARAM)hFont, TRUE);


    SendMessage(messagesTextbox, EM_SETREADONLY, 1, 0);

    if (!mainWindowHandle)
        return FALSE;

    CreateSocketThread();
    ShowWindow(mainWindowHandle, nCmdShow);
    UpdateWindow(mainWindowHandle);

    return TRUE;
}

//Obsługa komunikatów przez okno.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (HIWORD(wParam)) {
            case BN_CLICKED:
                switch (LOWORD(wParam)) {
                    case ID_SEND:
                        if (sock->GetPreConnection()->IsConnected())
                            sock->HandleSendMessage();
                     break;
                    case ID_CONNECT:
                        sock->GetPreConnection()->HandleIPAddress();
                        sock->GetPreConnection()->HandleName();
                        sock->GetPreConnection()->HandlePort();
                        if (!sock->GetPreConnection()->GetConnectionStatus() && !sock->GetPreConnection()->IsConnected()) {
                            if (sock->GetPreConnection()->GetName().size() > 0 && sock->GetPreConnection()->GetIPAddress().size() > 0 && sock->GetPreConnection()->GetPort() > 0) {
                                thread connectionThread(ConnectToSocket);
                                connectionThread.detach();
                            }
                            else
                                AppendMessage(messagesTextbox, L"Podano niepoprawne dane serwera!\r\n");
                        }
                        else if(!sock->GetPreConnection()->IsConnected()){
                            sock->GetPreConnection()->SetConnectingStatus(false);
                            SetWindowText(connectButton, L"Połącz");
                        }
                        else if (sock->GetPreConnection()->IsConnected()) {
                            sock->Disconnect();
                            SendMessage(mainWindowHandle, WM_NOTIFY, 0, 0);
                        }
                        break;
                }
            break;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SETTEXT:
        AppendMessage(messagesTextbox, (LPCTSTR)lParam);
        break;
    case WM_NOTIFY:
        sock->GetPreConnection()->SetConnected(false);
        SetWindowText(connectButton, L"Połącz");
        CreateSocketThread();
        break;
    case WM_DESTROY:
        if(sock->GetPreConnection()->IsConnected())
            sock->Disconnect();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}