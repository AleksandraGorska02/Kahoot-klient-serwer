package org.example;

import javax.swing.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Main {
    public static void main(String[] args) throws IOException {


        Socket clientSocket = new Socket();


        InetSocketAddress serverAddress = new InetSocketAddress(InetAddress.getByName("127.0.0.1"), 12135 );

        GameFrame gameFrame = new GameFrame(500, 600);

        connect(clientSocket, serverAddress, gameFrame);


    }
    static void threadsMessage(Socket clientSocket, GameFrame gameFrame){
      Thread getMessage=  new Thread(()->{
          while (true) {
              try {
                  BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

                  // Read a line from the socket
                  String message = reader.readLine();

                  if (message != null) {
                      System.out.println("Otrzymano dane od serwera watek: " + message);
                      messageFromServer(message, gameFrame);
                  } else {
                      // Handle case when the server disconnects
                      System.out.println("Serwer się rozłączył.");
                      break;
                  }
              } catch (IOException e) {
                  throw new RuntimeException(e);
              }
          }
        });
        getMessage.start();
    }
   public static void connect(Socket clientSocket, InetSocketAddress serverAddress,GameFrame gameFrame) {
       SwingUtilities.invokeLater(()->{

        while(true){
        try {
            //tworzymy okno
            gameFrame.setVisible(true);
            gameFrame.makeFirstPanel();
            gameFrame.setContentPane(gameFrame.firstPanel);
            gameFrame.validate();

//łączymy się z serwerem
            clientSocket.connect(new InetSocketAddress(serverAddress.getAddress(), serverAddress.getPort()));
            System.out.println("Połączono z serwerem 1");
            threadsMessage(clientSocket,gameFrame);
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
//dodajemy akcje dla przycisków
            question(clientSocket, out, gameFrame);
            ok(gameFrame, out);
            login(clientSocket, out, gameFrame);
            createGame(clientSocket, out, gameFrame);
            firstChose(clientSocket, out, gameFrame);
            startGame(clientSocket, out, gameFrame);
            code(clientSocket, out, gameFrame);
               break;

        } catch (IOException e) {
            System.err.println("Błąd przy połączeniu z serwerem");
            try {
                clientSocket.close();
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
            break;

        }}  });
    }
    static void messageFromServer(String message, GameFrame gameFrame){
        //przechwycenie i odpaleniie ekranu logowania
        if(message.startsWith("L")){
            gameFrame.setVisible(true);
            gameFrame.makeConnectionPanel();
            gameFrame.setContentPane(gameFrame.connectionPanel);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: przechwycenie wiadomosci " + message);
        }

        //login jest ok czekamy az gracz jest gotowy
        if(message.equals("ok")){
            gameFrame.setVisible(true);
            gameFrame.makeOkPanel();
            gameFrame.setContentPane(gameFrame.o);
            gameFrame.validate();
        }
        //przechwycenie pytania
        if (message.startsWith("P")){
            gameFrame.setVisible(true);
            gameFrame.makeQuestionPanel(message);

            gameFrame.setContentPane(gameFrame.questionPanel);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: przechwycenie wiadomosci " + message);
            gameFrame.questionLabel.setBounds(100,10,300, 70);

            gameFrame.a.setBounds(100,100,100, 40);
            gameFrame.b.setBounds(300,100,100, 40);
            gameFrame.c.setBounds(100,200,100, 40);
            gameFrame.d.setBounds(300,200,100, 40);


        }
        if(message.startsWith("Kod")){
            gameFrame.setVisible(true);
            gameFrame.makeGameMasterPanel();

            gameFrame.setContentPane(gameFrame.GameMasterPanel);
            gameFrame.StartGameButton.setEnabled(false);
            //ustaw kod pytania
            gameFrame.GameCodeLabel.setText(message);
            gameFrame.validate();
        }
        if(message.startsWith("G")){

           gameFrame.NumberOfPlayersLabel.setText(message);
           gameFrame.StartGameButton.setEnabled(true);


        }
        if(message.startsWith("X")){
            gameFrame.setVisible(true);
            gameFrame.makeSendCodePanel();
            gameFrame.setContentPane(gameFrame.sendCodePanel);
            gameFrame.validate();
        }
        if(message.startsWith("Y")){
            gameFrame.setVisible(true);
            gameFrame.makeSendCodePanel();
            gameFrame.sendCodeLabel.setText("Kod jest niepoprawny");
            gameFrame.setContentPane(gameFrame.sendCodePanel);
            gameFrame.sendCode.setEnabled(true);

            gameFrame.validate();
        }
        //przechwycenie odpowiedzi
        if (message.startsWith("O")){
            gameFrame.setVisible(true);
            gameFrame.makeAnswerPanel(message);
            gameFrame.setContentPane(gameFrame.answerPanel);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: przechwycenie wiadomosci " + message);
        }
        //przechwycenie rankingu
        if(message.startsWith("R")){
            gameFrame.setVisible(true);
            gameFrame.makeRankingPanel();

            gameFrame.setContentPane(gameFrame.rankingPanel);
            //tam gdzie jest w rankingu \t dodaj new line
            String s = message.replaceAll("\t", "<br>");
            gameFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            gameFrame.ranking1.setText("<html>"+s+"<html>");
            System.out.println(s);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: przechwycenie wiadomosci " + message);
        }

        //przechwycenie loginu jak  jest zajety
        if(message.startsWith("N")){
            gameFrame.setVisible(true);
            gameFrame.makeConnectionPanel();
            gameFrame.setContentPane(gameFrame.connectionPanel);
            gameFrame.connectionLabel.setText("login zajety, wpisz inny");
            gameFrame.wyslij.setEnabled(true);
            gameFrame.login.setEnabled(true);
            gameFrame.validate();
            System.out.println("login uzyty " + message);
        }
        if(message.startsWith("C")){
            gameFrame.setVisible(true);
            gameFrame.makeCreateGamePanel();



            gameFrame.setContentPane(gameFrame.createGamePanel);



            gameFrame.createGameLabel.setBounds(100,30,100, 20);
            gameFrame.createQuestion1Label.setBounds(100,60,100, 20);
            gameFrame.createQuestion1.setBounds(100,90,300, 30);
            gameFrame.createQuestion2Label.setBounds(100,150,100, 30);
            gameFrame.createQuestion2.setBounds(100,180,300, 30);
            gameFrame.createQuestion3Label.setBounds(100,2400,100, 30);
            gameFrame.createQuestion3.setBounds(100,270,300, 30);
            gameFrame.createQuestion4Label.setBounds(100,330,100, 30);
            gameFrame.createQuestion4.setBounds(100,360,300, 30);
            gameFrame.createQuestion5Label.setBounds(100,420,100, 30);
            gameFrame.createQuestion5.setBounds(100,450,300, 30);
            gameFrame.createAnswer1a.setBounds(100,120,100, 30);
            gameFrame.createAnswer1b.setBounds(200,120,100, 30);
            gameFrame.createAnswer1c.setBounds(300,120,100, 30);
            gameFrame.createAnswer1d.setBounds(400,120,100, 30);


            gameFrame.createAnswer2a.setBounds(100,210,100, 30);
            gameFrame.createAnswer2b.setBounds(200,210,100, 30);
            gameFrame.createAnswer2c.setBounds(300,210,100, 30);
            gameFrame.createAnswer2d.setBounds(400,210,100, 30);


            gameFrame.createAnswer3a.setBounds(100,300,100, 30);
            gameFrame.createAnswer3b.setBounds(200,300,100, 30);
            gameFrame.createAnswer3c.setBounds(300,300,100, 30);
            gameFrame.createAnswer3d.setBounds(400,300,100, 30);


            gameFrame.createAnswer4a.setBounds(100,390,100, 30);
            gameFrame.createAnswer4b.setBounds(200,390,100, 30);
            gameFrame.createAnswer4c.setBounds(300,390,100, 30);
            gameFrame.createAnswer4d.setBounds(400,390,100, 30);


            gameFrame.createAnswer5a.setBounds(100,480,100, 30);
            gameFrame.createAnswer5b.setBounds(200,480,100, 30);
            gameFrame.createAnswer5c.setBounds(300,480,100, 30);
            gameFrame.createAnswer5d.setBounds(400,480,100, 30);
            gameFrame.sendGame.setBounds(100,520,100, 30);
            gameFrame.validate();
        }

    }

    static void ok(GameFrame gameFrame, PrintWriter out){

        gameFrame.ok.addActionListener(e -> {
           System.out.println("do pytania");
            out.println("ok");
            System.out.println("Wysłano odpowiedź do serwera: ok");
            gameFrame.ok.setEnabled(false);
            gameFrame.okLabel.setText("czekaj na pytanie");


        });

    }
    static void startGame(Socket clientSocket, PrintWriter out, GameFrame gameFrame){
        gameFrame.StartGameButton.addActionListener(e -> {
            out.println("S");
            System.out.println("Wysłano odpowiedź do serwera: S");
            gameFrame.StartGameButton.setEnabled(false);

        });
    }
    static void firstChose(Socket clientSocket, PrintWriter out, GameFrame gameFrame){
        gameFrame.createGameButton.addActionListener(e -> {
            out.println("G");
            System.out.println("Wysłano odpowiedź do serwera: C");

        });
        gameFrame.joinGameButton.addActionListener(e -> {
            out.println("g");
            System.out.println("Wysłano odpowiedź do serwera: J");

        });
    }
    static void login(Socket clientSocket, PrintWriter out, GameFrame gameFrame){
        gameFrame.wyslij.addActionListener(e -> {
            //pole loginu nie może być puste
            if(gameFrame.login.getText().isEmpty()){
                gameFrame.connectionLabel.setText("wpisz login");
                return;
            }
            String login = "L"+gameFrame.login.getText();
            out.println(login);
            System.out.println("Wysłano odpowiedź do serwera: " + login);
            gameFrame.wyslij.setEnabled(false);
            gameFrame.connectionLabel.setText("Wysłano login");
            gameFrame.connectionLabel1.setText("Czekaj na pytanie");
            gameFrame.login.setEnabled(false);
        });
    }
    static void code(Socket clientSocket, PrintWriter out, GameFrame gameFrame){
        gameFrame.sendCode.addActionListener(e -> {
            String code = gameFrame.sendCodeTextField.getText();
            code="X"+code;
            out.println(code);
            System.out.println("Wysłano odpowiedź do serwera: " + code);
            gameFrame.sendCode.setEnabled(false);
        });
    }

    static void question(Socket clientSocket,PrintWriter out, GameFrame gameFrame) throws IOException {


                gameFrame.a.addActionListener(e -> {
                    String Resp = gameFrame.a.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    gameFrame.setVisible(true);
                    gameFrame.makeWaitForAnswerPanel();
                    gameFrame.setContentPane(gameFrame.waitForAnswerPanel);
                    gameFrame.validate();

               });

                gameFrame.b.addActionListener(e -> {
                    String Resp = gameFrame.b.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    gameFrame.setVisible(true);
                    gameFrame.makeWaitForAnswerPanel();
                    gameFrame.setContentPane(gameFrame.waitForAnswerPanel);
                    gameFrame.validate();

                });

                gameFrame.c.addActionListener(e -> {
                    String Resp = gameFrame.c.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    gameFrame.setVisible(true);
                    gameFrame.makeWaitForAnswerPanel();
                    gameFrame.setContentPane(gameFrame.waitForAnswerPanel);
                    gameFrame.validate();
               });

                gameFrame.d.addActionListener(e -> {
                    String Resp = gameFrame.d.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    gameFrame.setVisible(true);
                    gameFrame.makeWaitForAnswerPanel();
                    gameFrame.setContentPane(gameFrame.waitForAnswerPanel);
                    gameFrame.validate();
                });

        }
        static void createGame(Socket clientSocket, PrintWriter out, GameFrame gameFrame){
        gameFrame.sendGame.addActionListener(e -> {
            String question1 = gameFrame.createQuestion1.getText();
            String question2 = gameFrame.createQuestion2.getText();
            String question3 = gameFrame.createQuestion3.getText();
            String question4 = gameFrame.createQuestion4.getText();
            String question5 = gameFrame.createQuestion5.getText();
            //wez wartość zaznaczonego przycisku
            String answer1 = gameFrame.createAnswer1a.isSelected() ? "a" : gameFrame.createAnswer1b.isSelected() ? "b" : gameFrame.createAnswer1c.isSelected() ? "c" : gameFrame.createAnswer1d.isSelected() ? "d" : "";
            String answer2 = gameFrame.createAnswer2a.isSelected() ? "a" : gameFrame.createAnswer2b.isSelected() ? "b" : gameFrame.createAnswer2c.isSelected() ? "c" : gameFrame.createAnswer2d.isSelected() ? "d" : "";
            String answer3 = gameFrame.createAnswer3a.isSelected() ? "a" : gameFrame.createAnswer3b.isSelected() ? "b" : gameFrame.createAnswer3c.isSelected() ? "c" : gameFrame.createAnswer3d.isSelected() ? "d" : "";
            String answer4 = gameFrame.createAnswer4a.isSelected() ? "a" : gameFrame.createAnswer4b.isSelected() ? "b" : gameFrame.createAnswer4c.isSelected() ? "c" : gameFrame.createAnswer4d.isSelected() ? "d" : "";
            String answer5 = gameFrame.createAnswer5a.isSelected() ? "a" : gameFrame.createAnswer5b.isSelected() ? "b" : gameFrame.createAnswer5c.isSelected() ? "c" : gameFrame.createAnswer5d.isSelected() ? "d" : "";
            //sprawdz czy wszystkie pola sa wypelnione
            if(question1.equals("")||question2.equals("")||question3.equals("")||question4.equals("")||question5.equals("")||answer1.equals("")||answer2.equals("")||answer3.equals("")||answer4.equals("")||answer5.equals("")){
                gameFrame.createGameLabel.setText("Wypelnij wszystkie pola");
                return;
            }
            //wtynij wsztkie znaki nowej lini



            String QandA1= "P1"+question1+answer1+"\n";

            String QandA2= "P2"+question2+answer2+"\n";
            String QandA3= "P3"+question3+answer3+"\n";
            String QandA4= "P4"+question4+answer4+"\n";
            String QandA5= "P5"+question5+answer5+"\n";
//wyslij do serwera
            out.println(QandA1);

            out.println(QandA2);
            out.println(QandA3);
            out.println(QandA4);
            out.println(QandA5);
            System.out.println("Wysłano odpowiedź do serwera: " + QandA1);

        });
        }

    }
