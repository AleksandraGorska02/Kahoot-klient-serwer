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
    std::string clientLogin;

public:
    std::string clientAnswer;

public:
    double clientTime;

public:
    std::chrono::time_point<std::chrono::high_resolution_clock> clientTimeStart;

public:
    std::chrono::time_point<std::chrono::high_resolution_clock> clientTimeEnd;

public:
    bool gameMaster;
public:
    int gameCode;

public:
    Client()
    {
        clientSocket = 0;

        clientScore = 0;
        clientLogin = "";
        clientAnswer = "";
        gameMaster = false;
        gameCode = 0;
    };
};
std::string secondLine;
std::chrono::steady_clock::time_point alarmEndTime;
std::vector<Client> connectedClients; // Store connected clients
std::set<int> clientsReady;
std::string gameQuestion = "";
// lista loginów
std::vector<std::string> logins;
void startGame()
{
    std::cout << "Runda rozpoczęta\n";

   
    static std::ifstream file("example.txt"); // Otwórz plik
    static std::streampos lastPosition = 0;    // Ustaw pozycję na początku pliku

    std::string firstLine;

    // Move to the last position in the file
    file.seekg(lastPosition);

    // Read the first line
    if (std::getline(file, firstLine))
    {
        // Send the first line to all clients
        for (int i = 0; i < connectedClients.size(); ++i)
        {
            send(connectedClients[i].clientSocket, firstLine.c_str(), firstLine.size(), 0);
            std::cout << "Wysłano pierwszą linię do klienta: " << firstLine << std::endl;
            connectedClients[i].clientTimeStart = std::chrono::high_resolution_clock::now();
        }

        // Read the second line
        if (std::getline(file, secondLine))
        {
            std::cout << "Odczytano drugą linię z pliku: " << secondLine << std::endl;
        }
        else
        {
            std::cerr << "koniec pliku\n";
        }

        // Store the current position for the next call
        lastPosition = file.tellg();

        // Set an alarm for 10 seconds
        alarm(10);
    }
    else
    {
        std::cerr << "koniec pliku\n";
    }
}

void endRound()
{
    for (int i = 0; i < connectedClients.size(); ++i)
    {

        // pokaz dopowiedz klienta
        std::cout << "Otrzymano odpowiedź od klienta1: " << connectedClients[i].clientAnswer[0] << std::endl;
        std::cout << "poprawna odp: " << secondLine[0] << std::endl;
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
    std::cout << "Koniec rundy\n";
    startGame();
}



void givePoint(std::vector<Client> &connectedClients)
{

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
bool checkLogin(std::string login)
{
    for (int i = 0; i < logins.size(); i++)
    {
        if (logins[i] == login)
        {
            return false;
        }
    }
    return true;
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
    signal(
        SIGALRM, (__sighandler_t)[](int sig) {
            std::cout << "Czas minął\n";
            endRound();

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
            // czytaj wiadomosc od klienta tylko do znaku nowej lini wiec nie dawaj na sztywno 1024 jako rozmiar wiadomosci

            const int bufferSize = 1; // Read osne character at a time
            char buffer[bufferSize];

            std::string clientResponse1;
            ssize_t bytesRead = 0;

            while (true)
            {
                bytesRead = recv(clientSocket, buffer, bufferSize, 0);

                if (bytesRead == -1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // No more data to read
                        break;
                    }
                    else
                    {
                        // Handle error
                        std::cerr << "Error reading from client" << std::endl;
                        break;
                    }
                }
                else if (bytesRead == 0)
                {
                    // Connection closed by the client
                    std::cout << "Client " << clientSocket << " disconnected" << std::endl;
                 //przestan odbierac wiadomosci od tego klienta
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &epollEvent);
                    // zamknij gniazdo klienta
                    close(clientSocket);
                    // usun klienta z listy
                    for (int i = 0; i < connectedClients.size(); ++i)
                    {
                        if (connectedClients[i].clientSocket == clientSocket)
                        {
                            connectedClients.erase(connectedClients.begin() + i);
                            break;
                        }
                    }
                 

                    break;
                }
                else
                {
                    // Append the read character to the message
                    clientResponse1 += buffer[0];

                    // Check if the newline character is received
                    if (buffer[0] == '\n')
                    {
                        // Process the complete message
                        break;
                    }
                }
            }

            std::cout << "Otrzymano wiadomość od klienta: " << clientResponse1 << std::endl;
            if (clientResponse1[0] == 'o')
            {
                // Add the client to the set of ready clients
                clientsReady.insert(clientSocket);
                std::cout << "Client " << clientSocket << " is ready\n";
                if (clientsReady.size() == connectedClients.size())
                {
                    std::cout << "All clients are ready\n";
                    // All clients are ready, start the game
                    startGame();
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
            else if (clientResponse1[0] == 'L')
            {
                // login jest podany po znaku  "L" do znaku nowej linii
                std::string login = clientResponse1;
                login.erase(0, 1);
                login.erase(login.size() - 1, 1);
                std::cout << "login: " << login << std::endl;
                if (checkLogin(login))
                {
                    logins.push_back(login);
                    std::cout << "dodano login" << std::endl;
                    std::string loginOK = "ok";
                    send(clientSocket, loginOK.c_str(), loginOK.size(), 0);
                }
                else
                {
                    std::string loginNO = "NO";
                    send(clientSocket, loginNO.c_str(), loginNO.size(), 0);
                }
            }
            else if (clientResponse1[0] == 'P')
            {

                std::string question = clientResponse1;
                std::cout << "pytanie: " << question << std::endl;
                // wymaż wszytko co jest po znaku nowej linii
                // wyswieetl pytanie

                char answer = question[question.size() - 2];
                // wymaz znak nowej lini i ostatnia litere pytrania
                question.erase(question.size() - 2, 2);

                std::cout << "pytanie: " << question << " odpowiedz: " << answer << std::endl;
                //sprawdz czy klient ma przypisany kod gry jezeli nie to przypisz mu
                int gameCode;
                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].clientSocket == clientSocket)
                    {
                        if (connectedClients[i].gameCode == 0)
                        {
                            //kod to 4 losowe cyfry
                            srand(time(NULL));
                            gameCode = rand() % 9000 + 1000;
                            connectedClients[i].gameCode = gameCode;

                        }
                        break;
                    }
                }
                std::string gameCodeString = std::to_string(gameCode);
                std::string filename = std::to_string(gameCode) + ".txt";


                
                // zapisz pytanie na koncu pliku tekstowego jak nie istnieje to go stworz
                
                std::ofstream file(filename, std::ios_base::app);
                if (file.is_open())
                {
                    file << question << std::endl;
                    file << answer << std::endl;
                    file.close();
                }
                else
                {
                    std::cout << "Unable to open file";
                }
            }
            //nadanie roli gamemastera klientowi
            else if (clientResponse1[0] == 'G')
            {
                std::cout << "klient " << clientSocket << " jest gamemasterem" << std::endl;
                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].clientSocket == clientSocket)
                    {
                        connectedClients[i].gameMaster = true;
                        break;
                    }
                }
                //przejscie do tworzenia gry i wyslanie c do klienta
                std::string C = "C";
                send(clientSocket, C.c_str(), C.size(), 0);
            }
            //wyslij klientowi dostep do ekranu logowania
            else if (clientResponse1[0] == 'g')
            {
                std::string L = "L";
                send(clientSocket, L.c_str(), L.size(), 0);
                //wyslij klientowi dostep do ekranu logowania
                std::cout << "wyslano L do klienta" << std::endl;
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