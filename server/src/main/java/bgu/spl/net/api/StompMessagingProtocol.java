package bgu.spl.net.api;
import bgu.spl.net.impl.stomp.*;
//import bgu.spl.net.srv.Connections;

public interface StompMessagingProtocol {
	/**
	 * Used to initiate the current client protocol with it's personal connection ID and the connections implementation
	**/
    void start(int connectionId, StompConnections connections);
    
    void process(String msg);
	
	/**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();
}
