package de.naoth.rc.server;

import com.google.protobuf.ByteString;
import de.naoth.rc.messages.Messages.CMD;
import de.naoth.rc.messages.Messages.CMDArg;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.AsynchronousCloseException;
import java.nio.channels.NotYetConnectedException;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class handles all debug and connection stuff. Manager will register
 * itself here and all communication is done through this class.
 *
 * @author thomas
 */
public class MessageServer extends AbstractMessageServer {

    private class SingleExecEntry {
        private final ResponseListener listener;
        private final Command command;

        public SingleExecEntry(ResponseListener sender, Command command) {
            this.listener = sender;
            this.command = command;
        }
    }
    
    public final static String STRING_ENCODING = "ISO-8859-15";
    private final long updateIntervall = 33;

    private SocketChannel socketChannel;
    private InetSocketAddress address;
    private ScheduledExecutorService scheduledExecutorService;
    //private ScheduledFuture sendReceiveTask;

    // list of requested commands to be sent once (can be modified from outside)
    private final List<SingleExecEntry> pendingCommandsList = Collections.synchronizedList(new LinkedList<SingleExecEntry>());
    // list of requested commands to be executed periodically (can be modified from outside)
    private final List<CommandSender> pendingSubscribersList = Collections.synchronizedList(new LinkedList<CommandSender>());
    // list of commands which have been sent and are waiting for the response
    //private final List<SingleExecEntry> answerRequestList = new LinkedList<SingleExecEntry>();
    private final Map<Integer, SingleExecEntry> answerRequestMap = Collections.synchronizedMap(new HashMap<Integer, SingleExecEntry>());

    // each sent command has a unique id, which is used to assign the responces correctly
    private Integer commandId = 1;
    
    // just for statistics
    private long receivedBytes;
    private long sentBytes;
    private long loopCount;
    
    // heart beat sent in fixed intervals
    private long timeOfLastHeartBeat = System.currentTimeMillis();

    private final int connectionTimeout = 1000;
    
    public MessageServer() {
        this.socketChannel = null;
        this.receivedBytes = 0;
        this.sentBytes = 0;
        this.loopCount = 0;
    }

    public boolean connect(String host, int port) {
        return connect(new InetSocketAddress(host, port));
    }
    
    public boolean connect(InetSocketAddress address) {
        // close previous connection if necessary
        disconnect();
        
        try {
            this.address = address;

            // open and configure the socket
            this.socketChannel = SocketChannel.open();
            this.socketChannel.configureBlocking(true);

            //this.socketChannel.connect(address);
            this.socketChannel.socket().connect(address, this.connectionTimeout);

            this.scheduledExecutorService = Executors.newSingleThreadScheduledExecutor();
            this.scheduledExecutorService.scheduleAtFixedRate(new Runnable() {
                @Override
                public void run() {
                    sendReceiveLoop();
                }
            }, 0, updateIntervall, TimeUnit.MILLISECONDS);

            this.fireConnected(this.address);
            
            return true;
        } catch (SocketTimeoutException ex) {
            //e.printStackTrace(System.err);
            fireDisconnected(String.format("Connection atttimed out. Robot didn't respond after %d ms.", connectionTimeout));
        } catch (IOException e) {
            //e.printStackTrace(System.err);
            fireDisconnected(e.getLocalizedMessage());
        }
        
        return false;
    }//end connect

    public boolean reconnect() {
        return connect(this.address);
    }
    
    public void disconnect() {
        disconnect(null);
    }

    private void disconnect(String message) {
        if (!isConnected()) {
            return;
        }

        // stop the server thread and wait for it to die (max 2s)
        this.scheduledExecutorService.shutdown();
        try {
            if(!this.scheduledExecutorService.awaitTermination(2, TimeUnit.SECONDS)) {
                Logger.getLogger(MessageServer.class.getName()).log(Level.WARNING,
                    "The server thread couldn't be stopped gracefully.");
            }
        } catch (InterruptedException e) {  }

        // close the soccet
        try {
            socketChannel.close();
        } catch (IOException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
                    "Unexpected exception while closing the socket.", ex);
        }
        socketChannel = null;
        
        // NOTE: we expect the server thread to be dead by now
        //       either due to the executor service shutdown or
        //       because it tried to read from the closed socket.
        //       So, we don't synchronize the answerRequestList.
        
