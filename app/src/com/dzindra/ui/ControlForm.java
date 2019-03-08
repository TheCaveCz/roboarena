package com.dzindra.ui;

import com.dzindra.model.Control;
import com.dzindra.model.Robot;

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;

public class ControlForm extends JFrame implements Control.ControlListener, RobotPanel.Listener {
    private final Control control;
    private final RobotPanel[] robotPanels;

    public ControlForm(Control control) throws HeadlessException {
        super("RoboControl");
        this.control = control;

        setPreferredSize(new Dimension(800,500));
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        this.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosed(WindowEvent e) {
                System.out.println("Window closing");
                control.stop();
            }
        });

        getContentPane().setLayout(new BorderLayout());
        JPanel ctrlPane = new JPanel(new FlowLayout(FlowLayout.LEFT));

        JButton discoverButton = new JButton("Discover");
        discoverButton.addActionListener(e -> discoverButtonClick());
        ctrlPane.add(discoverButton);

        getContentPane().add(ctrlPane, BorderLayout.NORTH);


        JPanel roboPane = new JPanel(new GridLayout(2, 2, 10, 10));
        robotPanels = new RobotPanel[4];
        for (int i = 0; i < 4; i++) {
            robotPanels[i] = new RobotPanel();
            robotPanels[i].setRobot(control.getRobot(i));
            robotPanels[i].setListener(this);
            roboPane.add(robotPanels[i]);
        }
        getContentPane().add(roboPane, BorderLayout.CENTER);
        pack();
    }

    private void updatePanels() {
        for (int i = 0; i < 4; i++) {
            robotPanels[i].setRobot(control.getRobot(i));
        }
    }

    private void discoverButtonClick() {
        try {
            control.sendDiscover();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void gotNewRobot(Control control, Robot robot) {
        System.out.println("Got robot "+robot);
        updatePanels();
    }

    @Override
    public void gotNewSpeeds(Control control) {
        updatePanels();
    }

    @Override
    public void pwmChangeRequest(Robot robot) {
        PwmDialog d = new PwmDialog(this, robot);

        if (d.updatePwm()) {
            try {
                control.sendSetMaxPwm(robot);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
