#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // Asegura que se enlace con Winsock

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET s;
    sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error inicializando Winsock: " << WSAGetLastError() << "\n";
        return -1;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Error creando socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Error conectando al servidor: " << WSAGetLastError() << "\n";
        closesocket(s);
        WSACleanup();
        return -1;
    }

    std::cout << "Conectado al servidor.\n";

    char buffer[BUFFER_SIZE];
    std::string userInput;

    while (true) {
        // Recibir datos del servidor
        int bytes = recv(s, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            std::cout << "\nConexión cerrada por el servidor o error.\n";
            break;
        }
        buffer[bytes] = '\0';
        std::cout << buffer;

        // Leer del usuario
        std::cout << "> ";
        std::getline(std::cin, userInput);

        // Permitir salida con "exit" desde el cliente
        if (userInput == "exit") {
            std::cout << "Cerrando conexión...\n";
            break;
        }

        // Enviar al servidor
        if (send(s, userInput.c_str(), userInput.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Error enviando datos: " << WSAGetLastError() << "\n";
            break;
        }
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
