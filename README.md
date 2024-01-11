# Kahoot

## Table of Contents
* [General Info](#general-information)
* [Technologies Used](#technologies)
* [Setup](#setup)
* [Usage](#usage)
* [Room for Improvement](#room-for-improvement)



## General Information
Projekt ten skupia się na stworzeniu serwera do gry w quizzy oraz klientów, które pozwalają użytkownikom tworzyć nowe gry lub dołączać do istniejących, podobnie jak popularna platforma Kahoot.


## Technologies
W projekcie komunikacja klient-serwer odbywa się z wykorzystaniem protokołu TCP. Do obsługi wejścia oraz wyjścia została wykorzystana funkcja *epoll*.


## Setup
...


## Usage
### Creating new quizz
Użytkownik uruchamiając grę może wybrac jedną z dwóch opcji, które pokażą mu się na ekranie. 
![Example screenshot](./img/welcome_page.png)

Jeśli użytkownik wybierze opcję *Stwórz grę*, otworzy mu się formularz, w którym może wpisać swoje pytania z odpowiedziami oraz zaznaczyć, która odpowiedź jest poprawna. Następnie powinien kliknąć przycisk *wyślij grę*.
![Example screenshot](./img/question_form.png)

Po stworzeniu gry użytkownikowi wyświetli się Kod gry, który został przydzielony do jego quizzu oraz liczba graczy, która dołączyła do gry. Osoba odpowiedzialna za stworzenie gry może uruchomić quizz klikając przycisk *Start*.
![Example screenshot](./img/waiting_for_players.png)

### Joining quizz
Drugą opcją, któa jest dostępna dla użytkownika po uruchomieniu aplikacji jest dołączenie do już istniejącego quizzu. Po wybraniu opcji *Dołącz do gry*, gracz musi wpisać dowolnie wybrany login (jeśli dany login już istnieje gracz zostanie o tym poinformowany i będzie musiał wpisać inny). Następnym krokiem jest wpisanie kodu gry do jakiej użytkownik chce dołączyć. Po podaniu właściwego kodu gracz może potwierdzić gotowość do gry i po rozpoczęciu quizzu przez gamemastera otrzyma pytanie wraz z możliwością zaznaczenia wybranej odpowiedzi.

Do gry można dołączyć w czasie trwania quizzu jednak wtedy za wszystkie pytania, na które inni użytkownicy już odpowiedzieli otrzyma się 0 punktów i wynik będzie liczony od pierwszego otrzymanego pytania po dołączniu.
Na koniec gry wyświetlany jest ranking, który zawiera nazwy graczy, którzy brali udział w quizzie oraz otrzymane przez nich punkty.


## Room for Improvement
