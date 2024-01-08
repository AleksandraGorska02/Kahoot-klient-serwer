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
int gameMasterQuestionCounter;

public:
    Client()
    {
        clientSocket = 0;

        clientScore = 0;
        clientLogin = "";
        clientAnswer = "";
        gameMaster = false;
        gameCode = 0;
        gameMasterQuestionCounter = 0;
    
    };
};
//klasa gry
class Game
{
public:
    int gameCode;
    public:
    std::vector<Client> connectedClients;
    public:
    std::set<int> clientsReady;
    public:
    std::string secondLine;
    public:
     std::streampos lastPosition;
     //doaj deskryptor pliku



    
    public:
    Game(){
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
    std::cout<<"nazwa pliku: "<<filename<<std::endl;
std::fstream file(filename, std::ios::in | std::ios::out | std::ios::app);
   // Otwórz plik
     // Ustaw pozycję na początku pliku

    std::string firstLine;

    // Move to the last position in the file
    file.seekg(this->lastPosition);

    // Read the first line
    if (std::getline(file, firstLine))
    {
        // Send the first line to all clients
        for (int i = 0; i < this->connectedClients.size(); i++)
        {
            if (this->connectedClients[i].gameMaster == false)
            {
                send(this->connectedClients[i].clientSocket, firstLine.c_str(), firstLine.size(), 0);
                std::cout << "Wysłano pierwszą linię do klienta: " << firstLine << std::endl;
                this->connectedClients[i].clientTimeStart = std::chrono::high_resolution_clock::now();
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
        //zamknij plik
        file.close();
        // Set an alarm for 10 s
    }
    else
    {
        std::cerr << " brak pierwszej\n";
    }

    // Close the file
    file.close();
}

void endRound()
{
    for (int i = 0; i < this->connectedClients.size(); ++i)
    {

        // pokaz dopowiedz klienta
        std::cout << "Otrzymano odpowiedź od klienta1: " << this->connectedClients[i].clientAnswer[0] << std::endl;
        std::cout << "poprawna odp: " << secondLine[0] << std::endl;
        if (this->connectedClients[i].clientAnswer[0] == secondLine[0])
        {
            std::cout << "Odpowiedź klienta jest poprawna!\n";
            // pokaz czas klienta
            std::cout << "Czas klienta: " << this->connectedClients[i].clientTime << " seconds" << std::endl;
            std::cout << "Czas początkowy klienta: " << this->connectedClients[i].clientTimeStart.time_since_epoch().count() << " ns" << std::endl;
            std::cout << "Czas końcowy klienta: " << this->connectedClients[i].clientTimeEnd.time_since_epoch().count() << " ns" << std::endl;
            std::string poprawna = "Odpowiedź klienta jest poprawna!\n";

            send(this->connectedClients[i].clientSocket, poprawna.c_str(), poprawna.size(), 0);
        }
        else
        {
            std::cout << "Odpowiedź klienta jest niepoprawna.\n";
            // pokaz czas klienta
            std::cout << "Czas klienta: " << this->connectedClients[i].clientTime << std::endl;

            std::string niepoprawna = "Odpowiedź klienta jest niepoprawna.\n";
            send(this->connectedClients[i].clientSocket, niepoprawna.c_str(), niepoprawna.size(), 0);
        }
        //zerowanie odpowiedzi i czasu
        this->connectedClients[i].clientAnswer = "";
        connectedClients[i].clientTime = 0;
    }
    std::cout << "Koniec rundy\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    startGame();
}

};

std::chrono::steady_clock::time_point alarmEndTime;
std::vector<Client> connectedClients; // Store connected clients
std::set<int> clientsReady;

// lista loginów
std::vector<std::string> logins;
//mapa gier i ich kodów
std::map<int, Game> games;


void givePoint(std::vector<Client> &connectedClients)
{

 
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

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            if (clientResponse1[0] == 'o')
            {
                // Add the client to the set of ready clients
                clientsReady.insert(clientSocket);
                std::cout << "Client " << clientSocket << " is ready\n";
                int gameCode;
               //dodaj klienta do aktywnych graczy w konkretnej grze i wyslij ilosc aktywnych do gamemastera
                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].clientSocket == clientSocket)
                    {//dodano klienta do gry
                    std::cout<<"dodano klienta do gry"<<std::endl;
                        games[connectedClients[i].gameCode].clientsReady.insert(clientSocket);
                        gameCode = connectedClients[i].gameCode;
                        break;
                    }
                }
                //znajdz gamemastera klienta o podanym kodzie gry
                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].gameCode == gameCode && connectedClients[i].gameMaster == true)
                    {
                        //wyslij do gamemastera ilosc aktywnych graczy
                        std::cout << "wyslano ilosc aktywnych do gamemastera" << std::endl;
                        std::string activePlayers = "GIlosc aktywnych graczy:" + std::to_string(games[connectedClients[i].gameCode].clientsReady.size());
                        send(connectedClients[i].clientSocket, activePlayers.c_str(), activePlayers.size(), 0);
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
            
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
                    std::string loginOK = "X";
                    send(clientSocket, loginOK.c_str(), loginOK.size(), 0);
                }
                else
                {
                    std::string loginNO = "NO";
                    send(clientSocket, loginNO.c_str(), loginNO.size(), 0);
                }
            }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if(clientResponse1[0]=='X'){
                //klient przesłał kod gry
                std::string gameCodeString = clientResponse1;
                gameCodeString.erase(0, 1);
                gameCodeString.erase(gameCodeString.size() - 1, 1);
                int gameCode = std::stoi(gameCodeString);
                std::cout << "kod gry: " << gameCode << std::endl;
                //sprawdz czy istnieje gra o takim kodzie
                if(games.count(gameCode)!=0){
                    //dodaj klienta do gry
                    for (int i = 0; i < connectedClients.size(); ++i)
                    {
                        if (connectedClients[i].clientSocket == clientSocket)
                        {
                            connectedClients[i].gameCode = gameCode;
                            games[gameCode].connectedClients.push_back(connectedClients[i]);
                            //wyslij klientowi informacje o tym ze jest w grze
                            std::cout<<"wyslano ok do klienta"<<std::endl;
                            std::string gameCodeOK = "ok";
                            send(clientSocket, gameCodeOK.c_str(), gameCodeOK.size(), 0);
                            break;
                        }
                    }
                }
                else{
                    std::string gameCodeNO = "Y";
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
                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].clientSocket == clientSocket)
                    {
                        connectedClients[i].gameMasterQuestionCounter++;
                        if(connectedClients[i].gameMasterQuestionCounter==5){
                            //wyslij do gamemastera informacje o tym ze pytanie zostalo dodane
                            std::cout << "wyslano Q do gamemastera" << std::endl;
                            std::string questionAdded = "Kod gry:"+std::to_string(connectedClients[i].gameCode);
                            send(connectedClients[i].clientSocket, questionAdded.c_str(), questionAdded.size(), 0);
                            connectedClients[i].gameMasterQuestionCounter = 0;
                        }
                        if (connectedClients[i].gameCode == 0)
                        {
                            // kod to 4 losowe cyfry
                            srand(time(NULL));
                            gameCode = rand() % 9000 + 1000;
                            connectedClients[i].gameCode = gameCode;
                            //stwoz gre 
                            Game game;
                            game.gameCode = gameCode;
                            //dodaj gre do mapy gier
                            games.insert(std::pair<int, Game>(gameCode, game));
                            
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
                //wyslanie do game mastera informacji o tym ze pytanie zostalo dodane po dobiciu countera na 5
               
                    
                

            }
        
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            // nadanie roli gamemastera klientowi
            
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
                // przejscie do tworzenia gry i wyslanie c do klienta
                std::string C = "C";
                send(clientSocket, C.c_str(), C.size(), 0);
            }
            
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            // wyslij klientowi dostep do ekranu logowania
            else if (clientResponse1[0] == 'g')
            {
                std::string L = "L";
                send(clientSocket, L.c_str(), L.size(), 0);
                // wyslij klientowi dostep do ekranu logowania
                std::cout << "wyslano L do klienta" << std::endl;
            }
            //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            else if(clientResponse1[0]=='S'){
                //sprawdz czy klient jest gamemasterem
                for (int i = 0; i < connectedClients.size(); ++i)
                {
                    if (connectedClients[i].clientSocket == clientSocket)
                    {
                        if (connectedClients[i].gameMaster == true)
                        {
                            // rozpocznij gre na nowym watku
                            std::thread t(&Game::startGame, &games[connectedClients[i].gameCode]);
                            t.detach();

                            std::cout << "rozpoczeto gre " << std::to_string(games[connectedClients[i].gameCode].gameCode) << std::endl;
                        }
                        break;
                    }
                }
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