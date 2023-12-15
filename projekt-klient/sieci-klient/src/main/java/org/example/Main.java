package org.example;

import javax.swing.*;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.concurrent.CountDownLatch;

public class Main {
    public static void main(String[] args) throws IOException {


        Socket clientSocket = new Socket();


        InetSocketAddress serverAddress = new InetSocketAddress(InetAddress.getByName("127.0.0.1"), 12348 );

        GameFrame gameFrame = new GameFrame(500, 500);

        connect(clientSocket, serverAddress, gameFrame);









    }
    static void threadsMessage(Socket clientSocket, GameFrame gameFrame){
      Thread getMessage=  new Thread(()->{
            while(true){
                byte[] buffer = new byte[1024];
                int bytesRead = 0;
                try {
                    // Read bytes from socket
                    bytesRead = clientSocket.getInputStream().read(buffer);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
                if (bytesRead > 0) {
                    // Convert bytes to string
                    String message = new String(buffer, 0, bytesRead);
                    System.out.println("Otrzymano dane od serwera watek: " + message);
                    messageFromServer(message,gameFrame);


            }}
        });
        getMessage.start();
    }
   public static void connect(Socket clientSocket, InetSocketAddress serverAddress,GameFrame gameFrame) throws IOException {
       SwingUtilities.invokeLater(()->{

        while(true){
        try {
            //tworzymy okno
            gameFrame.setVisible(true);
            gameFrame.makeOkPanel();
            gameFrame.setContentPane(gameFrame.o);
            gameFrame.validate();
//łączymy się z serwerem
            clientSocket.connect(new InetSocketAddress(serverAddress.getAddress(), serverAddress.getPort()));
            System.out.println("Połączono z serwerem 1");
            threadsMessage(clientSocket,gameFrame);
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
//dodajemy akcje dla przycisków
            question(clientSocket, out, gameFrame);
            ok(gameFrame, out);
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
        if(message.equals("ok")){
            gameFrame.setVisible(true);
            gameFrame.makeOkPanel();
            gameFrame.setContentPane(gameFrame.o);
            gameFrame.validate();
        }
        if (message.startsWith("P")){
            gameFrame.setVisible(true);
            gameFrame.makeQuestionPanel(message);
            gameFrame.setContentPane(gameFrame.questionPanel);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: przechwycenie wiadomosci " + message);


            gameFrame.a.setBounds(100,100,100, 40);
            gameFrame.b.setBounds(300,100,100, 40);
            gameFrame.c.setBounds(100,200,100, 40);
            gameFrame.d.setBounds(300,200,100, 40);


        }
        if (message.startsWith("O")){
            gameFrame.setVisible(true);
            gameFrame.makeAnswerPanel(message);
            gameFrame.setContentPane(gameFrame.answerPanel);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: przechwycenie wiadomosci " + message);
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

    static void question(Socket clientSocket,PrintWriter out, GameFrame gameFrame) throws IOException {


                gameFrame.a.addActionListener(e -> {
                    String Resp = gameFrame.a.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
               });

                gameFrame.b.addActionListener(e -> {
                    String Resp = gameFrame.b.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                });

                gameFrame.c.addActionListener(e -> {
                    String Resp = gameFrame.c.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
               });

                gameFrame.d.addActionListener(e -> {
                    String Resp = gameFrame.d.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                });





        }

    }
