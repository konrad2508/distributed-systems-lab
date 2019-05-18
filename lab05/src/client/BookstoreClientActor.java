package client;

import akka.actor.AbstractActor;
import akka.actor.ActorSelection;
import akka.actor.Props;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import messages.Request;
import messages.MessageType;
import messages.Response;
import server.BookstoreActor;

public class BookstoreClientActor extends AbstractActor {
    private final LoggingAdapter log = Logging.getLogger(getContext().getSystem(), this);
    private final ActorSelection bookstore = getContext().actorSelection("akka.tcp://bookstore@127.0.0.1:2552/user/bookstore");

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(String.class, s -> {
                    MessageType type = null;
                    if (s.startsWith("find")) type = MessageType.FIND;
                    else if (s.startsWith("order")) type = MessageType.ORDER;
                    else if (s.startsWith("stream")) type = MessageType.STREAM;
                    else System.out.println(s);

                    try {
                        sendRequest(type, s);
                    } catch (ArrayIndexOutOfBoundsException e) {
                        System.out.println("Specify an argument");
                    } catch (NullPointerException e) {
                        System.out.println("Wrong type of operation");
                    }
                })
                .match(Response.class, r -> {
                    switch (r.getType()) {
                        case FIND:
                            if ((int) r.getArg() == -1)
                                System.out.println("Could not find specified title in the bookstore");
                            else System.out.println("Found specified title. Cost: " + r.getArg());
                            break;
                        case ORDER:
                            if ((boolean) r.getArg())
                                System.out.println("Successfully ordered the title");
                            else System.out.println("Failed to order the title");
                            break;
                        case STREAM:
                            System.out.println(r.getArg());
                            break;
                        default:
                            System.out.println("ayy");
                    }
                })
                .matchAny(o -> log.info("received unknown message"))
                .build();
    }

    // optional
    @Override
    public void preStart() {
        context().actorOf(Props.create(BookstoreActor.class), "bookstore");
    }

    private void sendRequest(MessageType type, String message) {
        if (type == null) throw new NullPointerException();

        String arg = message.split(" ")[1];
        Request req = new Request(type, arg);
        bookstore.tell(req, getSelf());
    }

}
