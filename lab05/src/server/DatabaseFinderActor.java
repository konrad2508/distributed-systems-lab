package server;

import akka.actor.AbstractActor;

import java.io.BufferedReader;
import java.io.FileReader;

public class DatabaseFinderActor extends AbstractActor {
    private final String title;
    private final String database;

    DatabaseFinderActor(String title, String database) {
        this.title = title;
        this.database = database;
    }

    @Override
    public Receive createReceive() {
        return receiveBuilder()
                .matchAny(r -> {
                    BufferedReader reader = new BufferedReader(new FileReader("./src/data/" + database));
                    String record;
                    int titlePrice = -1;
                    while ((record = reader.readLine()) != null) {
                        if (record.startsWith(title)) {
                            String price = record.split(" ")[1];
                            titlePrice = Integer.parseInt(price);
                            break;
                        }
                    }
                    reader.close();
                    getSender().tell(titlePrice, null);
                })
                .build();
    }
}
