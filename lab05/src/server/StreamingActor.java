package server;

import akka.NotUsed;
import akka.actor.AbstractActor;
import akka.stream.ActorMaterializer;
import akka.stream.Materializer;
import akka.stream.ThrottleMode;
import akka.stream.javadsl.Sink;
import akka.stream.javadsl.Source;
import messages.MessageType;
import messages.Request;
import messages.Response;
import scala.concurrent.duration.Duration;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.concurrent.TimeUnit;
import java.util.stream.Stream;

public class StreamingActor extends AbstractActor {

    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    Materializer materializer = ActorMaterializer.create(getContext().getSystem());
                    Stream<String> stream = Files.lines(Paths.get("./src/data/" + r.getArg() + ".txt"));
                    Source<String, NotUsed> source = Source.from(stream::iterator);

                    source.map(o -> new Response(MessageType.STREAM, o.toString()))
                            .throttle(1, Duration.create(1, TimeUnit.SECONDS), 1, ThrottleMode.shaping())
                            .runWith(Sink.actorRef(getSender(), null), materializer);
                })
                .build();
    }
}
