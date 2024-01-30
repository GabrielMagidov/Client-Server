package bgu.spl.net.api;

import bgu.spl.net.srv.*;

public interface MessagingProtocol {
 
    /**
     * process the given message 
     * @param msg the received message
     * @return the response to send or null if no response is expected by the client
     */
    String process(String msg);
 
    /**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();

    void start(int connectionId, Connections connections);
 
}