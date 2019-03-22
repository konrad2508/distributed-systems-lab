import java.io.Serializable;

public class MessageObject implements Serializable {
    private MessageObjectType type;
    private String key;
    private Integer element;

    public MessageObject(MessageObjectType type, String key) {
        this.type = type;
        this.key = key;
    }

    public MessageObject(MessageObjectType type, String key, Integer element) {
        this.type = type;
        this.key = key;
        this.element = element;
    }

    public MessageObjectType getType() {
        return this.type;
    }

    public String getKey() {
        return this.key;
    }

    public Integer getElement() {
        return this.element;
    }
}
