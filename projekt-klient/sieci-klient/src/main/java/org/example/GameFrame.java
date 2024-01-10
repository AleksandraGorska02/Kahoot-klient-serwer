package org.example;

import javax.swing.*;

public class GameFrame extends JFrame{

    JButton a=new JButton("a");//a
    JButton b=new JButton("b");//b
    JButton c=new JButton("c");//c
    JButton d=new JButton("d");//d
    JButton ok=new JButton("ok");//ok
    JButton wyslij=new JButton("wyslij");//wyslij
    JButton sendGame=new JButton("wyslij gre");//wyslij gre
    JButton sendCode=new JButton("wyslij kod");//wyslij kod
    JPanel sendCodePanel;
    JLabel sendCodeLabel;
    JTextField sendCodeTextField;

    JPanel GameMasterPanel;
    JLabel GameCodeLabel;
    JLabel NumberOfPlayersLabel;
    JButton StartGameButton= new JButton("Start");
  JLabel questionLabel;
  JLabel connectionLabel;
  JLabel okLabel;
  JPanel questionPanel;
  JPanel createGamePanel;
  JPanel firstPanel;
  JButton createGameButton;
  JButton joinGameButton;
  JPanel rankingPanel;
    JLabel rankingLabel;
    JLabel ranking1;

  JLabel createGameLabel;
  JLabel createQuestion1Label;
    JLabel createQuestion2Label;
    JLabel createQuestion3Label;
    JLabel createQuestion4Label;
    JLabel createQuestion5Label;
    JTextField createQuestion1;
    JTextField createQuestion2;
    JTextField createQuestion3;
    JTextField createQuestion4;
    JTextField createQuestion5;
    JRadioButton createAnswer1a;
    JRadioButton createAnswer1b;
    JRadioButton createAnswer1c;
    JRadioButton createAnswer1d;
    JRadioButton createAnswer2a;
    JRadioButton createAnswer2b;
    JRadioButton createAnswer2c;
    JRadioButton createAnswer2d;
    JRadioButton createAnswer3a;



    JRadioButton createAnswer3b;
    JRadioButton createAnswer3c;
    JRadioButton createAnswer3d;
    JRadioButton createAnswer4a;
    JRadioButton createAnswer4b;
    JRadioButton createAnswer4c;
    JRadioButton createAnswer4d;
    JRadioButton createAnswer5a;
    JRadioButton createAnswer5b;
    JRadioButton createAnswer5c;
    JRadioButton createAnswer5d;

  JPanel o;
  JPanel answerPanel;
  JPanel connectionPanel;
  JPanel waitForAnswerPanel;
  JLabel waitForAnswerLabel;
    JLabel connectionLabel1;
JTextField login;




