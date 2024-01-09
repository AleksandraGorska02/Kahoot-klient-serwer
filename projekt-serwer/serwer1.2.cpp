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
#include <thread>
#include <map>
#include <future>
#include <algorithm>

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
    std::chrono::high_resolution_clock::time_point clientTimeStart;

public:
    std::chrono::time_point<std::chrono::high_resolution_clock> clientTimeEnd;

public:
    bool gameMaster;

public:
    int gameCode;

public:
    int gameMasterQuestionCounter;

public:
    int clientPoints;

public:
    Client()
    {
        clientSocket = 0;

        clientPoints = 0;

        clientAnswer = "";

        gameMaster = false;
        gameCode = 0;
        gameMasterQuestionCounter = 0;
    };
};
// klasa gry
class Game
{
public:
    int gameCode;

public:
    std::map<int, Client> connectedClients;

public:
    std::set<int> clientsReady;

public:
    std::string secondLine;

public:
    std::streampos lastPosition;

public:
    Game()
    {
        gameCode = 0;

        secondLine = "";
        lastPosition = 0;
    };
    void startGame()
    {
        std::cout << "Runda rozpoczęta\n";
        int gameCode = this->gameCode;
        std::string gameCodeString = std::to_string(gameCode);
        std::string filename = std::to_string(gameCode) + ".txt";
        std::cout << "nazwa pliku: " << filename << std::endl;
        std::fstream file(filename, std::ios::in | std::ios::out | std::ios::app);
        // Otwórz plik
        // Ustaw pozycję na początku pliku

        std::string firstLine;

        // Move to the last position in the file
        file.seekg(this->lastPosition);

        // Read the first line
        if (std::getline(file, firstLine))
        {
            // Send the first line to all clients pętla foreach
            for (auto &entry : this->connectedClients)
            {

                auto &client = entry.second; // Access the client object from the map entry
                // dodanie znaku nowej linii do pytania
                if (client.gameMaster == false)
                {
                    firstLine += "\n";
                    std::cout << "Wysłano pytanie do klienta: " << firstLine << std::endl;
                    send(client.clientSocket, firstLine.c_str(), firstLine.size(), 0);
                    // zapisz czas rozpoczecia pytania
                    client.clientTimeStart = std::chrono::high_resolution_clock::now();
                }
            }
            // Read the second line
            if (std::getline(file, secondLine))
            {
                std::cout << "Odczytano drugą linię z pliku: " << secondLine << std::endl;
            }
            else
            {
                std::cerr << "brak drugiej lini\n";
            }

            // Store the current position for the next call
            this->lastPosition = file.tellg();
            // zamknij plik
            file.close();
            // Set an alarm for 10 s
        }
        else
        {
            std::cerr << " brak pierwszej\n";
        }

        // Close the file
        file.close();
        (void)std::async(std::launch::async, [this]()
                         {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        endRound(); });
    };

