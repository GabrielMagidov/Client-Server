package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        boolean isTPC = false;
        if(args[1].equals("tpc")){
            isTPC = true;
        }
        int port = Integer.parseInt(args[0]);
        if(isTPC){
            Server.threadPerClient(port,() -> new StompProtocol(), () -> new StompEncDec()).serve();
        }
        else{
            Server.reactor(Runtime.getRuntime().availableProcessors(),port, () -> new StompProtocol(),() -> new StompEncDec()).serve();
        }
    }
    
}
