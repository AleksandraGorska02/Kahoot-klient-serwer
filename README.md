# Kahoot

## Table of Contents
* [General Info](#general-information)
* [Technologies Used](#technologies)
* [Setup](#setup)
* [Usage](#usage)
* [Room for Improvement](#room-for-improvement)



## General Information
Projekt ten skupia się na stworzeniu serwera do gry w quizy oraz klientów, które pozwalają użytkownikom tworzyć nowe gry lub dołączać do istniejących, podobnie jak popularna platforma Kahoot.


## Technologies
W projekcie komunikacja klient-serwer odbywa się z wykorzystaniem protokołu TCP. Do obsługi wejścia oraz wyjścia została wykorzystana funkcja *epoll*.


## Setup
...


## Usage
Użytkownik uruchamiając grę może wybrac jedną z dwóch opcji, które pokażą mu się na ekranie. 
![Example screenshot](./img/welcome_page.png)

Jeśli użytkownik wybierze opcję *Stwórz grę*, otworzy mu się formularz, w którym może wpisać swoje pytania z odpowiedziami oraz zaznaczyć, która odpowiedź jest poprawna. Następnie powinien kliknąć przycisk *wyślij grę*.
![Example screenshot](./img/question_form.png)


## Room for Improvement
