#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Utwórz gniazdo klienta
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Błąd przy tworzeniu gniazda klienta\n";
        return -1;
    }

    // Skonfiguruj strukturę adresową serwera
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // Użyj tego samego portu co serwer
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr));

    // Połącz się z serwerem
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Błąd przy połączeniu z serwerem\n";
        close(clientSocket);
        return -1;
    }

    std::cout << "Połączono z serwerem\n";

    // Tutaj możesz dodać kod do wysyłania i odbierania danych od serwera

    // Zamknij gniazdo klienta
    close(clientSocket);

    return 0;
}
