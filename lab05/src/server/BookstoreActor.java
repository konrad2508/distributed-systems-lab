package server;

import akka.actor.AbstractActor;
import akka.actor.ActorRef;
import akka.actor.Props;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import messages.MessageType;
import messages.Request;
import messages.Response;

public class BookstoreActor extends AbstractActor {
    private final LoggingAdapter log = Logging.getLogger(getContext().getSystem(), this);

    private ActorRef streaming = null;
    private ActorRef ordering = null;

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    MessageType type = r.getType();
                    int titlePrice = findTitle(r.getArg());

                    if (type == MessageType.FIND) {
                        Response res = new Response(MessageType.FIND, titlePrice);
                        getSender().tell(res, null);
                    } else if (type == MessageType.ORDER) {
                        if (titlePrice == -1){
                            Response res = new Response(MessageType.ORDER, false);
                            getSender().tell(res, null);
                        } else {
                            Request order = new Request(MessageType.ORDER, r.getArg());
                            ordering.tell(order, getSender());
                        }
                    } else if (type == MessageType.STREAM) {
                        if (titlePrice != -1){
                            Request stream = new Request(MessageType.STREAM, r.getArg());
                            streaming.tell(stream, getSender());
                        }
                    } else {
                        System.out.println("ayy");
                    }
                })
                .matchAny(o -> log.info("received unknown message"))
                .build();
    }

    @Override
    public void preStart() {
        streaming = context().actorOf(Props.create(StreamingActor.class), "streaming");
        ordering = context().actorOf(Props.create(OrderingActor.class), "ordering");
    }

    private int findTitle(String title) throws InterruptedException {
        Finder finder1 = new Finder(title, "db1.txt");
        Finder finder2 = new Finder(title, "db2.txt");

        Thread t1 = new Thread(finder1);
        Thread t2 = new Thread(finder2);

        t1.start();
        t2.start();

        t1.join();
        t2.join();

        return Math.max(finder1.getTitlePrice(), finder2.getTitlePrice());
    }

}

