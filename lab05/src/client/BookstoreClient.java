package client;

import akka.actor.ActorRef;
import akka.actor.ActorSystem;
import akka.actor.Props;
import com.typesafe.config.Config;
import com.typesafe.config.ConfigFactory;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;

public class BookstoreClient {
    public static void main(String[] args) throws Exception {
        final Config config = ConfigFactory.parseFile(new File("./src/config/bookstoreclient.conf"));

        final ActorSystem system = ActorSystem.create("client", config);
        final ActorRef actor = system.actorOf(Props.create(BookstoreClientActor.class), "client");
        System.out.println("Commands: 'find [name]', 'order [name]', 'stream [name]','q'");

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        while (true) {
            String line = br.readLine();
            if (line.equals("q")) {
                break;
            }
            actor.tell(line, null);     // send message to actor
        }

        system.terminate();
    }
}
