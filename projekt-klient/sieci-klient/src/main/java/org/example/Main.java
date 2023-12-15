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


        InetSocketAddress serverAddress = new InetSocketAddress(InetAddress.getByName("127.0.0.1"), 10013);

        GameFrame gameFrame = new GameFrame(500, 500);
        try {
            clientSocket = new Socket();
            clientSocket.connect(new InetSocketAddress(serverAddress.getAddress(), serverAddress.getPort()));
            System.out.println("Połączono z serwerem 1");
        } catch (IOException e) {
            System.err.println("Błąd przy połączeniu z serwerem");
            clientSocket.close();

        }

        PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
       ok(gameFrame);
        question(clientSocket, out, gameFrame);




        clientSocket.close();
        System.out.println("Zamknięto połączenie z serwerem");


    }

    static void ok(GameFrame gameFrame){
        gameFrame.setVisible(true);
        gameFrame.makeOkPanel();
        gameFrame.setContentPane(gameFrame.o);

        CountDownLatch latch = new CountDownLatch(1);
        gameFrame.ok.addActionListener(e -> {
            //poczekaj na nacisniecie i potem przejdz dalej

            latch.countDown();
        });
        try {
            latch.await();
            System.out.println("przechodzimy do pytania");
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

    }

    static void question(Socket clientSocket, PrintWriter out, GameFrame gameFrame) throws IOException {

        byte[] buffer = new byte[1024];
        int bytesRead = clientSocket.getInputStream().read(buffer);
        if (bytesRead > 0) {
            String question = new String(buffer, 0, bytesRead);
gameFrame.setVisible(true);
            gameFrame.makeQuestionPanel(question);

            gameFrame.setContentPane(gameFrame.questionPanel);
            gameFrame.validate();
            System.out.println("Otrzymano dane od serwera: " + question);

            gameFrame.a.setBounds(100,100,100, 40);
            gameFrame.b.setBounds(300,100,100, 40);
            gameFrame.c.setBounds(100,200,100, 40);
            gameFrame.d.setBounds(300,200,100, 40);



                CountDownLatch latch = new CountDownLatch(1);

                gameFrame.a.addActionListener(e -> {
                    String Resp = gameFrame.a.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    latch.countDown(); // Signal that the button has been pressed
                });

                gameFrame.b.addActionListener(e -> {
                    String Resp = gameFrame.b.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    latch.countDown(); // Signal that the button has been pressed
                });

                gameFrame.c.addActionListener(e -> {
                    String Resp = gameFrame.c.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    latch.countDown(); // Signal that the button has been pressed
                });

                gameFrame.d.addActionListener(e -> {
                    String Resp = gameFrame.d.getText();
                    out.println(Resp);
                    System.out.println("Wysłano odpowiedź do serwera: " + Resp);
                    latch.countDown(); // Signal that the button has been pressed
                });

                try {
                    latch.await();
                    System.out.println("Oczekuję na odpowiedź od serwera");
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }



        }

    }
}