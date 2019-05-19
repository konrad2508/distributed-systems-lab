package server;

import akka.actor.*;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import akka.japi.pf.DeciderBuilder;
import messages.*;
import scala.concurrent.duration.Duration;

import java.io.FileNotFoundException;
import java.io.IOException;

public class BookstoreActor extends AbstractActor {
    private final LoggingAdapter log = Logging.getLogger(getContext().getSystem(), this);
    private final Orderer orderer = new Orderer("orders.txt");
    private int id = 0;

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    MessageType type = r.getType();
                    id++;

                    if (type == MessageType.FIND) findProcedure(r);
                    else if (type == MessageType.ORDER) orderProcedure(r);
                    else if (type == MessageType.STREAM) streamProcedure(r);
                    else System.out.println("ayy");
                })
                .matchAny(o -> log.info("received unknown message"))
                .build();
    }

    private void findProcedure(Request r) {
        ActorRef finding = context().actorOf(Props.create(FindingActor.class), "finding" + id);

        finding.tell(r, getSender());
        finding.tell(PoisonPill.getInstance(), null);
    }

    private void orderProcedure(Request r) {
        ActorRef ordering = context().actorOf(Props.create(OrderingActor.class), "ordering" + id);
        OrderArg arg = new OrderArg(orderer, (String) r.getArg());
        Request order = new Request(MessageType.ORDER, arg);

        ordering.tell(order, getSender());
        ordering.tell(PoisonPill.getInstance(), null);
    }

    private void streamProcedure(Request r) {
        ActorRef streaming = context().actorOf(Props.create(StreamingActor.class), "streaming" + id);
        Request stream = new Request(MessageType.STREAM, r.getArg());

        streaming.tell(stream, getSender());
        streaming.tell(PoisonPill.getInstance(), null);
    }

    private static SupervisorStrategy strategy
            = new OneForOneStrategy(10, Duration.create("1 minute"), DeciderBuilder
            .match(IOException.class, e -> SupervisorStrategy.restart())
            .match(InterruptedException.class, e -> SupervisorStrategy.restart())
            .matchAny(o -> SupervisorStrategy.restart())
            .build());

    @Override
    public SupervisorStrategy supervisorStrategy () {
        return strategy;
    }
}

