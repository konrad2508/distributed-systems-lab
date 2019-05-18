package server;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

class Orderer {
    private final String fileName;
    private final Object lock = new Object();

    Orderer(String fileName) {
        this.fileName = "./src/data/" + fileName;
    }

    boolean writeToFile(String text) {
        synchronized (lock) {
            BufferedWriter writer = null;
            try {
                writer = new BufferedWriter(new FileWriter(fileName));
                writer.append(text).append("\n");
                writer.close();
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }
        return true;
    }
}
