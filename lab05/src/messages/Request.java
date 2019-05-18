package messages;

import java.io.Serializable;

public class Request implements Serializable {
    private final MessageType type;
    private final String arg;

    public Request(MessageType type, String arg) {
        this.type = type;
        this.arg = arg;
    }

    public MessageType getType() {
        return type;
    }

    public String getArg() {
        return arg;
    }
}
