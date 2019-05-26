package server;

import akka.actor.AbstractActor;
import messages.*;

public class OrderingActor extends AbstractActor {

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    OrderArg args = (OrderArg) r.getArg();

                    Orderer orderer = args.getOrderer();
                    String title = args.getTitle();

                    boolean invoice = orderer.writeToFile(title);
                    Response res = new Response(MessageType.ORDER, invoice);
                    getSender().tell(res, null);
                })
                .build();
    }
}
