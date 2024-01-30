package bgu.spl.net.impl.stomp;


import java.util.LinkedList;

import bgu.spl.net.api.StompMessagingProtocol;

public class StompProtocol implements StompMessagingProtocol {
    
    private boolean shouldTerminate;
    private StompConnections connections;
    private int connectionId;

    public StompProtocol(){
        shouldTerminate = false;
    }

    public void start(int connectionId, StompConnections connections){
        this.connections = connections;
        this.connectionId = connectionId;
    }
    
    public void process(String message){
        String[] lines = message.split("\n");
        String commend = lines[0];
        String[][] f = new String[lines.length - 1][2];
        String receipt = "";
        for(int i = 0; i < lines.length - 1; i++){
            f[i] = lines[i+1].split(":");
            if(f[i][0].indexOf("receipt") >= 0){
                receipt = f[i][1];
            }
        }
        if(commend.equals("CONNECT")){
            shouldTerminate = connect(f,message,receipt);
            if(receipt.length() > 0 && !shouldTerminate){
                connections.send(connectionId, "RECEIPT\nreceipt-id:"+receipt+"\n\n");
            }
        }
        else if(commend.equals("SUBSCRIBE")){
            shouldTerminate = subscribe(f, message, receipt);
            if(receipt.length() > 0 && !shouldTerminate){
                connections.send(connectionId, "RECEIPT\nreceipt-id:"+receipt+"\n\n");
            }
        }
        else if(commend.equals("UNSUBSCRIBE")){
            shouldTerminate = unSubscribe(f, message, receipt);
            if(receipt.length() > 0&& !shouldTerminate){
                connections.send(connectionId, "RECEIPT\nreceipt-id:"+receipt+"\n\n");
            }
        }
        else if(commend.equals("SEND")){
            shouldTerminate = send(f, lines, message, receipt, connectionId);
            if(receipt.length() > 0&& !shouldTerminate){
                connections.send(connectionId, "RECEIPT\nreceipt-id:"+receipt+"\n\n");
            }
        }
        else if(commend.equals("DISCONNECT")){
            if(receipt.length() > 0&& !shouldTerminate){
                try {
                    connections.send(connectionId, "RECEIPT\nreceipt-id:"+receipt+"\n\n");
                } catch (Exception e) {}
            }
            shouldTerminate = true;
            connections.disconnect(connectionId);
            if(connections.subscriberToTopics.containsKey(connectionId)){
                for(String[] temp : connections.subscriberToTopics.get(connectionId)){
                    if(temp != null){
                        String topic = temp[0];
                        LinkedList<Integer[]> lst = connections.topicToSubscribers.get(topic);
                        for(Integer[] temp2 : lst){
                            if(temp2 != null){
                                if(temp2[0] == connectionId){
                                    lst.remove(temp2);
                                }
                            }   
                        }
                        connections.topicToSubscribers.put(topic, lst);
                    }
                }
            }
            connections.subscriberToTopics.remove(connectionId);
        }
        else{
            sendError(message, receipt, "The frame is illegal");
        }
    }

    private boolean connect(String[][] f, String message, String receipt){
        boolean error = false;
        String login = "";
        String passcode = "";
        int loginC = 0;
        int hostC = 0;
        int passC = 0;
        int verC = 0;
        
        for(int i = 0; i < f.length && !error; i++){
            if(f[i][0] != null && f[i][0].equals("accept-version")){
                if(f[i][1] != null && f[i][1].equals("1.2")){
                    verC++;
                }
                else{
                    sendError(message, receipt, "The version isn't equal");
                    error = true;
                    break;
                }
            }
            if(f[i][0] != null && f[i][0].equals("host")){
                if(f[i][1].equals("stomp.cs.bgu.ac.il")){
                    hostC++;
                }
                else{
                    sendError(message, receipt, "The host isn't equal");
                    error = true;
                    break;
                }
            }
            if(f[i][0] != null && f[i][0].equals("login")){
                login = f[i][1];
                loginC++;
            }
            if(f[i][0] != null && f[i][0].equals("passcode")){
                passcode = f[i][1];
                passC++;
            }
        }
        if(verC != 1 || hostC!=1 || passC!=1 || loginC != 1){
            sendError(message, receipt, "There is too many headers or Some of the headers are missing");
            error = true;
        }
        if(!error){
            if(connections.userNameToPasscode.containsKey(login)){
                if(connections.userNameToPasscode.get(login).equals(passcode)){
                    if(!connections.userNameToState.get(login)){
                        connections.userNameToState.put(login, true);
                        connections.userIdToName.put(connectionId, login);
                        connections.send(connectionId, "CONNECTED\nversion:1.2\n\n");
                    }
                    else{
                        sendError(message, receipt, "User already logged in");
                        error = true;
                    }
                }
                else {
                    sendError(message, receipt, "Wrong password");
                    error = true;
                }
            }
            else {
                connections.userNameToPasscode.put(login, passcode);
                connections.userIdToName.put(connectionId, login);
                connections.userNameToState.put(login, true);
                connections.send(connectionId, "CONNECTED\nversion:1.2\n\n");
            }
        }
    return error;
    }

