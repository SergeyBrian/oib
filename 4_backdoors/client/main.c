#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT             "12345"
#define DEFAULT_MESSAGE_LEN      200
#define WINSOCK_API_INIT_ERROR   0xFF
#define GET_ADDR_INFO_ERROR      0xFE
#define SOCKET_INIT_ERROR        0xFD
#define CONNECT_ERROR            0xF9



int main(void)
{
    WSADATA wsa_data;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in addr;
    int result;

    char ip_address[20];
//    char message[DEFAULT_MESSAGE_LEN] = "";
    char file_name[DEFAULT_MESSAGE_LEN] = "";

    result = WSAStartup(MAKEWORD(1, 1), &wsa_data);
    if (result != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        return WINSOCK_API_INIT_ERROR;
    }
    fprintf(stdout, "Set ip address for connect:");
    scanf("%s", ip_address);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, ip_address, &addr.sin_addr.s_addr);

    fprintf(stdout, "Enter file to delete:");
    scanf("%s", file_name);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return SOCKET_INIT_ERROR;
    }

    if (connect(client_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "connect failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return CONNECT_ERROR;
    }
    fprintf(stdout, "Connection established!\n");

//    int connect = recv(client_socket, message, sizeof(message), 0);
    send(client_socket, file_name, sizeof(file_name), 0);
//    fprintf(stdout, "Message from server:\n %s", message);



    getchar();
}