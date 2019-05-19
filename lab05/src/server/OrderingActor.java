package server;

import akka.actor.AbstractActor;
import messages.MessageType;
import messages.Request;
import messages.Response;

public class OrderingActor extends AbstractActor {
    private final Orderer orderer = new Orderer("orders.txt");

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    orderer.writeToFile(r.getArg());
                    Response res = new Response(MessageType.ORDER, true);
                    getSender().tell(res, null);
                })
                .build();
    }
}
