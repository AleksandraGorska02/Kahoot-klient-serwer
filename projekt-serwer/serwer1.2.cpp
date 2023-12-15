#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // Utwórz gniazdo serwera
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Błąd przy tworzeniu gniazda serwera\n";
        return -1;
    }

    // Skonfiguruj strukturę adresową serwera
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(10013); // Możesz zmienić port
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Zwiąż gniazdo z adresem
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Błąd przy wiązaniu gniazda z adresem\n";
        close(serverSocket);
        return -1;
    }

    // Nasłuchuj na połączenia
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Błąd przy nasłuchiwaniu na połączenia\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Serwer nasłuchuje na porcie 12345...\n";

    while (true)
    {
        sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);
        if (clientSocket == -1)
        {
            std::cerr << "Błąd przy akceptowaniu połączenia\n";
            close(serverSocket);
            return -1;
        }

        std::cout << "Połączenie zaakceptowane\n";

        // Odczytaj pierwszą linię z pliku tekstowego
        std::ifstream file("example.txt");
        std::string firstLine;
        if (std::getline(file, firstLine))
        {
            // Wyślij pierwszą linię do klienta
            send(clientSocket, firstLine.c_str(), firstLine.size(), 0);
            std::cout << "Wysłano pierwszą linię do klienta: " << firstLine << std::endl;
        }
        else
        {
            std::cerr << "Błąd przy odczycie pliku\n";
        }

        std::string secondLine;
        if (std::getline(file, secondLine))
        {
            std::cout << "Odczytano drugą linię z pliku: " << secondLine << std::endl;

            // Poczekaj na odpowiedź od klienta
            char clientResponse[1024];
            recv(clientSocket, clientResponse, sizeof(clientResponse), 0);
            int recvSize = strlen(clientResponse);

            std::cout << "Otrzymano odpowiedź od klienta: " << clientResponse[0] << std::endl;

            // Sprawdź odpowiedź
            if (secondLine[0] == clientResponse[0])
            {
                std::cout << "Odpowiedź klienta jest poprawna!\n";
            }
            else
            {
                std::cout << "Odpowiedź klienta jest niepoprawna.\n";
            }
        }
        else
        {
            std::cerr << "Błąd przy odczycie pliku\n";
        }
        close(clientSocket);
    }

    // Zamknij gniazdo serwera i klienta
    close(serverSocket);
    std::cout << "Zamknięto gniazdo serwera\n";

    std::cout << "Zamknięto gniazdo klienta\n";
    return 0;
}
