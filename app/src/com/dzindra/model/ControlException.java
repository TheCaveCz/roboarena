package com.dzindra.model;

import java.io.IOException;

public class ControlException extends IOException {

    public ControlException() {
    }

    public ControlException(String message) {
        super(message);
    }

    public ControlException(String message, Throwable cause) {
        super(message, cause);
    }

    public ControlException(Throwable cause) {
        super(cause);
    }
}
