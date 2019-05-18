package server;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class Finder implements Runnable {

    private final String title;
    private final String databaseName;
    private int titlePrice = -1;

    Finder(String title, String databaseName) {
        this.title = title;
        this.databaseName = databaseName;
    }

    @Override
    public void run() {
        try {
            BufferedReader reader = new BufferedReader(new FileReader("./src/data/" + databaseName));
            String record;
            while ((record = reader.readLine()) != null) {
                if (record.startsWith(title)) {
                    String price = record.split(" ")[1];
                    titlePrice = Integer.parseInt(price);
                    break;
                }
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    int getTitlePrice() {
        return titlePrice;
    }
}
