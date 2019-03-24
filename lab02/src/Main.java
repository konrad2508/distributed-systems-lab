import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Main {
    public static void main(String[] args) throws Exception {
        System.setProperty("java.net.preferIPv4Stack", "true");
        DistributedMap client = new DistributedMap("channel1");

        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        while (true) {
            System.out.print("> ");
            System.out.flush();

            String line = in.readLine().toLowerCase();

            if (line.startsWith("quit") || line.startsWith("exit")) {
                break;

            } else if (line.startsWith("storage")) {
                System.out.println(client.getStorage().keySet());

            } else if (line.startsWith("cluster")) {
                System.out.println(client.getClusterStorage().keySet());

            } else if (line.startsWith("add")) {
                String[] splitted = line.split(" ");
                String key = splitted[1];
                try {
                    Integer value = Integer.parseInt(splitted[2]);

                    if (key == null) {
                        System.out.println("Invalid arguments. Usage: add <key> <value>.");
                    } else {
                        client.put(key, value);
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Invalid arguments. Usage: add <key> <value>.");
                }

            } else if (line.startsWith("get")) {
                String[] splitted = line.split(" ");
                String key = splitted[1];

                if (key == null) {
                    System.out.println("Invalid argument. Usage: get <key>.");
                } else {
                    System.out.println(client.get(key));
                }

            } else if (line.startsWith("remove")) {
                String[] splitted = line.split(" ");
                String key = splitted[1];

                if (key == null) {
                    System.out.println("Invalid argument. Usage: remove <key>.");
                } else {
                    System.out.println(client.remove(key));
                }
            }

        }
        client.disconnect();
    }
}
