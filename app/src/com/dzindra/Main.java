package com.dzindra;

import com.dzindra.model.Control;
import com.dzindra.ui.ControlForm;

import javax.swing.*;

public class Main {

    public static void main(String[] args) throws Exception {
        Control control = new Control(null);

        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception ignored) {
        }

        SwingUtilities.invokeAndWait(() -> {
            ControlForm cf = new ControlForm(control);
            control.setListener(cf);
            cf.setLocationRelativeTo(null);
            cf.setVisible(true);
        });

        Thread.sleep(200);
        control.sendDiscover();

    }
}
