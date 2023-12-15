#include <iostream>
#include <fstream>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>

struct QuestionAnswer {
    std::string question;
    std::string answer;
};

int main() {
    // tworzy gniazdo serwera
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Błąd przy tworzeniu gniazda serwera\n";
        return -1;
    }

    // struktura adresowa serwera
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888); 
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        std::cerr << "Błąd przy ustawianiu opcji SO_REUSEADDR\n";
        close(serverSocket);
        return -1;
    }


    // wiaze gniazdo z adresem
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Błąd przy wiązaniu gniazda z adresem\n";
        close(serverSocket);
        return -1;
    }

    // czeka na połączenia
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Błąd przy nasłuchiwaniu na połączenia\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Serwer nasłuchuje na porcie 12345...\n";

    // akceptuje połączenia od klientów
    sockaddr_in clientAddress;
    socklen_t clientSize = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "Błąd przy akceptowaniu połączenia\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Połączenie zaakceptowane\n";

    // Odczyt pytan i odpowiedzi z pliku
    std::ifstream file("example.txt");
    std::vector<QuestionAnswer> questions;

    std::string line;
    while (std::getline(file, line)) {
        QuestionAnswer qa;
        qa.question = line;

        if (std::getline(file, line)) {
            qa.answer = line;
            questions.push_back(qa);
        } else {
            std::cerr << "Błąd przy odczycie pliku: brak odpowiedzi\n";
            break;
        }
    }

    // Przesyła pytania i sprawdzaj odpowiedzi
    for (const auto& qa : questions) {
        std::cout << "Wysyłanie pytania do klienta: " << qa.question << std::endl;
        send(clientSocket, qa.question.c_str(), qa.question.size(), 0);
        // Wysyla pytanie do klienta
        send(clientSocket, qa.question.c_str(), qa.question.size(), 0);
        std::cout << "Wysłano pytanie do klienta: " << qa.question << std::endl;

        // czeka na odpowiedź od klienta
        char clientResponse[1024];
        recv(clientSocket, clientResponse, sizeof(clientResponse), 0);

        // Sprawdza odpowiedź
        if (qa.answer == clientResponse) {
            std::cout << "Odpowiedź klienta jest poprawna!\n";
        } else {
            std::cout << "Odpowiedź klienta jest niepoprawna. Poprawna odpowiedź: " << qa.answer << std::endl;
        }
    }

    // zamyka gniazdo serwera i klienta
    close(serverSocket);
    close(clientSocket);

    return 0;
}
