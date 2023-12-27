#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <set>

#define MAX_EVENTS 10

class Client
{ //klasa klienta
public: int clientSocket;
    public: int clientID;
    public: int clientScore;
    public: std::string clientName;
    public: std::string clientAnswer;
    public: float clientTime;
    public: Client(){
        clientSocket = 0;
        clientID = 0;
        clientScore = 0;
        clientName = "";
        clientAnswer = "";
    };
};



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
    serverAddress.sin_port = htons(12128); // Możesz zmienić port
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

    // Ustaw epoll
    int epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        std::cerr << "Błąd przy tworzeniu epoll\n";
        close(serverSocket);
        return -1;
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = serverSocket;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) == -1)
    {
        std::cerr << "Błąd przy dodawaniu gniazda do epoll\n";
        close(serverSocket);
        close(epollFd);
        return -1;
    }

    std::cout << "Serwer nasłuchuje na porcie 12345...\n";
    std::vector<Client> connectedClients; // Store connected clients
    std::set<int> clientsReady;           // Set to track clients that are ready

    while (true)
    {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            std::cerr << "Błąd przy epoll_wait\n";
            break;
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == serverSocket)
            {
                // New connection
                sockaddr_in clientAddress;
                socklen_t clientSize = sizeof(clientAddress);
                int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);
                if (clientSocket == -1)
                {
                    std::cerr << "Błąd przy akceptowaniu połączenia\n";
                    continue;
                }

                // Create a new instance of the Client class
                Client newClient;
                newClient.clientSocket = clientSocket;

                std::cout << "Połączenie zaakceptowane\n";

                // Add the new client to the list
                connectedClients.push_back(newClient);

                // Add the new client's socket to epoll
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientSocket;

                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
                {
                    std::cerr << "Błąd przy dodawaniu klienta do epoll\n";
                    close(clientSocket);
                }
            }
            else
            {
                // Handle data from the client
                int clientSocket = events[i].data.fd;

                char clientResponse1[1024];
                recv(clientSocket, clientResponse1, sizeof(clientResponse1), 0);

                if (clientResponse1[0] == 'o')
                {
                    // Add the client to the set of ready clients
                    clientsReady.insert(clientSocket);
                    std::cout << "Client " << clientSocket << " is ready\n";
                }
                else if (clientResponse1[0] == 'n')
                {
                    // Remove the client from the set of ready clients
                    clientsReady.erase(clientSocket);
                    std::cout << "Client " << clientSocket << " is not ready\n";
                }
            }
        }
        if(clientsReady.size() == connectedClients.size()){
            std::cout << "All clients are ready\n";
        
            // Send the first line to all clients
            break;

        }
    }
    while(true){
        std::ifstream file("example.txt");
        std::string firstLine;

        if (std::getline(file, firstLine))
        {
            // Send the first line to all clients
            for (int i = 0; i < connectedClients.size(); ++i)
            {
                send(connectedClients[i].clientSocket, firstLine.c_str(), firstLine.size(), 0);
                std::cout << "Wysłano pierwszą linię do klienta: " << firstLine << std::endl;
            }
        }
        else
        {
            std::cerr << "Błąd przy odczycie pliku\n";
        }

        std::string secondLine;
        if (std::getline(file, secondLine))
        {
            std::cout << "Odczytano drugą linię z pliku: " << secondLine << std::endl;

            // Wait for a response from the client
            for (int i = 0; i < connectedClients.size(); ++i)
            {
                char clientResponse[1024];
                recv(connectedClients[i].clientSocket, clientResponse, sizeof(clientResponse), 0);

                std::cout << "Otrzymano odpowiedź od klienta: " << clientResponse[0] << std::endl;

                // Check the response
                if (secondLine[0] == clientResponse[0])
                {
                    std::cout << "Odpowiedź klienta jest poprawna!\n";
                    std::string poprawna = "Odpowiedź klienta jest poprawna!\n";
                    send(connectedClients[i].clientSocket, poprawna.c_str(), poprawna.size(), 0);
                }
                else
                {
                    std::cout << "Odpowiedź klienta jest niepoprawna.\n";
                    //wyswietl odpowiedz i klienta
                    std::cout << "Odpowiedź klienta: " << clientResponse[0] << "klient: " << connectedClients[i].clientSocket << "\n";

                    std::string niepoprawna = "Odpowiedź klienta jest niepoprawna.\n";
                    send(connectedClients[i].clientSocket, niepoprawna.c_str(), niepoprawna.size(), 0);
                }
            }
        }
        else
        {
            std::cerr << "Błąd przy odczycie pliku\n";
        }

        break;
    }

    /*
    std::vector<int> activeClients;
    while (true)
    {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            std::cerr << "Błąd przy epoll_wait\n";
            break;
        }

        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == serverSocket)
            {
                
            // Nowe połączenie
            sockaddr_in clientAddress;
            socklen_t clientSize = sizeof(clientAddress);
            int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);
            if (clientSocket == -1)
            {
                std::cerr << "Błąd przy akceptowaniu połączenia\n";
                continue;
            }

            // Create a new instance of the Client class
            Client newClient;
            
            newClient.clientSocket = clientSocket;

            std::cout << "Połączenie zaakceptowane\n";

            // Dodaj nowe gniazdo do epoll
            event.events = EPOLLIN | EPOLLET;
            event.data.fd = clientSocket;

            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
            {
                std::cerr << "Błąd przy dodawaniu klienta do epoll\n";
                close(clientSocket);
            }

            // Add the new client to the list
            activeClients.push_back(clientSocket);
            }
            else
            {
                // Obsługa danych od klienta
                int clientSocket = events[i].data.fd;

                // Odczytaj pierwszą linię z pliku tekstowego
                char clientResponse1[1024];
                recv(clientSocket, clientResponse1, sizeof(clientResponse1), 0);
                if (clientResponse1[0] == 'o')
                {

                    std::ifstream file("example.txt");
                    std::string firstLine;

                    if (std::getline(file, firstLine))
                    {
                        sleep(5);
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

                        std::cout << "Otrzymano odpowiedź od klienta: " << clientResponse[0] << std::endl;

                        // Sprawdź odpowiedź
                        if (secondLine[0] == clientResponse[0])
                        {
                            std::cout << "Odpowiedź klienta jest poprawna!\n";
                            std::string poprawna = "Odpowiedź klienta jest poprawna!\n";
                            send(clientSocket, poprawna.c_str(), poprawna.size(), 0);
                        }
                        else
                        {
                            std::cout << "Odpowiedź klienta jest niepoprawna.\n";

                            std::string niepoprawna = "Odpowiedź klienta jest niepoprawna.\n";
                            send(clientSocket, niepoprawna.c_str(), niepoprawna.size(), 0);
                        }
                    }
                    else
                    {
                        std::cerr << "Błąd przy odczycie pliku\n";
                    }

                    // Usuń klienta z epoll
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, NULL);
                    close(clientSocket);
                    std::cout << "Zamknięto gniazdo klienta: " << clientSocket << std::endl;
                }
            }
        }
    }
*/
    // Zamknij gniazdo serwera
    close(serverSocket);
    std::cout << "Zamknięto gniazdo serwera\n";

    // Zamknij epoll
    close(epollFd);

    return 0;
}