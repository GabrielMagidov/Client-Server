package bgu.spl.net.impl.echo;

import bgu.spl.net.api.MessagingProtocol;
import java.time.LocalDateTime;
import bgu.spl.net.srv.Connections;

public class EchoProtocol implements MessagingProtocol {

    private boolean shouldTerminate = false;

    @Override
    public String process(String msg) {
        shouldTerminate = "bye".equals(msg);
        System.out.println("[" + LocalDateTime.now() + "]: " + msg);
        return createEcho(msg);
    }

    private String createEcho(String message) {
        String echoPart = message.substring(Math.max(message.length() - 2, 0), message.length());
        return message + " .. " + echoPart + " .. " + echoPart + " ..";
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    public void start(int connectionId, Connections connections){}
}
