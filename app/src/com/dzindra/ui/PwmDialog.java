package com.dzindra.ui;

import com.dzindra.model.Robot;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

class PwmDialog extends JDialog {
    private final Robot robot;
    private boolean modify = false;
    private final JTextField maxPwm1Edit;
    private final JTextField maxPwm2Edit;

    PwmDialog(Frame owner, Robot robot) {
        super(owner, "Pwm change", true);

        this.robot = robot;

        getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));

        maxPwm1Edit = new JTextField("" + robot.getMaxPwm1());
        getContentPane().add(new RowPanel("maxPwm1", maxPwm1Edit));

        maxPwm2Edit = new JTextField("" + robot.getMaxPwm2());
        getContentPane().add(new RowPanel("maxPwm2", maxPwm2Edit));

        JButton okBtn = new JButton("OK");
        okBtn.addActionListener(e -> {
            int pwm1, pwm2;
            try {
                pwm1 = Integer.parseInt(maxPwm1Edit.getText());
                pwm2 = Integer.parseInt(maxPwm2Edit.getText());
            } catch (NumberFormatException ee) {
                return;
            }
            if (pwm1 < 0 || pwm1 > 1023 || pwm2 < 0 || pwm2 > 1023) return;

            robot.setMaxPwm1(pwm1);
            robot.setMaxPwm2(pwm2);
            modify = true;
            dispose();
        });
        JButton cancelBtn = new JButton("Cancel");
        cancelBtn.addActionListener(e -> dispose());
        JPanel pnl = new JPanel(new FlowLayout(FlowLayout.TRAILING));
        pnl.add(okBtn);
        pnl.add(cancelBtn);
        getContentPane().add(pnl);

        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        pack();
    }


    boolean updatePwm() {
        setLocationRelativeTo(getOwner());
        setVisible(true);
        return modify;
    }
}
