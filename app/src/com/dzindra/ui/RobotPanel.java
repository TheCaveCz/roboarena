package com.dzindra.ui;

import com.dzindra.model.Robot;

import javax.swing.*;
import javax.swing.border.BevelBorder;

class RobotPanel extends JPanel {
    interface Listener {
        void pwmChangeRequest(Robot robot);
    }

    private final JLabel idLabel;
    private final JLabel addrLabel;
    private final JLabel pwm1Label;
    private final JLabel pwm2Label;
    private final JLabel speedXLabel;
    private final JLabel speedYLabel;
    private final JLabel brakeLabel;
    private final SpeedPanel speedPanel;
    private Robot robot;
    private Listener listener;

    RobotPanel() {
        super();
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBorder(new BevelBorder(BevelBorder.RAISED));

        idLabel = new JLabel();
        add(new RowPanel("ID", idLabel));

        speedXLabel = new JLabel();
        speedYLabel = new JLabel();
        speedPanel = new SpeedPanel();
        add(new RowPanel("speeds", speedPanel, speedXLabel, speedYLabel));

        brakeLabel = new JLabel();
        add(new RowPanel("brake", brakeLabel));

        pwm1Label = new JLabel();
        pwm2Label = new JLabel();
        JButton pwmChangeBtn = new JButton("Change");
        pwmChangeBtn.addActionListener(e -> pwmChangeAction());
        add(new RowPanel("maxPwm", pwm1Label, pwm2Label, pwmChangeBtn));

        addrLabel = new JLabel();
        add(new RowPanel("addr", addrLabel));
    }

    void setRobot(Robot robot) {
        this.robot = robot;
        SwingUtilities.invokeLater(this::updatePanel);
    }

    private void pwmChangeAction() {
        if (listener != null && robot != null)
            listener.pwmChangeRequest(robot);
    }

    private void updatePanel() {
        if (robot == null) return;

        idLabel.setText(robot.getId() + "");
        addrLabel.setText(robot.getAddress() + ":" + robot.getPort() + " (fw " + robot.getFwVersion() + ")");
        pwm1Label.setText(robot.getMaxPwm1() + "");
        pwm2Label.setText(robot.getMaxPwm2() + "");
        speedXLabel.setText(robot.getSpeedX() + "");
        speedYLabel.setText(robot.getSpeedY() + "");
        brakeLabel.setText(robot.isBrake() ? "ON" : "OFF");
        speedPanel.setSpeeds(robot.getSpeedX(), robot.getSpeedY());
    }

    public void setListener(Listener listener) {
        this.listener = listener;
    }
}
