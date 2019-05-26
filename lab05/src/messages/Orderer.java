package messages;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

public class Orderer {
    private final String fileName;
    private final Object lock = new Object();

    public Orderer(String fileName) {
        this.fileName = "./src/data/" + fileName;
    }

    public boolean writeToFile(String text) throws IOException {
        synchronized (lock) {
            BufferedWriter writer = null;
            writer = new BufferedWriter(new FileWriter(fileName, true));
            writer.append(text).append("\n");
            writer.close();
        }
        return true;
    }
}
