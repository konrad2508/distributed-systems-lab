package messages;

import java.io.Serializable;

public class Request implements Serializable {
    private final MessageType type;
    private final Object arg;

    public Request(MessageType type, Object arg) {
        this.type = type;
        this.arg = arg;
    }

    public MessageType getType() {
        return type;
    }

    public Object getArg() {
        return arg;
    }
}