    GameFrame(int width,int height){
        super("example");
        this.setSize(width,height);
        this.setLocation(50,50);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);


    }

    public void makeQuestionPanel(String question){
        questionPanel = new JPanel();
        questionLabel = new JLabel(question);
        //wrap text
        questionLabel.setSize(100,100);
        questionLabel.setInputMap(JComponent.WHEN_FOCUSED, null);


        questionPanel.add(questionLabel);
        questionPanel.add(a);
        questionPanel.add(b);
        questionPanel.add(c);
        questionPanel.add(d);


    }
    public void makeConnectionPanel(){
        connectionPanel = new JPanel();
        connectionLabel = new JLabel("Połączono z serwerem");
        connectionLabel1 = new JLabel("Podaj login:");
        login = new JTextField(10);

        connectionPanel.add(connectionLabel);
        connectionPanel.add(connectionLabel1);
        connectionPanel.add(login);
        connectionPanel.add(wyslij);
    }
    public void makeOkPanel(){
        o = new JPanel();
        okLabel = new JLabel("czy jesteś gotowy?");

        o.add(okLabel);
        o.add(ok);

    }
    public void makeSendCodePanel(){
        sendCodePanel = new JPanel();
        sendCodeLabel = new JLabel("Podaj kod gry:");
        sendCodeTextField = new JTextField(10);
        sendCodePanel.add(sendCodeLabel);
        sendCodePanel.add(sendCodeTextField);
        sendCodePanel.add(sendCode);
    }
    public void makeGameMasterPanel(){
        GameMasterPanel = new JPanel();
        GameCodeLabel = new JLabel("Kod gry: ");
        NumberOfPlayersLabel = new JLabel("Liczba graczy: ");

        //niedostepny przycisk

        GameMasterPanel.add(GameCodeLabel);
        GameMasterPanel.add(NumberOfPlayersLabel);
        GameMasterPanel.add(StartGameButton);
    }
    public void makeFirstPanel(){
        firstPanel = new JPanel();
        createGameButton = new JButton("Stwórz grę");
        joinGameButton = new JButton("Dołącz do gry");
        firstPanel.add(createGameButton);
        firstPanel.add(joinGameButton);
    }
    public void makeAnswerPanel(String answer){
        answerPanel = new JPanel();
        JLabel answerLabel = new JLabel(answer);
        answerPanel.add(answerLabel);
    }
    public void makeWaitForAnswerPanel(){
        waitForAnswerPanel = new JPanel();
        waitForAnswerLabel = new JLabel("odpowiedz wyslana posze czekac");
        waitForAnswerPanel.add(waitForAnswerLabel);
    }
    public void makeRankingPanel(){
        rankingPanel=new JPanel();
        rankingLabel=new JLabel("ranking graczy");
        ranking1=new JLabel("czekaj");

        rankingPanel.add(rankingLabel);
        rankingPanel.add(ranking1);


    }
    public void makeCreateGamePanel() {
        createGamePanel = new JPanel();
        createGameLabel = new JLabel("Stwórz grę\n");
        createQuestion1Label = new JLabel("Pytanie :\n");
        createQuestion2Label = new JLabel("Pytanie :\n");
        createQuestion3Label = new JLabel("Pytanie :\n");
        createQuestion4Label = new JLabel("Pytanie :\n");
        createQuestion5Label = new JLabel("Pytanie :'n");
        createQuestion1 = new JTextField(40);
        createQuestion2 = new JTextField(40);
        createQuestion3 = new JTextField(40);
        createQuestion4 = new JTextField(40);
        createQuestion5 = new JTextField(40);
        //tylko jedna odpowiedz moze byc poprawna

        createAnswer1a = new JRadioButton("a");
        createAnswer1b = new JRadioButton("b");
        createAnswer1c = new JRadioButton("c");
        createAnswer1d = new JRadioButton("d");
        //tylko jeden radio mozna zaznaczyc
        ButtonGroup group1 = new ButtonGroup();
        group1.add(createAnswer1a);
        group1.add(createAnswer1b);
        group1.add(createAnswer1c);
        group1.add(createAnswer1d);
        ButtonGroup group2 = new ButtonGroup();

        createAnswer2a = new JRadioButton("a");
        createAnswer2b = new JRadioButton("b");
        createAnswer2c = new JRadioButton("c");
        createAnswer2d = new JRadioButton("d");
        group2.add(createAnswer2a);
        group2.add(createAnswer2b);
        group2.add(createAnswer2c);
        group2.add(createAnswer2d);
        ButtonGroup group3 = new ButtonGroup();
        createAnswer3a = new JRadioButton("a");
        createAnswer3b = new JRadioButton("b");
        createAnswer3c = new JRadioButton("c");
        createAnswer3d = new JRadioButton("d");
        group3.add(createAnswer3a);
        group3.add(createAnswer3b);
        group3.add(createAnswer3c);
        group3.add(createAnswer3d);
        ButtonGroup group4 = new ButtonGroup();
        createAnswer4a = new JRadioButton("a");
        createAnswer4b = new JRadioButton("b");
        createAnswer4c = new JRadioButton("c");
        createAnswer4d = new JRadioButton("d");
        group4.add(createAnswer4a);
        group4.add(createAnswer4b);
        group4.add(createAnswer4c);
        group4.add(createAnswer4d);
        ButtonGroup group5 = new ButtonGroup();
        createAnswer5a = new JRadioButton("a");
        createAnswer5b = new JRadioButton("b");
        createAnswer5c = new JRadioButton("c");
        createAnswer5d = new JRadioButton("d");
        group5.add(createAnswer5a);
        group5.add(createAnswer5b);
        group5.add(createAnswer5c);
        group5.add(createAnswer5d);

        createGamePanel.add(createGameLabel);
        createGamePanel.add(createQuestion1Label);
        createGamePanel.add(createQuestion1);
        createGamePanel.add(createAnswer1a);
        createGamePanel.add(createAnswer1b);
        createGamePanel.add(createAnswer1c);
        createGamePanel.add(createAnswer1d);
        createGamePanel.add(createQuestion2Label);
        createGamePanel.add(createQuestion2);
        createGamePanel.add(createAnswer2a);
        createGamePanel.add(createAnswer2b);
        createGamePanel.add(createAnswer2c);
        createGamePanel.add(createAnswer2d);
        createGamePanel.add(createQuestion3Label);
        createGamePanel.add(createQuestion3);
        createGamePanel.add(createAnswer3a);
        createGamePanel.add(createAnswer3b);
        createGamePanel.add(createAnswer3c);
        createGamePanel.add(createAnswer3d);
        createGamePanel.add(createQuestion4Label);
        createGamePanel.add(createQuestion4);

        createGamePanel.add(createAnswer4a);
        createGamePanel.add(createAnswer4b);
        createGamePanel.add(createAnswer4c);
        createGamePanel.add(createAnswer4d);
        createGamePanel.add(createQuestion5Label);
        createGamePanel.add(createQuestion5);
        createGamePanel.add(createAnswer5a);
        createGamePanel.add(createAnswer5b);
        createGamePanel.add(createAnswer5c);
        createGamePanel.add(createAnswer5d);
createGamePanel.add(sendGame);





    }


}
