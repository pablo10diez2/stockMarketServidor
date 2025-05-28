#include <iostream>
#include <string>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

int main() {
    WSADATA wsaData;
    SOCKET s;

    sockaddr_in server;
    char recvBuff[512];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. Error: " << WSAGetLastError() << "\n";
        return -1;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error: " << WSAGetLastError() << "\n";
        closesocket(s);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to server.\n";

    while (true) {
        // Recibir mensaje del servidor
        int bytes = recv(s, recvBuff, sizeof(recvBuff) - 1, 0);
        if (bytes <= 0) {
            std::cout << "Connection closed by server.\n";
            break;
        }
        recvBuff[bytes] = '\0';
        std::cout << recvBuff;

        // Leer input del usuario
        std::string input;
        std::getline(std::cin, input);

        // Enviar al servidor
        send(s, input.c_str(), input.size(), 0);

        if (input == "3") break; // Salir opcional (depende del servidor)
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