    void endRound()
    {
        std::cout << "Koniec rundy\n";
        // Send the second line to all clients
        buildRanking();
        for (const auto &entry : this->connectedClients)
        {
            const auto &client = entry.second;
            if (client.gameMaster == false)
            {
                // pokaz dopowiedz klienta
                std::cout << "Otrzymano odpowiedź od klienta1: " << client.clientAnswer[0] << std::endl;
                std::cout << "poprawna odp: " << secondLine[0] << std::endl;

                if (client.clientAnswer[0] == secondLine[0])
                {
                    std::cout << "Odpowiedź klienta jest poprawna!\n";
                    // pokaz czas klienta
                    std::cout << "Czas klienta: " << client.clientTime << " seconds" << std::endl;
                    std::cout << "Czas początkowy klienta: " << client.clientTimeStart.time_since_epoch().count() << " ns" << std::endl;
                    std::cout << "Czas końcowy klienta: " << client.clientTimeEnd.time_since_epoch().count() << " ns" << std::endl;
                    std::cout << "login w endround: " << client.clientLogin << std::endl;
                    std::string poprawna = "Odpowiedź klienta jest poprawna!\n";
                    send(client.clientSocket, poprawna.c_str(), poprawna.size(), 0);
                }
                else
                {
                    std::cout << "Odpowiedź klienta jest niepoprawna.\n";
                    // pokaz czas klienta
                    std::cout << "Czas klienta: " << client.clientTime << std::endl;
                    std::cout << "login w endround: " << client.clientLogin << std::endl;
                    std::string niepoprawna = "Odpowiedź klienta jest niepoprawna.\n";
                    send(client.clientSocket, niepoprawna.c_str(), niepoprawna.size(), 0);
                }

                // zerowanie odpowiedzi i czasu klienta

                const_cast<Client &>(client).clientAnswer.clear();
                const_cast<Client &>(client).clientTime = 0;
            }
        }
        std::cout << "Koniec rundy\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
        startGame();
    };
    void buildRanking()
    {
        std::cout << "Budowanie rankingu\n";
        // Sort the clients by their time
        std::vector<Client> sortedClients;
        for (const auto &entry : this->connectedClients)
        {
            sortedClients.push_back(entry.second);
        }
        std::sort(sortedClients.begin(), sortedClients.end(), [](const Client &a, const Client &b)
                  { return a.clientTime < b.clientTime; });
        //dla posortowanego wektora zmnien punkty na 0
        for (int i = 0; i < static_cast<int>(sortedClients.size()); ++i)
        {
            sortedClients[i].clientPoints = 0;
        }

        // teraz sprawdz czy odpowiedzi są poprawne jezeli tak to daj pierwszemu 3 punkty 2 drugiemu, a pozostałym z poprawnymi daj 1 punkt
        for (int i = 0; i < static_cast<int>(sortedClients.size()); ++i)
        {
            if (sortedClients[i].clientAnswer[0] == secondLine[0])
            {
                std::cout<<"dodano punkty"<<sortedClients[i].clientLogin<<"odp"<<sortedClients[i].clientAnswer[0]<<std::endl;
                std::cout<<"poprawna odp"<<secondLine[0]<<std::endl;
                sortedClients[i].clientPoints = +1;
            }
        }
        int counter = 0;
        for (int i = 0; i < static_cast<int>(sortedClients.size()); ++i)
        {
            if (sortedClients[i].clientAnswer[0] == secondLine[0])
            {

                counter++;
                if (counter == 1)
                {
                    sortedClients[i].clientPoints = 3;
                    continue;
                }
                if (counter == 2)
                {
                    sortedClients[i].clientPoints = 2;
                    break;
                }
            }
        }
        // przeiteruj po klientach i dodaj punkty do ich kont w klasie game nie uzywając games bo ni emamy do tego dostepu
        for (int i = 0; i < static_cast<int>(sortedClients.size()); ++i)
        {
            for (const auto &entry : this->connectedClients)
            {
                if (entry.second.clientLogin == sortedClients[i].clientLogin)
                {
                    const_cast<Client &>(entry.second).clientPoints += sortedClients[i].clientPoints;
                }
            }
        }
            // wyswietlanie punktow z mapy mapy gier
            for (const auto &entry : this->connectedClients)
            {
                std::cout << "login: " << entry.second.clientLogin << " punkty: " << entry.second.clientPoints << std::endl;
            }
     
    };
   
};

std::chrono::steady_clock::time_point alarmEndTime;

std::map<int, Client> connectedClients; // Store connected clients
std::set<int> clientsReady;

// lista loginów
std::vector<std::string> logins;
// mapa gier i ich kodów
std::map<int, Game> games;

