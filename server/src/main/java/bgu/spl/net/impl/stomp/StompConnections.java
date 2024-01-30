package bgu.spl.net.impl.stomp;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.BlockingConnectionHandler;

public class StompConnections implements Connections {
    
    int connectionsCounter;
    int messageCounter;
    protected HashMap<Integer, String> userIdToName = new HashMap<Integer, String>();
    protected HashMap<String, String> userNameToPasscode = new HashMap<String, String>();
    protected HashMap<String, Boolean> userNameToState = new HashMap<String, Boolean>();
    protected HashMap<ConnectionHandler, Integer> handlerToId = new HashMap<ConnectionHandler, Integer>();
    protected HashMap<Integer, ConnectionHandler> idToHandler = new HashMap<Integer, ConnectionHandler>();
    protected HashMap<String, LinkedList<Integer[]>> topicToSubscribers = new HashMap<String, LinkedList<Integer[]>>();
    protected HashMap<Integer, LinkedList<String[]>> subscriberToTopics = new HashMap<Integer, LinkedList<String[]>>();
    protected HashMap<Integer, String> mIdToMessage = new HashMap<Integer, String>();

    public StompConnections(){
        connectionsCounter = 0;
        messageCounter = 0;
    }

    public int getNextConnectionId(){
        int nextConnectionId = connectionsCounter;
        connectionsCounter++;
        return nextConnectionId;
    }

    public void Connect(int id, ConnectionHandler handler){
        handlerToId.put(handler, id);
        idToHandler.put(id, handler);
    }

    public synchronized boolean send(int connectionId, String msg){
        ConnectionHandler handler = idToHandler.get(connectionId);
        if(handler != null){
            try {
                handler.send(msg);
            } catch (Exception e) {}
        }
        return true;
    }


    public void send(String channel, String message){
        LinkedList<Integer[]> lst = topicToSubscribers.get(channel);
        for(Integer[] temp : lst){
            if(temp != null && temp[1] == -1){
                String msg = "MESSAGE\nmessage-id:"+messageCounter+"\ndestination:"+channel+"\n\n"+message+"\n";
                mIdToMessage.put(messageCounter, msg);
                send(temp[0], msg);
                messageCounter++;
            }
            else if(temp != null){
                String msg = "MESSAGE\nsubsciption:"+temp[1]+"\nmessage-id:"+messageCounter+"\ndestination:"+channel+"\n\n"+message+"\n";
                mIdToMessage.put(messageCounter, msg);
                send(temp[0], msg);
                messageCounter++;
            }
        }
        
    }

    public void disconnect(int connectionId){
        ConnectionHandler handler = idToHandler.get(connectionId);
        if(handler != null){
            handlerToId.remove(handler);
            idToHandler.remove(connectionId);
            String name = userIdToName.get(connectionId);
            userNameToState.put(name, false);
            if(handler instanceof BlockingConnectionHandler){
                try{
                    handler.close();
                }
                catch(IOException e){}
            }
        }
    }
}
