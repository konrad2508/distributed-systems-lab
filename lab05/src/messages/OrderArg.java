package messages;

public class OrderArg {
    private final Orderer orderer;
    private final String title;

    public OrderArg(Orderer orderer, String title) {
        this.orderer = orderer;
        this.title = title;
    }

    public Orderer getOrderer() {
        return orderer;
    }

    public String getTitle() {
        return title;
    }
}
