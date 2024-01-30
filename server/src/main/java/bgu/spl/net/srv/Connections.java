package bgu.spl.net.srv;

//import java.io.IOException;

public interface Connections {

    boolean send(int connectionId, String msg);

    void send(String channel, String msg);

    void disconnect(int connectionId);

    int getNextConnectionId();
}