        // call error handlers of remaining requests
        for (SingleExecEntry a : answerRequestMap.values()) {
            a.listener.handleError(-2);
        }
        answerRequestMap.clear();

        MessageServer.this.fireDisconnected(message);
    }//end disconnect

    public InetSocketAddress getAddress() {
        return address;
    }

    /**
     * @return whether RC is connected to a robot
     */
    public boolean isConnected() {
        return socketChannel != null && socketChannel.isConnected();
    }
    
    @Deprecated
    public List<CommandSender> getListeners() {
        return new ArrayList<CommandSender>();
    }

    public long getReceivedBytes() {
        return receivedBytes;
    }

    public long getSentBytes() {
        return sentBytes;
    }
    
    public long getLoopCount() {
        return loopCount;
    }

    /**
   * Add a {@link CommandSender} to the repeating schedule.
   * A {@link CommandSender} will not be added twice.
   * @param commandSender
   */
    public void subscribe(CommandSender commandSender)
            throws NotYetConnectedException, IllegalArgumentException {
        if (!isConnected()) {
            throw new NotYetConnectedException();
        }

        if (commandSender == null || commandSender.getCurrentCommand() == null) {
            throw new IllegalArgumentException("A valid command is required.");
        }

        if(!this.pendingSubscribersList.contains(commandSender)) {
            this.pendingSubscribersList.add(commandSender);
        }
    }//end subscribe
    
    /**
    * Remove a {@link CommandSender} from the repeating schedule.
    * @param commandSender
    */
    public void unsubscribe(CommandSender commandSender) {
        this.pendingSubscribersList.remove(commandSender);
    }
    
    /**
     * Schedule a single command for execution. It is not guaranteed when it
     * will be executed but the {@link ResponseListener} will be notified with 
     * the command and the response data as arguments.
     *
     * @param listener This listener will be called when the response has arrived
     * @param command Command to be sent
     *
     * @throws NotYetConnectedException is thrown if the server is not connected
     */
    public void executeCommand(ResponseListener listener, Command command)
            throws NotYetConnectedException, IllegalArgumentException {
        if (!isConnected()) {
            throw new NotYetConnectedException();
        }

        if (listener == null || command == null) {
            throw new IllegalArgumentException("A valid command or listener is required.");
        }

        this.pendingCommandsList.add(new SingleExecEntry(listener, command));
    }//end executeSingleCommand

    @Deprecated
    public void executeSingleCommand(CommandSender commandSender, Command command)
            throws NotYetConnectedException {
        executeCommand(commandSender, command);
    }

    @Deprecated
    public void executeSingleCommand(CommandSender commandSender)
            throws NotYetConnectedException {
        executeCommand(commandSender, commandSender.getCurrentCommand());
    }

    // send-receive-loop //
    private void sendReceiveLoop() {
        try {
            if (isConnected()) {
                
                // read the answers for all the requests in answerRequestQueue
                pollAnswers();
                
                // send new commands
                // NOTE: this is a approximation:
                //       send new commands only when the old answers have been received
                if(this.answerRequestMap.size() < this.pendingSubscribersList.size() + this.pendingCommandsList.size()) {
                    sendPendingCommands();
                }

                // send heart beat
                sendHeartBeat();
                
                this.loopCount++;
            }
        } catch (AsynchronousCloseException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.INFO, "Connection was closed while trying to read.", ex);
            disconnect(ex.getMessage());
        } catch (IOException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
                    "Unexpected exception...", ex);
            disconnect(ex.getMessage());
        } catch (Exception ex) { // this is needed for possible null exception and such
            Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
                    "Unexpected exception...", ex);
            disconnect(ex.getMessage());
        }
    }//end sendReceiveLoop

    private void sendHeartBeat() throws IOException {
        if(System.currentTimeMillis() > timeOfLastHeartBeat + 1000) {
            timeOfLastHeartBeat = System.currentTimeMillis();
            sentBytes += socketChannel.write(byteBufferFromInt(-1));
        }
    }
    
    private void sendPendingCommands() throws IOException {
        socketChannel.configureBlocking(true);
        
        // handle the single execution requests
        synchronized (this.pendingCommandsList) {
            for (SingleExecEntry e : this.pendingCommandsList) {
                Integer id = sendCommand(socketChannel, e.command);
                //answerRequestList.add(e);
                answerRequestMap.put(id, e);
            }
            this.pendingCommandsList.clear();
        }
        
        // handle the periodic execution requests
        synchronized (this.pendingSubscribersList) {
            for (CommandSender c : this.pendingSubscribersList) {
                SingleExecEntry e = new SingleExecEntry(c, c.getCurrentCommand());
                Integer id = sendCommand(socketChannel, e.command);
                //answerRequestList.add(e);
                answerRequestMap.put(id, e);
            }
        }
        
    }//end sendPendingCommands

    private void pollAnswers() throws IOException {
        //socketChannel.configureBlocking(true);
        
        //Iterator<SingleExecEntry> i = answerRequestList.iterator();
        while (!answerRequestMap.isEmpty()) 
        {
            //SingleExecEntry e = i.next();
            
            // read size of data
            socketChannel.configureBlocking(false);
            ByteBuffer idBuffer = readContent(socketChannel, 4);
            if (idBuffer == null) {
                break;
            }
            int id = idBuffer.getInt();
            
            if (id == 0) {
                // shouldn't be happening anymore, "commandId" is initialized with "1"!
                Logger.getLogger(MessageServer.class.getName()).log(Level.WARNING,
                    "Ignore illegal response ID '0'. Why is this happening?!!");
                break;
            }
            
            // read the rest of the data
            socketChannel.configureBlocking(true);
            
            ByteBuffer sizeBuffer = readContent(socketChannel, 4);
            if (sizeBuffer == null) {
                break;
            }
            int size = sizeBuffer.getInt();
            
            SingleExecEntry e = answerRequestMap.remove(id);
            if (e == null) {
                throw new IOException("Illegal response ID: " + id);
            }
            
            ByteBuffer data = readContent(socketChannel, size);
            if (data == null) {
                throw new IOException("No data could be read from the socket.");
            }
            
            // call responce handler
            try {
                e.listener.handleResponse(data.array(), e.command);
            } catch (Exception ex) {
                Logger.getLogger(MessageServer.class.getName()).log(Level.WARNING,
                    "Exception while handling response to " + e.command, ex);
            }

            //i.remove();
        }//end for
    }//end pollAnswers

    // helpers
    private Integer sendCommand(SocketChannel socket, Command command) throws IOException {
        // convert to Protobuf
        CMD.Builder cmd = CMD.newBuilder().setName(command.getName());

        if (command.getArguments() != null) {
            for (Map.Entry<String, byte[]> e : command.getArguments().entrySet()) {
                CMDArg.Builder arg = CMDArg.newBuilder().setName(e.getKey());
                if (e.getValue() != null) {
                    arg = arg.setBytes(ByteString.copyFrom(e.getValue()));
                }
                cmd = cmd.addArgs(arg);
            }
        }

        byte[] commandAsByteArray = cmd.build().toByteArray();
        int intLength = commandAsByteArray.length;
        int currentCommandId = commandId;

        ByteBuffer bId = byteBufferFromInt(currentCommandId);
        ByteBuffer bLength = byteBufferFromInt(intLength);
        ByteBuffer bCommand = ByteBuffer.wrap(commandAsByteArray);

        // write id    
        sentBytes += socket.write(bId);
        // write length    
        sentBytes += socket.write(bLength);
        // write data
        sentBytes += socket.write(bCommand);
        
        commandId = (commandId + 1) % 1000000;
        
        return currentCommandId;
    }//end sendCommand

    private ByteBuffer readContent(SocketChannel socket, int length) throws IOException {
        if (length < 0) {
            throw new IOException("invalid message length: " + length);
        }
        if (length > 10485760) { // more than 10 MB
            throw new IOException("Message size received bigger than 10 MB, this is too big! Will disconnect.");
        }
        if (socket == null) {
            throw new IOException("Socket is null.");
        }

        ByteBuffer buffer = ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN);

        int byteCountTotal = 0;
        int byteCountSingle = 0;
        while ((byteCountSingle = socket.read(buffer)) > 0) {
            byteCountTotal += byteCountSingle;
        }

        // accumulate the size of received data
        receivedBytes += byteCountTotal;

        if (byteCountTotal == length) {
            buffer.flip();
            return buffer;
        } else if (byteCountSingle == -1) {
            throw new IOException("socket stream closed");
        } else {
            return null;
        }
    }//end readContent

    public static ByteBuffer byteBufferFromInt(int value) {
        ByteBuffer b = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
        b.putInt(value);
        b.flip();
        return b;
    }
}//end class MessageServer
