package com.dzindra.model;

import java.net.InetAddress;

public class Robot {
    private final int id;
    private int maxPwm1;
    private int maxPwm2;
    private final int fwVersion;
    private final InetAddress address;
    private final int port;
    private int speedX;
    private int speedY;
    private boolean brake;

    Robot(int id, int maxPwm1, int maxPwm2, int fwVersion, InetAddress address, int port) {
        this.id = id;
        this.maxPwm1 = maxPwm1;
        this.maxPwm2 = maxPwm2;
        this.fwVersion = fwVersion;
        this.address = address;
        this.port = port;
    }

    Robot(Robot other) {
        this.id = other.id;
        this.maxPwm1 = other.maxPwm1;
        this.maxPwm2 = other.maxPwm2;
        this.fwVersion = other.fwVersion;
        this.address = other.address;
        this.port = other.port;
        this.speedX = other.speedX;
        this.speedY = other.speedY;
        this.brake = other.brake;
    }

    public int getId() {
        return id;
    }

    public int getMaxPwm1() {
        return maxPwm1;
    }

    public void setMaxPwm1(int maxPwm1) {
        this.maxPwm1 = maxPwm1;
    }

    public int getMaxPwm2() {
        return maxPwm2;
    }

    public void setMaxPwm2(int maxPwm2) {
        this.maxPwm2 = maxPwm2;
    }

    public int getFwVersion() {
        return fwVersion;
    }


    public InetAddress getAddress() {
        return address;
    }


    public int getPort() {
        return port;
    }


    public int getSpeedX() {
        return speedX;
    }

    void setSpeedX(int speedX) {
        this.speedX = speedX;
    }

    public int getSpeedY() {
        return speedY;
    }

    void setSpeedY(int speedY) {
        this.speedY = speedY;
    }

    public boolean isBrake() {
        return brake;
    }

    void setBrake(boolean brake) {
        this.brake = brake;
    }

    @Override
    public String toString() {
        return "Robot{" +
                "id=" + id +
                ", maxPwm1=" + maxPwm1 +
                ", maxPwm2=" + maxPwm2 +
                ", fwVersion=" + fwVersion +
                ", address=" + address +
                ", port=" + port +
                ", speedX=" + speedX +
                ", speedY=" + speedY +
                ", brake=" + brake +
                '}';
    }
}
