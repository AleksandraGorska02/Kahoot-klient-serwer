package org.example;

import javax.swing.*;
import javax.swing.text.JTextComponent;

public class GameFrame extends JFrame{

    JButton a=new JButton("a");//a
    JButton b=new JButton("b");//b
    JButton c=new JButton("c");//c
    JButton d=new JButton("d");//d
    JButton ok=new JButton("ok");//ok
  JLabel questionLabel;
  JLabel okLabel;
  JPanel questionPanel;

  JPanel o;
  JPanel answerPanel;




    GameFrame(int width,int height){
        super("example");
        this.setSize(width,height);
        this.setLocation(50,50);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    }

    public void makeQuestionPanel(String question){
        questionPanel = new JPanel();
        questionLabel = new JLabel(question);
        questionPanel.add(questionLabel);
        questionPanel.add(a);
        questionPanel.add(b);
        questionPanel.add(c);
        questionPanel.add(d);


    }
    public void makeOkPanel(){
        o = new JPanel();
        okLabel = new JLabel("czy jesteś gotowy?");

        o.add(okLabel);
        o.add(ok);

    }
    public void makeAnswerPanel(String answer){
        answerPanel = new JPanel();
        JLabel answerLabel = new JLabel(answer);
        answerPanel.add(answerLabel);
    }


}
