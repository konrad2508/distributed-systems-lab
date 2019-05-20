package server;

import akka.actor.*;
import akka.pattern.Patterns;
import akka.util.Timeout;
import messages.MessageType;
import messages.Request;
import messages.Response;
import scala.concurrent.Await;
import scala.concurrent.Future;
import scala.concurrent.duration.Duration;

public class FindingActor extends AbstractActor {
    @Override
    public AbstractActor.Receive createReceive() {
        return receiveBuilder()
                .match(Request.class, r -> {
                    String title = (String) r.getArg();
                    ActorRef db1Finder = context().actorOf(Props.create(DatabaseFinderActor.class, title, "db1.txt"), "searchingDb1");
                    ActorRef db2Finder = context().actorOf(Props.create(DatabaseFinderActor.class, title, "db2.txt"), "searchingDb2");

                    Timeout timeout = new Timeout(Duration.create(5, "seconds"));
                    Future<Object> future1 = Patterns.ask(db1Finder, null, timeout);
                    Future<Object> future2 = Patterns.ask(db2Finder, null, timeout);

                    int result1 = (int) Await.result(future1, timeout.duration());
                    int result2 = (int) Await.result(future2, timeout.duration());

                    db1Finder.tell(PoisonPill.getInstance(), null);
                    db2Finder.tell(PoisonPill.getInstance(), null);

                    int foundPrice = Math.max(result1, result2);

                    Response res = new Response(MessageType.FIND, foundPrice);
                    getSender().tell(res, null);
                })
                .build();
    }

}
