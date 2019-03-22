import org.jgroups.*;
import org.jgroups.stack.IpAddress;
import org.jgroups.util.UUID;
import org.jgroups.util.Util;

import java.io.*;
import java.net.Inet6Address;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Scanner;
import java.util.stream.Collectors;

public class Client extends ReceiverAdapter {

    List<Address> oldAddr = null;
    List<Address> addressList = null;
    JChannel channel;
    String username;
    Address ownAddress;

    public static void main(String[] args) throws Exception {
        System.setProperty("java.net.preferIPv4Stack", "true");
        new Client().start();
    }

    private void start() throws Exception {
        System.out.print("Enter your username: ");
        Scanner scanner = new Scanner(System.in);
        username = scanner.nextLine();

        channel = new JChannel(); // use the default config, udp.xml

        channel.setReceiver(this);

        channel.connect("my_cluster");

//        channel.getState(null, 10000);

        if (addressList == null) {
            addressList = new ArrayList<>();
            addressList.add(ownAddress);
        }

        eventLoop();

        channel.close();

    }

    private void eventLoop() {

        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

        while (true) {

            try {

                System.out.print("> ");
                System.out.flush();

                String line = in.readLine().toLowerCase();

                if (line.startsWith("quit") || line.startsWith("exit"))

                    break;

                if (line.startsWith("state")) {
                    System.out.println(addressList);
                } else {

                    line = "[" + username + "] " + line;

                    View view = channel.getView();
                    Address addr = view.getMembers().get(view.getMembers().size() - 1);

//                IpAddress addr = new IpAddress(address);

                    Message msg = new Message(addr, null, line);

//                System.out.println(msg.getDest());

                    channel.send(msg);
                }
            } catch (Exception e) {

            }

        }

    }

    public void getState(OutputStream output) throws Exception {
        synchronized (addressList) {
            Util.objectToStream(addressList, new DataOutputStream(output));
        }
    }

    public void setState(InputStream input) throws Exception {
        List<Address> list;
        list = (List<Address>) Util.objectFromStream(new DataInputStream(input));
        synchronized (addressList) {
            System.out.println(list);
            addressList.clear();
            addressList.addAll(list);
        }
    }

    public void viewAccepted(View new_view) {
        if (ownAddress == null) {
            ownAddress = new_view.getMembers().get(new_view.getMembers().size() - 1);
            System.out.println("** own address: " + ownAddress);
        }

        if (oldAddr == null || oldAddr.size() < new_view.size()) {
            System.out.println("View increased in size");
        } else {
            System.out.println("View decreased in size");

            List<Address> newAddr = new_view.getMembers();

            List toReturn = new ArrayList(oldAddr);
            toReturn.removeAll(newAddr);

            System.out.println("** has left: " + toReturn);

        }

        System.out.println("** view: " + new_view);
        oldAddr = new_view.getMembers();
    }

    public void receive(Message msg) {

        System.out.println(msg.getSrc() + ": " + msg.getObject());

    }
}
