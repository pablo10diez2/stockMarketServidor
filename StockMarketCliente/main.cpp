#include <iostream>
#include <string>
#include <cstring>  // For std::memset
#include <winsock2.h>
#include <ws2tcpip.h> // For better socket support
#include <algorithm> // For std::remove

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000
#define BUFFER_SIZE 1024  // Increased buffer size

// Función para recibir un mensaje completo del servidor
std::string receiveMessage(SOCKET socket) {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);

    int bytes = recv(socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
        return "";
    }

    buffer[bytes] = '\0';
    return std::string(buffer);
}

int main() {
    WSADATA wsaData;
    SOCKET s;
    sockaddr_in server;
    char recvBuff[BUFFER_SIZE];

    std::cout << "Inicializando Winsock...\n";
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. Error: " << WSAGetLastError() << "\n";
        return -1;
    }

    std::cout << "Creando socket...\n";
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    std::cout << "Conectando al servidor " << SERVER_IP << ":" << SERVER_PORT << "...\n";
    if (connect(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error: " << WSAGetLastError() << "\n";
        closesocket(s);
        WSACleanup();
        return -1;
    }

    std::cout << "Conectado al servidor.\n";

    bool connected = true;
    bool inAccountMenu = false;

    while (connected) {
        // Recibir mensaje del servidor
        std::memset(recvBuff, 0, sizeof(recvBuff)); // Limpiar buffer antes de recibir
        int bytes = recv(s, recvBuff, sizeof(recvBuff) - 1, 0);
        if (bytes <= 0) {
            std::cout << "Conexión cerrada por el servidor. Error: " << WSAGetLastError() << "\n";
            break;
        }

        recvBuff[bytes] = '\0';
        std::cout << recvBuff; // Mostrar mensaje del servidor

        // Determinar si estamos en el menú de cuenta
        std::string message(recvBuff);
        if (message.find("=== MENU CUENTA ===") != std::string::npos) {
            inAccountMenu = true;
        }

        // Leer input del usuario
        std::string input;
        std::getline(std::cin, input);

        // Si la entrada está vacía, enviar un espacio para evitar problemas
        if (input.empty()) {
            input = " ";
        }

        // Enviar al servidor
        int sendResult = send(s, input.c_str(), input.size(), 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Error al enviar datos: " << WSAGetLastError() << "\n";
            break;
        }

        // Si estamos en el menú de cuenta, manejar las opciones especiales
        if (inAccountMenu) {
            if (input == "1") {  // Ver perfil
                // Primero recibe confirmación de opción seleccionada
                std::string response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }

                // Luego recibe los datos del perfil
                Sleep(100);
                response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }
            }
            else if (input == "2") {  // Cambiar contraseña
                // Primero recibe confirmación de opción seleccionada
                std::string response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }

                // Luego recibe solicitud de nueva contraseña
                Sleep(100);
                response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }

                // Ingresar nueva contraseña
                std::string newPassword;
                std::getline(std::cin, newPassword);
                send(s, newPassword.c_str(), newPassword.size(), 0);

                // Recibir confirmación
                Sleep(100);
                response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }
            }
            else if (input == "3") {  // Introducir fondos
                // Primero recibe confirmación de opción seleccionada
                std::string response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }

                // Luego recibe solicitud de número de cuenta
                Sleep(100);
                response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }

                // Ingresar número de cuenta
                std::string accountNumber;
                std::getline(std::cin, accountNumber);
                send(s, accountNumber.c_str(), accountNumber.size(), 0);

                // Recibir solicitud de cantidad
                Sleep(100);
                response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }

                // Ingresar cantidad
                std::string amount;
                std::getline(std::cin, amount);
                send(s, amount.c_str(), amount.size(), 0);

                // Recibir confirmación
                Sleep(100);
                response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }
            }
            else if (input == "4") {  // Volver al menú principal
                std::string response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }
                inAccountMenu = false;
            }
            else if (input == "5") {  // Salir completamente
                std::string response = receiveMessage(s);
                if (!response.empty()) {
                    std::cout << response;
                }
                connected = false;
            }
        }
        // Si estamos en el menú principal y seleccionamos la opción de cuenta
        else if (input == "3") {
            // Recibir mensaje "Accediendo a gestión de cuenta..."
            Sleep(100);
            std::string response = receiveMessage(s);
            if (!response.empty()) {
                std::cout << response;
            }
        }
        // Si seleccionamos salir desde el menú principal
        else if (input == "4" && !inAccountMenu) {
            // Recibir mensaje de despedida
            Sleep(100);
            std::string response = receiveMessage(s);
            if (!response.empty()) {
                std::cout << response;
            }
            connected = false;
        }
    }

    std::cout << "Cerrando conexión...\n";
    closesocket(s);
    WSACleanup();
    std::cout << "Conexión cerrada. Presiona Enter para salir.\n";
    std::cin.get(); // Esperar a que el usuario presione Enter
    return 0;
}
