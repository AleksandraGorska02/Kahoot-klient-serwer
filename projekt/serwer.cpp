#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Utwórz gniazdo serwera
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Błąd przy tworzeniu gniazda serwera\n";
        return -1;
    }

    // Skonfiguruj strukturę adresową serwera
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // Możesz zmienić port
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Zwiąż gniazdo z adresem
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Błąd przy wiązaniu gniazda z adresem\n";
        close(serverSocket);
        return -1;
    }

    // Nasłuchuj na połączenia
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Błąd przy nasłuchiwaniu na połączenia\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Serwer nasłuchuje na porcie 12345...\n";

    // Akceptuj połączenia od klientów
    sockaddr_in clientAddress;
    socklen_t clientSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "Błąd przy akceptowaniu połączenia\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Połączenie zaakceptowane\n";

    // Tutaj możesz dodać kod obsługujący komunikację z klientem

    // Zamknij gniazdo serwera i klienta
    close(serverSocket);
    close(clientSocket);

    return 0;
}
