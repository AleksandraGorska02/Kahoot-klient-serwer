#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

int main() {
    // tworzy gniazdo klienta
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Błąd przy tworzeniu gniazda klienta\n";
        return -1;
    }

    // konfiguruje strukturę adresową serwera
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888); // Użyj tego samego portu co serwer
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr));

    // łączy z serwerem
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Błąd przy połączeniu z serwerem\n";
        close(clientSocket);
        return -1;
    }

    std::cout << "Połączono z serwerem\n";

    // Pobiera pytania od serwera i wysyłaj odpowiedzi
    while (true) {
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Odbiera pytanie od serwera
        char question[1024];
        int bytesRead = recv(clientSocket, question, sizeof(question), 0);
        if (bytesRead <= 0) {
            std::cerr << "Błąd przy odbieraniu pytania od serwera\n";
            break;
        }

        std::cout << "Otrzymano pytanie od serwera: " << question << std::endl;

        // Wprowadzanie odpowiedzi z klawiatury
        std::cout << "Wprowadź odpowiedź: ";
        std::string response;
        std::getline(std::cin, response);

        // Wyślij odpowiedź do serwera
        send(clientSocket, response.c_str(), response.size(), 0);
    }

    // Zamknij gniazdo klienta
    close(clientSocket);

    return 0;
}
