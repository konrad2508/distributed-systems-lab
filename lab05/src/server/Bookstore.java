package server;

import akka.actor.ActorRef;
import akka.actor.ActorSystem;
import akka.actor.Props;
import com.typesafe.config.Config;
import com.typesafe.config.ConfigFactory;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;

public class Bookstore {

    public static void main(String[] args) throws Exception {
        final Config config = ConfigFactory.parseFile(new File("./src/config/bookstore.conf"));

        final ActorSystem system = ActorSystem.create("bookstore", config);
        final ActorRef actor = system.actorOf(Props.create(BookstoreActor.class), "bookstore");

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        while (true) {
            String line = br.readLine();
            if (line.equals("q")) {
                break;
            }
        }

        system.terminate();
    }
}
