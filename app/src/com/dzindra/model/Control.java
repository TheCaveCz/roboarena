package com.dzindra.model;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.*;


public class Control implements Runnable {
    public interface ControlListener {
        void gotNewRobot(Control control, Robot robot);

        void gotNewSpeeds(Control control);
    }

    private static final int PACKET_HEADER = 0x62;
    private static final int MSG_SPEED = 1;
    private static final int MSG_SET_ID = 2;
    private static final int MSG_SET_PWM = 3;
    private static final int MSG_DISCOVER = 4;
    private static final int MSG_DISCOVER_RESPONSE = 5;

    private final Object lock = new Object();
    private final Thread thread;
    private volatile boolean running;

    private DatagramSocket socket;
    private InetAddress addr;
    private int port;

    private Robot[] robots;

    private ControlListener listener;


    public Control(ControlListener listener) throws IOException {
        this(listener, 44123);
    }

    public Control(ControlListener listener, int port) throws IOException {
        this.port = port;
        socket = new DatagramSocket(port);
        addr = Inet4Address.getByName("255.255.255.255");
        this.listener = listener;

        robots = new Robot[4];

        running = true;
        thread = new Thread(this, "control-receiver");
        thread.start();
    }

    public void setListener(ControlListener listener) {
        this.listener = listener;
    }

    public void stop() {
        running = false;
        thread.interrupt();
        socket.close();
    }

    public Robot getRobot(int id) {
        if (id < 0 || id > 3) return null;

        synchronized (lock) {
            return robots[id] == null ? null : new Robot(robots[id]);
        }
    }

    public void sendDiscover() throws IOException {
        sendMessage(MSG_DISCOVER, null, 0);
    }

    public void sendSetMaxPwm(Robot robot) throws IOException {
        sendMessage(MSG_SET_PWM,
                robot.getAddress(),
                robot.getPort(),
                robot.getMaxPwm1() >> 8,
                robot.getMaxPwm1() & 0xff,
                robot.getMaxPwm2() >> 8,
                robot.getMaxPwm2() & 0xff
        );
    }

    private void sendMessage(int code, InetAddress addr, int port, int... data) throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        baos.write(PACKET_HEADER);
        baos.write(code);
        for (int b : data) {
            baos.write(b);
        }

        byte[] buffer = baos.toByteArray();
        DatagramPacket packet = new DatagramPacket(buffer, 0, buffer.length, addr == null ? this.addr : addr, port == 0 ? this.port : port);
        socket.send(packet);
    }


    private void parsePacket(byte[] packet, InetAddress addr, int port) throws IOException {
        DataInputStream daos = new DataInputStream(new ByteArrayInputStream(packet));
        if (daos.readUnsignedByte() != PACKET_HEADER) {
            throw new ControlException("Invalid packet header");
        }
        int msg = daos.readUnsignedByte();
        switch (msg) {
            case MSG_DISCOVER_RESPONSE:
                int roboId = daos.readUnsignedByte();
                int maxPwm1 = daos.readUnsignedShort();
                int maxPwm2 = daos.readUnsignedShort();
                int fwVer = daos.readUnsignedByte();

                Robot robot = new Robot(roboId, maxPwm1, maxPwm2, fwVer, addr, port);
                if (roboId >= 1 && roboId <= 4) {
                    synchronized (lock) {
                        robots[roboId - 1] = robot;
                    }
                }
                if (listener != null) listener.gotNewRobot(this, robot);
                break;

            case MSG_SPEED:
                synchronized (lock) {
                    for (Robot r : robots) {
                        int sx = daos.readUnsignedByte();
                        int sy = daos.readUnsignedByte();
                        if (r == null) continue;
                        r.setSpeedX(sx);
                        r.setSpeedY(sy);
                    }
                    int bm = daos.readUnsignedByte();
                    for (int i = 0; i < 4; i++) {
                        if (robots[i] != null) {
                            robots[i].setBrake((bm & (1 << i)) != 0);
                        }
                    }
                }
                if (listener != null) listener.gotNewSpeeds(this);
                break;
        }
    }

    @Override
    public void run() {
        DatagramPacket p = new DatagramPacket(new byte[512], 512);
        while (running) {
            try {
                socket.receive(p);

                parsePacket(p.getData(), p.getAddress(), p.getPort());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}
