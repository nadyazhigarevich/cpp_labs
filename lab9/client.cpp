#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h> // Для inet_pton

#pragma comment(lib, "ws2_32.lib") // Подключение библиотеки WinSock

using namespace std;

void sendCommand(SOCKET socket, const string& command) {
    send(socket, command.c_str(), command.length(), 0);
    char buffer[1024] = { 0 };
    recv(socket, buffer, sizeof(buffer), 0);
    cout << "Server: " << buffer << endl;
}

void initializeClient(SOCKET& clientSocket, const string& ipAddress, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error: Unable to initialize WinSock" << endl;
        return;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Error: Unable to create socket" << endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    // Используем inet_pton для преобразования IP-адреса
    if (inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        cerr << "Error: Invalid address" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error: Unable to connect to server" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }
}

int main() {
    SOCKET clientSocket;
    string ipAddress = "127.0.0.1"; // IP-адрес сервера
    int port = 12345; // Порт сервера

    initializeClient(clientSocket, ipAddress, port);

    string command;
    while (true) {
        cout << "Enter command (type 'exit' to quit): ";
        getline(cin, command);

        if (command == "exit") {
            break; // Завершение работы клиента
        }

        sendCommand(clientSocket, command);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}