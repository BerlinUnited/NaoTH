package de.naoth.rc.server;

import com.google.protobuf.ByteString;
import de.naoth.rc.messages.Messages.CMD;
import de.naoth.rc.messages.Messages.CMDArg;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.AsynchronousCloseException;
import java.nio.channels.NotYetConnectedException;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
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
    private Thread senderThread;
    private boolean isActive;
    private InetSocketAddress address;

    // list of requested commands to be sent once (can be modified from outside)
    private final List<SingleExecEntry> pendingCommandsList = Collections.synchronizedList(new LinkedList<SingleExecEntry>());
    // list of requested commands to be executed periodically (can be modified from outside)
    private final List<CommandSender> pendingSubscribersList = Collections.synchronizedList(new LinkedList<CommandSender>());
    // list of commands which have been sent and are waiting for the response
    private final List<SingleExecEntry> answerRequestList = new LinkedList<SingleExecEntry>();

    
    private long receivedBytes;
    private long sentBytes;

    public MessageServer() {
        this.socketChannel = null;
        this.receivedBytes = 0;
        this.sentBytes = 0;
    }

    public void connect(String host, int port) throws IOException {
        // close previous connection if necessary
        disconnect();

        this.address = new InetSocketAddress(host, port);

        // open and configure the socket
        this.socketChannel = SocketChannel.open();
        this.socketChannel.configureBlocking(true);
        //this.socketChannel.connect(address);
        this.socketChannel.socket().connect(address, 1000);

        this.isActive = true;

        // create and start the sender thread
        this.senderThread = new Thread(new Runnable() {
            @Override
            public void run() {
                sendReceiveLoop();
            }
        });

        this.senderThread.start();

        this.fireConnected(this.address);
    }//end connect

    public void disconnect() {
        disconnect(null);
    }

    private void disconnect(String message) {
        if (!isConnected()) {
            return;
        }

        this.isActive = false;

        // wait until the sender Thread is dead
        if (this.senderThread != null) {
            try {
                senderThread.join(1000); // wait max one second for the thread to stop
            } catch (InterruptedException e) { /* ignore */ }
        }

        // call error handlers of remaining requests
        for (SingleExecEntry a : answerRequestList) {
            a.listener.handleError(-2);
        }
        answerRequestList.clear();

        // close the soccet
        try {
            socketChannel.close();
        } catch (IOException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
                    "Unexpected exception...", ex);
        }
        socketChannel = null;

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
            throw new IllegalArgumentException("A valid command is required.");
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
            while (isActive && isConnected()) {
                long startTime = System.currentTimeMillis();
                // send all new commands
                sendPendingCommands();

                // read the answers for all the requests in answerRequestQueue
                pollAnswers();

                long stopTime = System.currentTimeMillis();
                long diff = updateIntervall - (stopTime - startTime);
                long wait = Math.max(5, diff);
                if (wait > 0) {
                    Thread.sleep(wait);
                }
            }
        } catch (AsynchronousCloseException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.INFO, "Connection was closed while trying to read.", ex);
            disconnect(ex.getMessage());
        } catch (InterruptedException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE, "thread was interupted", ex);
            disconnect(ex.getMessage());
        } catch (IOException ex) {
            Logger.getLogger(MessageServer.class.getName()).log(Level.SEVERE,
                    "Unexpected exception...", ex);
            disconnect(ex.getMessage());
        }
    }//end sendReceiveLoop

    private void sendPendingCommands() throws IOException {
        socketChannel.configureBlocking(true);

        // handle the single execution requests
        synchronized (this.pendingCommandsList) {
            for (SingleExecEntry e : this.pendingCommandsList) {
                sendCommand(socketChannel, e.command);
                answerRequestList.add(e);
            }
            this.pendingCommandsList.clear();
        }
        
        // handle the periodic execution requests
        synchronized (this.pendingSubscribersList) {
            for (CommandSender c : this.pendingSubscribersList) {
                SingleExecEntry e = new SingleExecEntry(c, c.getCurrentCommand());
                sendCommand(socketChannel, e.command);
                answerRequestList.add(e);
            }
        }
    }//end sendPendingCommands

    private void pollAnswers() throws IOException {
        socketChannel.configureBlocking(true);
        for (SingleExecEntry e : answerRequestList) {
            // read size of data
            int size = readContent(socketChannel, 4).getInt();

            ByteBuffer data = readContent(socketChannel, size);
            if (data == null) {
                throw new IOException("No data could be read from the socket.");
            }

            // call sender
            e.listener.handleResponse(data.array(), e.command);
        }//end while

        answerRequestList.clear();
    }//end pollAnswers

    // helpers
    private void sendCommand(SocketChannel socket, Command command) throws IOException {
        // convert to Protobuf
        CMD.Builder cmd = CMD.newBuilder().setName(command.getName());

        if (command.getArguments() != null) {
            for (Map.Entry<String, byte[]> e : command.getArguments().entrySet()) {
                CMDArg.Builder arg = CMDArg.newBuilder()
                        .setName(e.getKey());
                if (e.getValue() != null) {
                    arg = arg.setBytes(ByteString.copyFrom(e.getValue()));
                }
                cmd = cmd.addArgs(arg);
            }
        }

        byte[] commandAsByteArray = cmd.build().toByteArray();
        int intLength = commandAsByteArray.length;

        ByteBuffer bLength = byteBufferFromInt(intLength);

        ByteBuffer bCommand = ByteBuffer.wrap(commandAsByteArray);

        // write length    
        sentBytes += socket.write(bLength);
        // write data
        sentBytes += socket.write(bCommand);
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
