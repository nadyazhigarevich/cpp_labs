#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Подключение библиотеки WinSock

using namespace std;

const int BOARD_SIZE = 15; // Размер игрового поля
const char EMPTY = '.';
const char PLAYER_X = 'X';
const char PLAYER_O = 'O';

class GameBoard {
public:
    GameBoard() : board(BOARD_SIZE, vector<char>(BOARD_SIZE, EMPTY)) {}

    // Отображение игрового поля
    string display() const {
        string result;
        for (const auto& row : board) {
            for (char cell : row) {
                result += cell;
                result += ' ';
            }
            result += '\n';
        }
        return result;
    }

    // Совершить ход
    bool makeMove(int x, int y, char player) {
        if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[x][y] != EMPTY) {
            return false;
        }
        board[x][y] = player;
        return true;
    }

    // Проверка на победу
    bool checkWin(int x, int y, char player) {
        return checkDirection(x, y, player, 1, 0) || // Горизонтально
            checkDirection(x, y, player, 0, 1) || // Вертикально
            checkDirection(x, y, player, 1, 1) || // Диагональ 
            checkDirection(x, y, player, 1, -1);  // Диагональ /
    }

private:
    vector<vector<char>> board;

    // Проверка направления
    bool checkDirection(int x, int y, char player, int dx, int dy) {
        int count = 1;

        // Вперед
        for (int i = 1; i < 5; ++i) {
            int nx = x + i * dx, ny = y + i * dy;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == player) {
                ++count;
            }
            else {
                break;
            }
        }

        // Назад
        for (int i = 1; i < 5; ++i) {
            int nx = x - i * dx, ny = y - i * dy;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == player) {
                ++count;
            }
            else {
                break;
            }
        }

        return count >= 5;
    }
};

void runServer(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error: Unable to initialize WinSock" << endl;
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error: Unable to create socket" << endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error: Unable to bind socket" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, 2) == SOCKET_ERROR) {
        cerr << "Error: Unable to listen on socket" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    cout << "Server is running on port " << port << "..." << endl;

    GameBoard game;
    char currentPlayer = PLAYER_X;

    while (true) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Error: Unable to accept connection" << endl;
            continue;
        }

        char buffer[1024] = { 0 };
        recv(clientSocket, buffer, sizeof(buffer), 0);

        string command(buffer);
        string response;

        if (command == "display") {
            response = game.display();
        }
        else if (command.rfind("move", 0) == 0) {
            int x, y;
            sscanf_s(command.c_str(), "move %d %d", &x, &y);

            if (game.makeMove(x, y, currentPlayer)) {
                if (game.checkWin(x, y, currentPlayer)) {
                    response = "win\n";
                    cout << "Player " << currentPlayer << " wins!" << endl;
                    closesocket(clientSocket);
                    break;
                }
                else {
                    response = "ok\n";
                    currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
                }
            }
            else {
                response = "invalid\n";
            }
        }
        else {
            response = "unknown command\n";
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    runServer(12345);
    return 0;
}