    private boolean subscribe(String[][] f, String massage, String receipt){
        boolean error = false;
        String dest = "";
        String id = "";
        for(int i = 0; i < f.length; i++){
            if(f[i][0].equals("destination")){
                dest = f[i][1];
            }
            if(f[i][0].equals("id")){
                id = f[i][1];
            }
        }
        if((dest.length() == 0)){
            sendError(massage, receipt, "The destination is missing");
            error = true;
        }
        if(!error){
            LinkedList<String[]> lst1 = new LinkedList<String[]>();
            LinkedList<Integer[]> lst2 = new LinkedList<Integer[]>();
            if (connections.subscriberToTopics.containsKey(connectionId)){
                lst1 = connections.subscriberToTopics.get(connectionId);
            }
            if(connections.topicToSubscribers.containsKey(dest)){
                lst2 = connections.topicToSubscribers.get(dest);
            }
            Integer Id = -1;
            if(id.length() > 0){
                try{
                    Id = Integer.valueOf(id);
                }
                catch(Exception e){}
            }
            if(id.length() > 0 && Id == -1){
                sendError(massage, receipt, "The id isn't number");                   
                error = true;
            }
            else{
                Integer[] toAdd2 = {(Integer)connectionId, Id};
                String[] toAdd = {dest, id};
                lst1.add(toAdd);
                lst2.add(toAdd2);
                connections.topicToSubscribers.put(dest, lst2);
                connections.subscriberToTopics.put(connectionId, lst1);
            }
        }
        return error;
    }
    
    private boolean unSubscribe(String[][] f, String massage, String receipt){
        boolean error = false;
        String id = "";
        for(int i = 0; i < f.length; i++){
            if(f[i][0].equals("id")){
                if(f[i].length > 1){
                    id = f[i][1];
                }
            }
        }
        if(id.length() == 0){
            sendError(massage, receipt, "The id is missing");                    
            error = true;
        }
        if(!error){
            Integer Id = -1;
            if(id.length() > 0){
                try{
                    Id = Integer.valueOf(id);
                }
                catch(Exception e){}
            }
            if(id.length() > 0 && Id == -1){
                sendError(massage, receipt, "The id isn't number");                      
                error = true;
            }
            else{
                LinkedList<String[]> lst1 = connections.subscriberToTopics.get(connectionId);
                String topic = "";
                String[] temp = {"",""};
                for(String[] arr:lst1){
                    if(arr != null && arr[1].equals(id)){
                        topic = arr[0];
                        temp = arr;
                    }
                }
                if(topic.length() == 0){
                    sendError(massage, receipt, "The user isn't subscribed to the topic");                     
                    error = true;
                }
                else{
                    lst1.remove(temp);
                    if(lst1.isEmpty()){
                        connections.subscriberToTopics.remove(connectionId);
                    }
                    else{
                        connections.subscriberToTopics.put(connectionId,lst1);
                    }
                    LinkedList<Integer[]> lst = connections.topicToSubscribers.get(topic);
                    for(Integer[] toRemove : lst){
                        if(toRemove[0] == connectionId){
                            lst.remove(toRemove);
                        }
                    }
                    if(lst.isEmpty()){
                        connections.topicToSubscribers.remove(topic);
                    }
                    else{
                        connections.topicToSubscribers.put(topic, lst);
                    }
                }
            }  
        }
        return error;
    }

    public boolean send(String[][] f, String[] lines, String massage, String receipt, int connectionId){
        boolean error = false;
        String dest = "";
        int index = 0;
        for(int i = 0; i < f.length; i++){
            if(f[i][0].equals("destination")){
                dest = f[i][1];
                index = i;
            }
        }
        if(dest.length() == 0){
            sendError(massage, receipt, "The destination is missing");                     
            error = true;
        }
        if(!error){
            boolean found =false;
            LinkedList<Integer[]> lst = connections.topicToSubscribers.get(dest);
            if(lst != null){
                for(Integer[] temp : lst){
                    if(temp[0] == connectionId){
                        found = true;
                    }
                }
            }
            if(!found){
                sendError(massage, receipt, "The user isn't subscribed to the channel");                    
                error = true;
            }
            else{
                    String msg = "";
                    for(int i = index + 3; i < lines.length; i++){
                        msg = msg + lines[i] +"\n";
                    }
                    connections.send(dest, msg);
            }
        }
        return error;
    }
    
    private void sendError(String message, String receipt, String reason){
        if(connections.subscriberToTopics.containsKey(connectionId)){
            for(String[] temp : connections.subscriberToTopics.get(connectionId)){
                if(temp != null){
                    String topic = temp[0];
                    LinkedList<Integer[]> lst = connections.topicToSubscribers.get(topic);
                    for(Integer[] temp2 : lst){
                        if(temp2 != null){
                            if(temp2[0] == connectionId){
                                lst.remove(temp2);
                            }
                        }
                    }
                connections.topicToSubscribers.put(topic, lst);
                }
            }
        }
        connections.subscriberToTopics.remove(connectionId);
        String error = "";
        if(receipt.length() == 0){
            error = "ERROR\nThe message:\n-----\n"+message+"\n-----\n"+reason+"\n\n";
        }
        else{
            error = "ERROR\nreceipt-id:"+receipt+"\n\nThe message:\n-----\n"+message+"\n-----\n"+reason+"\n\n";
        }
        connections.send(connectionId, error);
        shouldTerminate = true;
        connections.disconnect(connectionId); 
    }

    public boolean shouldTerminate(){
        return shouldTerminate;
    }
}
