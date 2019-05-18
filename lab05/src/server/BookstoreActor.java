package server;

import akka.Done;
import akka.NotUsed;
import akka.actor.AbstractActor;
import akka.actor.ActorSystem;
import akka.event.Logging;
import akka.event.LoggingAdapter;
import akka.stream.ActorMaterializer;
import akka.stream.Materializer;
import akka.stream.ThrottleMode;
import akka.stream.javadsl.Sink;
import akka.stream.javadsl.Source;
import messages.MessageType;
import messages.Request;
import messages.Response;
import scala.Int;
import scala.concurrent.duration.Duration;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.concurrent.CompletionStage;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class BookstoreActor extends AbstractActor {
    private final LoggingAdapter log = Logging.getLogger(getContext().getSystem(), this);
    private final Orderer orderer = new Orderer("orders.txt");

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    MessageType type = r.getType();
                    if (type == MessageType.FIND) {
                        int price = findTitle(r.getArg());
                        Response res = new Response(MessageType.FIND, price);
                        getSender().tell(res, null);
                    } else if (type == MessageType.ORDER) {
                        boolean result = orderTitle(r.getArg());
                        Response res = new Response(MessageType.ORDER, result);
                        getSender().tell(res, null);
                    } else if (type == MessageType.STREAM) {
                        streamTitle(r.getArg());
                    } else {
                        System.out.println("ayy");
                    }
                })
                .matchAny(o -> log.info("received unknown message"))
                .build();
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

    private boolean orderTitle(String title) throws InterruptedException {
        if (findTitle(title) == -1) return false;
        else return orderer.writeToFile(title);
    }

    private void streamTitle(String title) throws IOException, InterruptedException {
        if (findTitle(title) == -1) return;

        Materializer materializer = ActorMaterializer.create(getContext().getSystem());
        Stream<String> stream = Files.lines(Paths.get("./src/data/" + title + ".txt"));
        Source<String, NotUsed> source = Source.from(stream::iterator);

        source.map(o -> new Response(MessageType.STREAM, o.toString()))
                .throttle(1, Duration.create(1, TimeUnit.SECONDS), 1, ThrottleMode.shaping())
                .runWith(Sink.actorRef(getSender(), null), materializer);
    }
}

