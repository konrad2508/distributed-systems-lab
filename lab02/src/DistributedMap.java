import com.google.protobuf.InvalidProtocolBufferException;
import org.jgroups.*;
import org.jgroups.util.Util;
import protos.MessageObjectProtos;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class DistributedMap extends ReceiverAdapter implements SimpleStringMap {
    private JChannel channel;
    private Address ownAddress;
    private HashMap<String, Integer> storage;
    private HashMap<String, Address> foreignStorage;
    //    private HashMap<String, Address> peers;
    private List<Address> oldAddr;

    private Integer toRet;

    public DistributedMap(String channelName) throws Exception {
        this.channel = new JChannel();
        this.storage = new HashMap<>();
        this.foreignStorage = new HashMap<>();

        channel.setReceiver(this);
        channel.connect(channelName);
        channel.getState(null, 10000);
    }

    @Override
    public boolean containsKey(String key) {
        return (storage.containsKey(key) || foreignStorage.containsKey(key));
    }

    @Override
    public Integer get(String key) {
        if (storage.containsKey(key)) {
            return storage.get(key);
        } else {
            Address dst = foreignStorage.get(key);

            MessageObjectProtos.MessageObject msgObj = MessageObjectProtos.MessageObject.newBuilder()
                    .setType(MessageObjectProtos.MessageObject.MessageObjectType.GET_ELEMENT)
                    .setKey(key)
                    .build();
            byte[] toSend = msgObj.toByteArray();
            Message msg = new Message(dst, null, toSend);
            toRet = null;

            try {
                channel.send(msg);
            } catch (Exception e) {
                e.printStackTrace();
            }

            while (true) {
                if (toRet != null) {
                    return toRet;
                }

                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    @Override
    public void put(String key, Integer value) {
        storage.put(key, value);
        foreignStorage.put(key, ownAddress);
        MessageObjectProtos.MessageObject msgObj = MessageObjectProtos.MessageObject.newBuilder()
                .setType(MessageObjectProtos.MessageObject.MessageObjectType.NEW_ELEMENT)
                .setKey(key)
                .build();
        byte[] toSend = msgObj.toByteArray();
        Message msg = new Message(null, null, toSend);

        try {
            channel.send(msg);
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    @Override
    public Integer remove(String key) {
        return null;
    }

    public void getState(OutputStream output) throws Exception {
        synchronized (foreignStorage) {
            Util.objectToStream(foreignStorage, new DataOutputStream(output));
        }
    }

    public void setState(InputStream input) throws Exception {
        HashMap<String, Address> map;
        map = (HashMap<String, Address>) Util.objectFromStream(new DataInputStream(input));
        synchronized (foreignStorage) {
            foreignStorage.clear();
            foreignStorage.putAll(map);
        }
    }

    public void viewAccepted(View new_view) {
        if (ownAddress == null) {
            ownAddress = new_view.getMembers().get(new_view.getMembers().size() - 1);
        }

        if (oldAddr != null && oldAddr.size() > new_view.size()) {
            List<Address> newAddr = new_view.getMembers();

            List<Address> toReturn = new ArrayList<>(oldAddr);
            toReturn.removeAll(newAddr);

            for (Address addr : toReturn) {
                foreignStorage.values().remove(addr);
            }

        }

        oldAddr = new_view.getMembers();

    }

    public void receive(Message msg) {
        byte[] buf = msg.getBuffer();

        MessageObjectProtos.MessageObject msgObject = null;
        try {
            msgObject = MessageObjectProtos.MessageObject.parseFrom(buf);
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }

        String key = msgObject.getKey();

        switch (msgObject.getType()) {
            case GET_ELEMENT:
                MessageObjectProtos.MessageObject msgReplyObj = MessageObjectProtos.MessageObject.newBuilder()
                        .setType(MessageObjectProtos.MessageObject.MessageObjectType.RET_ELEMENT)
                        .setKey(key)
                        .setValue(storage.get(key))
                        .build();
                byte[] toSend = msgReplyObj.toByteArray();
                Message msgReply = new Message(msg.getSrc(), null, toSend);

                try {
                    channel.send(msgReply);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                break;

            case RET_ELEMENT:
                toRet = msgObject.getValue();

                break;

            case NEW_ELEMENT:
                foreignStorage.put(key, msg.getSrc());
                break;
        }

    }

    public HashMap<String, Integer> getStorage() {
        return this.storage;
    }

    public HashMap<String, Address> getForeignStorage() {
        return this.foreignStorage;
    }

    public void disconnect() {
        channel.close();
    }
}
