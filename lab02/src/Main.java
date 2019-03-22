import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Main {
    public static void main(String[] args) throws Exception {
        DistributedMap client = new DistributedMap("channel1");

        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        while (true) {
            System.out.print("> ");
            System.out.flush();

            String line = in.readLine().toLowerCase();
            if (line.startsWith("quit") || line.startsWith("exit")) break;
            else if (line.startsWith("storage")) {
                System.out.println(client.getStorage());
            } else if (line.startsWith("foreign")) {
                System.out.println(client.getForeignStorage());
            } else if (line.startsWith("add")) {
                String[] splitted = line.split(" ");
                String key = splitted[1];
                Integer value = Integer.parseInt(splitted[2]);

                client.put(key, value);
            } else if (line.startsWith("get")) {
                String[] splitted = line.split(" ");
                String key = splitted[1];

                System.out.println(client.get(key));
            }

        }
        client.disconnect();
    }
}
