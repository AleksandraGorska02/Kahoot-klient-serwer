#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <set>
#include <ctime>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <chrono>
#include <csignal>

#define MAX_EVENTS 10

class Client
{ // klasa klienta
public:
    int clientSocket;

public:
    int clientScore;

public:
    std::string clientName;

public:
    std::string clientAnswer;

public:
    double clientTime;

public:
    std::chrono::time_point<std::chrono::high_resolution_clock> clientTimeStart;

public:
    std::chrono::time_point<std::chrono::high_resolution_clock> clientTimeEnd;

public:
    Client()
    {
        clientSocket = 0;

        clientScore = 0;
        clientName = "";
        clientAnswer = "";
    };
};
std::string secondLine;
std::chrono::steady_clock::time_point alarmEndTime;
  std::vector<Client> connectedClients; // Store connected clients
    std::set<int> clientsReady;      
void endThisGame()
{
    for (int i = 0; i < connectedClients.size(); ++i)
    {

        // pokaz dopowiedz klienta
        std::cout << "Otrzymano odpowiedź od klienta1: " << connectedClients[i].clientAnswer[0] << std::endl;
        std::cout<<"poprawna odp: "<<secondLine[0]<<std::endl;
        if (connectedClients[i].clientAnswer[0] == secondLine[0])
        {
            std::cout << "Odpowiedź klienta jest poprawna!\n";
            // pokaz czas klienta
            std::cout << "Czas klienta: " << connectedClients[i].clientTime << " seconds" << std::endl;
            std::cout << "Czas początkowy klienta: " << connectedClients[i].clientTimeStart.time_since_epoch().count() << " ns" << std::endl;
            std::cout << "Czas końcowy klienta: " << connectedClients[i].clientTimeEnd.time_since_epoch().count() << " ns" << std::endl;
            std::string poprawna = "Odpowiedź klienta jest poprawna!\n";

            send(connectedClients[i].clientSocket, poprawna.c_str(), poprawna.size(), 0);
        }
        else
        {
            std::cout << "Odpowiedź klienta jest niepoprawna.\n";
            // pokaz czas klienta
            std::cout << "Czas klienta: " << connectedClients[i].clientTime << std::endl;

            std::string niepoprawna = "Odpowiedź klienta jest niepoprawna.\n";
            send(connectedClients[i].clientSocket, niepoprawna.c_str(), niepoprawna.size(), 0);
        }
    }
}


void startGame(std::vector<Client> &connectedClients, std::string &secondLine)
{
    std::cout << "Gra rozpoczęta\n";

    std::cout << "All clients are ready\n";

    std::ifstream file("example.txt");
    std::string firstLine;
    if (std::getline(file, firstLine))
    {
        // Send the first line to all clients
        for (int i = 0; i < connectedClients.size(); ++i)
        {
            send(connectedClients[i].clientSocket, firstLine.c_str(), firstLine.size(), 0);
            std::cout << "Wysłano pierwszą linię do klienta: " << firstLine << std::endl;
            connectedClients[i].clientTimeStart = std::chrono::high_resolution_clock::now();
        }
        if (std::getline(file, secondLine))
        {
            std::cout << "Odczytano drugą linię z pliku: " << secondLine << std::endl;
        }
        else
        {
            std::cerr << "Błąd przy odczycie pliku\n";
        }
    alarm(10);
    }
}


void givePoint(std::vector<Client> &connectedClients){
    for (int i = 0; i < connectedClients.size(); ++i)
    {

       if (connectedClients[i].clientAnswer[0] == secondLine[0])
        {
            
        }
        else
        {
           
        }
    }


}
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
    serverAddress.sin_port = htons(12135); // Możesz zmienić port
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

    epoll_event epollEvent;
    epollEvent.events = EPOLLIN;
    epollEvent.data.fd = serverSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &epollEvent) == -1) // dodaj gniazdo do epoll
    {
        std::cerr << "Błąd przy dodawaniu gniazda do epoll\n";
        close(serverSocket);
        close(epollFd);
        return -1;
    }

    std::cout << "Serwer nasłuchuje na porcie 12345...\n";
       // Set to track clients that are ready
    signal(SIGALRM, (__sighandler_t)[](int sig) {
        std::cout << "Czas minął\n";
        endThisGame();
        alarm(10);
    });

    while (true)
    {
        int nfds = epoll_wait(epollFd, &epollEvent, 1, -1);

        if (epollEvent.events & EPOLLIN && epollEvent.data.fd == serverSocket)
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
            epollEvent.events = EPOLLIN;
            epollEvent.data.fd = clientSocket;

            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &epollEvent) == -1)
            {
                std::cerr << "Błąd przy dodawaniu klienta do epoll\n";
                close(clientSocket);
            }
        }
        else if (epollEvent.events & EPOLLIN && epollEvent.data.fd != serverSocket)
        {
            // Handle data from the client
            int clientSocket = epollEvent.data.fd;

            char clientResponse1[1024];
            recv(clientSocket, clientResponse1, sizeof(clientResponse1), 0);

            if (clientResponse1[0] == 'o')
            {
                // Add the client to the set of ready clients
                clientsReady.insert(clientSocket);
                std::cout << "Client " << clientSocket << " is ready\n";
                if (clientsReady.size() == connectedClients.size())
                {
                    std::cout << "All clients are ready\n";
                    // All clients are ready, start the game
                    startGame(connectedClients, secondLine);
                }
            }
            else if (clientResponse1[0] == 'n')
            {
                // Remove the client from the set of ready clients
                clientsReady.erase(clientSocket);
                std::cout << "Client " << clientSocket << " is not ready\n";
            }
            else if (clientResponse1[0] == 'a' || clientResponse1[0] == 'b' || clientResponse1[0] == 'c' || clientResponse1[0] == 'd')
            {
                std::string clientAnswer = clientResponse1;
                std::cout << "Client " << clientSocket << " answered: " << clientAnswer[0] << std::endl;

                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].clientSocket == clientSocket)
                    {
                        connectedClients[i].clientAnswer = clientAnswer;
                        connectedClients[i].clientTimeEnd = std::chrono::high_resolution_clock::now();
                        auto time = std::chrono::duration_cast<std::chrono::seconds>(connectedClients[i].clientTimeEnd - connectedClients[i].clientTimeStart).count();
                        connectedClients[i].clientTime = static_cast<double>(time);
                        break;
                    }
                }

            }
           
           
            else
            {
                std::cout << "Otrzymano nieznany komunikat od klienta\n";
            }
          
        }
    }

    // Zamknij gniazdo serwera
    close(serverSocket);
    std::cout << "Zamknięto gniazdo serwera\n";

    // Zamknij epoll
    close(epollFd);

    return 0;
}