bool checkLogin(std::string login)
{
    for (auto it = logins.begin(); it != logins.end(); ++it)
    {
        if (*it == login)
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

    while (true)
    {
        int nfds = epoll_wait(epollFd, &epollEvent, 1, -1);
        std::cout << "nfds: " << nfds << std::endl;
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

            // Add the new client to the list of connected clients
            connectedClients.insert(std::pair<int, Client>(clientSocket, newClient));

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
                    // przestan odbierac wiadomosci od tego klienta
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, &epollEvent);
                    // zamknij gniazdo klienta
                    close(clientSocket);
                    // usun klienta z listy
                    connectedClients.erase(clientSocket);

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

            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            if (clientResponse1[0] == 'o')
            {
                // Add the client to the set of ready clients
                clientsReady.insert(clientSocket);
                std::cout << "Client " << clientSocket << " is ready\n";
                int gameCode;
                // dodaj klienta do aktywnych graczy w konkretnej grze i wyslij ilosc aktywnych do gamemastera
                std::cout << "dodano klienta do gry" << std::endl;
                games[connectedClients[clientSocket].gameCode].clientsReady.insert(clientSocket);
                gameCode = games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].gameCode;
                gameCode = connectedClients[clientSocket].gameCode;
                // wyslij ilosc aktywnych do gamemastera ktorego kod gry jest rowny kodowi gry klienta
                std::cout << "wyslano ilosc aktywnych do gamemastera" << std::endl;
                // nadaj login klientowi
                std::string login = connectedClients[clientSocket].clientLogin;
                games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].clientLogin = login;

                // znajdz gamemastera
                // znajdz gamemastera klienta o podanym kodzie gry
                for (int i = 0; i < static_cast<int>(connectedClients.size()); ++i)
                {
                    if (connectedClients[i].gameCode == gameCode && connectedClients[i].gameMaster == true)
                    {
                        // wyslij do gamemastera ilosc aktywnych graczy
                        std::cout << "wyslano ilosc aktywnych do gamemastera" << std::endl;
                        std::string activePlayers = "GIlosc aktywnych graczy:" + std::to_string(games[connectedClients[i].gameCode].clientsReady.size()) + "\n";
                        send(connectedClients[i].clientSocket, activePlayers.c_str(), activePlayers.size(), 0);
                        int clientSocket1 = connectedClients[i].clientSocket;
                        games[connectedClients[clientSocket1].gameCode].connectedClients[clientSocket1].gameMaster = true;
                        break;
                    }
                }
            }

            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if (clientResponse1[0] == 'n')
            {
                // Remove the client from the set of ready clients
                clientsReady.erase(clientSocket);
                std::cout << "Client " << clientSocket << " is not ready\n";
            }

            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if (clientResponse1[0] == 'a' || clientResponse1[0] == 'b' || clientResponse1[0] == 'c' || clientResponse1[0] == 'd')
            {
                std::string clientAnswer = clientResponse1;
                std::cout << "Client " << clientSocket << " answered: " << clientAnswer[0] << std::endl;

                // dodaj odpowiedz klienta do klienta

                games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].clientAnswer = clientAnswer;
                games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].clientTimeEnd = std::chrono::high_resolution_clock::now();
                auto time = std::chrono::duration_cast<std::chrono::seconds>(games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].clientTimeEnd - games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].clientTimeStart).count();
                games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].clientTime = static_cast<double>(time);
            }

            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if (clientResponse1[0] == 'L')
            {
                // login jest podany po znaku  "L" do znaku nowej linii
                std::string login = clientResponse1;
                login.erase(0, 1);
                login.erase(login.size() - 2, 2);
                std::cout << "login: " << login << std::endl;
                if (checkLogin(login))
                {
                    std::cout << "login jest ok" << std::endl;
                    // dodaj login do listy

                    logins.push_back(login);

                    std::string loginOK = "X\n";
                    send(clientSocket, loginOK.c_str(), loginOK.size(), 0);
                    // dodaj login do klienta
                    connectedClients[clientSocket].clientLogin = login;
                }
                else
                {
                    std::string loginNO = "NO\n";
                    send(clientSocket, loginNO.c_str(), loginNO.size(), 0);
                }
            }
            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if (clientResponse1[0] == 'X')
            {
                // klient przesłał kod gry
                std::string gameCodeString = clientResponse1;
                gameCodeString.erase(0, 1);
                gameCodeString.erase(gameCodeString.size() - 1, 1);
                int gameCode = std::stoi(gameCodeString);
                std::cout << "kod gry: " << gameCode << std::endl;
                // sprawdz czy istnieje gra o takim kodzie
                if (games.count(gameCode) != 0)
                {
                    // dodaj klienta do gry

                    connectedClients[clientSocket].gameCode = gameCode;
                    // dodaj klienta do gry
                    games[gameCode].connectedClients.insert(std::pair<int, Client>(clientSocket, connectedClients[clientSocket]));
                    // pokaz kod gry
                    std::cout << "kod gry: " << games[gameCode].gameCode << std::endl;
                    // wyslij klientowi informacje o tym ze jest w grze

                    std::cout << "wyslano ok do klienta" << std::endl;
                    std::string gameCodeOK = "ok\n";
                    send(clientSocket, gameCodeOK.c_str(), gameCodeOK.size(), 0);
                }
                else
                {
                    std::string gameCodeNO = "Y\n";
                    send(clientSocket, gameCodeNO.c_str(), gameCodeNO.size(), 0);
                }
            }
            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
                // sprawdz czy klient ma przypisany kod gry jezeli nie to przypisz mu
                int gameCode;

                connectedClients[clientSocket].gameMasterQuestionCounter++;
                if (connectedClients[clientSocket].gameMasterQuestionCounter == 5)
                {
                    // wyslij do gamemastera informacje o tym ze pytanie zostalo dodane
                    std::cout << "wyslano Q do gamemastera" << std::endl;
                    std::string questionAdded = "Kod gry:" + std::to_string(connectedClients[clientSocket].gameCode) + "\n";
                    send(connectedClients[clientSocket].clientSocket, questionAdded.c_str(), questionAdded.size(), 0);
                    connectedClients[clientSocket].gameMasterQuestionCounter = 0;
                }
                if (connectedClients[clientSocket].gameCode == 0)
                {
                    // kod to 4 losowe cyfry
                    srand(time(NULL));
                    gameCode = rand() % 9000 + 1000;
                    connectedClients[clientSocket].gameCode = gameCode;

                    // stwoz gre
                    Game game;
                    game.gameCode = gameCode;
                    // dodaj gre do mapy gier
                    games.insert(std::pair<int, Game>(gameCode, game));
                    games[gameCode].connectedClients[clientSocket].gameCode = gameCode;
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
                // wyslanie do game mastera informacji o tym ze pytanie zostalo dodane po dobiciu countera na 5
            }

            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            // nadanie roli gamemastera klientowi

            else if (clientResponse1[0] == 'G')
            {
                std::cout << "klient " << clientSocket << " jest gamemasterem" << std::endl;
                // nadaj klientowi role gamemastera
                connectedClients[clientSocket].gameMaster = true;
                games[connectedClients[clientSocket].gameCode].connectedClients[clientSocket].gameMaster = true;

                // przejscie do tworzenia gry i wyslanie c do klienta
                std::string C = "C\n";
                send(clientSocket, C.c_str(), C.size(), 0);
            }

            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            // wyslij klientowi dostep do ekranu logowania
            else if (clientResponse1[0] == 'g')
            {
                std::string L = "L\n";
                send(clientSocket, L.c_str(), L.size(), 0);
                // wyslij klientowi dostep do ekranu logowania
                std::cout << "wyslano L do klienta" << std::endl;
            }
            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if (clientResponse1[0] == 'S')
            {
                // sprawdz czy klient jest gamemasterem

                // rozpocznij gre na nowym watku
                std::thread t(&Game::startGame, &games[connectedClients[clientSocket].gameCode]);
                t.detach();

                std::cout << "rozpoczeto gre " << std::to_string(games[connectedClients[clientSocket].gameCode].gameCode) << std::endl;
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