package com.dzindra.ui;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import java.awt.*;

class RowPanel extends JPanel {

    RowPanel(String title, Component... children) {
        super(new GridLayout(1, 2));
        setBorder(new BevelBorder(BevelBorder.LOWERED));
        JLabel l = new JLabel(title);
        l.setForeground(new Color(128,128,128));
        add(l);
        if (children.length == 1) {
            add(children[0]);
        } else if (children.length > 1) {
            JPanel p = new JPanel(new FlowLayout(FlowLayout.LEADING));
            for (Component c : children) {
                p.add(c);
            }
            add(p);
        }
    }
}
