package com.dzindra.ui;

import javax.swing.*;
import java.awt.*;

class SpeedPanel extends JPanel {
    private int sx;
    private int sy;

    SpeedPanel() {
        super();

        setPreferredSize(new Dimension(100, 100));
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        double px = (double)sy / 255.0 * getWidth();
        double py = (1-(double)sx / 255.0) * getHeight();

        g.setColor(Color.BLACK);
        g.fillRect(0, 0, getWidth(), getHeight());

        g.setColor(Color.RED);
        g.fillRect((int)px - 3, (int)py - 3, 6, 6);
    }

    void setSpeeds(int x, int y) {
        sx = x;
        sy = y;
        repaint();
    }
}
