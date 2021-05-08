package de.naoth.rc.components.simspark;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;

/**
 * Base class for communicating with Simspark.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
abstract class Simspark extends Thread {

    protected DataInputStream in;
    protected DataOutputStream out;
    protected Socket socket;
    public BooleanProperty isConnected = new SimpleBooleanProperty(false);

    public Simspark() {}

    /**
     * Connect to a simspark instance.
     * @param host the host/ip of the simspark instance
     * @param port the port of the simspark instance
     * @throws IOException if coudn't connect to the simspark instance
     */
    public void connect(String host, int port) throws IOException {
        socket = new Socket();
        socket.setSoTimeout(1000);
        // connecting this way to provid timeout ...
        socket.connect(new InetSocketAddress(host, port), 1000);

        in = new DataInputStream(socket.getInputStream());
        out = new DataOutputStream(socket.getOutputStream());
        
        isConnected.set(true);
        // start listening thread
        this.start();
    }

    /**
     * Disconnects from the simspark instance.
     * @throws IOException
     * @throws InterruptedException 
     */
    public void disconnect() throws IOException, InterruptedException {
        isConnected.set(false);
        if(isAlive()) { join(1000); }
        if(in != null) { in.close(); }
        if(out != null) { out.close(); }
        if(socket != null) { socket.close(); }
    }

    /**
     * Sends a message to the server.
     * <p/>
     * This method formats an agent/monitor message (String of SimSpark effector
     * messages) according to the network protocol and sends it to the server.
     * <p/>
     * The content of the agent message is not validated.
     *
     * @param msg Agent message with effector commands.
     * @param out
     */
	public boolean sendMessage(String msg) throws IOException
	{
        if (out == null || !isConnected.get()) {
            return false;
        }
        out.write(intToBytes(msg.length()));
        out.write(msg.getBytes());
        out.flush();
        return true;
	}

    /**
     * Converts an integer to a byte array.
     * 
     * @param i the integer, which should be converted
     * @return the converted integer as byte array
     */
    private byte[] intToBytes(int i)
	{
		byte[] buf = new byte[4];
		buf[0] = (byte) ((i >> 24) & 255);
		buf[1] = (byte) ((i >> 16) & 255);
		buf[2] = (byte) ((i >> 8) & 255);
		buf[3] = (byte) (i & 255);
		return buf;
	}
    
    /**
     * Receives a server message and returns it.
     * <p/>
     * This method listens (blocking) for the next SimSpark message from the
     * server, removes the header concerning the SimSpark network protocol and
     * returns the server message (String of perceptor messages). <br>
     * If the server has sent more then one message since the last call of this
     * method, the oldest is returned, that means the messages are provided
     * always in chronological order.
     * <p/>
     * @param in
     * @return The raw server message (String of concatenated perceptor
     * messages).
     */
    protected String receiveMessage() throws IOException {
        // message is prefixed by its size in bytes
        int length = 0;
        if (in == null || (length = in.readInt()) <= 0) {
            // not connected or disconnected?!
            return null;
        }
        // read from stream until all bytes in message are read
        byte[] buf = new byte[length];
        int bytesRead = 0;
        while (bytesRead < length) {
            bytesRead += in.read(buf, bytesRead, length - bytesRead);
        }
        return new String(buf);
    }
    
    /**
     * Returns whether the connection is still active (true) or if the connection was lost (false).
     * 
     * @return true|false
     */
    public boolean checkConnection() {
        boolean disconnect = true;
        try {
            disconnect = !sendMessage("(getAck)");
        } catch (IOException ex) {}
        
        if(disconnect) {
            try {
                disconnect();
            } catch (IOException | InterruptedException ex) {
                Logger.getLogger(Simspark.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        return !disconnect;
    }
    
    /**
     * Returns the host ip/name this instance is connected to simspark.
     * 
     * @return the host ip/name or null if not connected
     */
    public String getHost() {
        if(socket != null && checkConnection()) {
            return ((InetSocketAddress) socket.getRemoteSocketAddress()).getHostName();
        }
        return null;
    }
    
    /**
     * Returns the port this instance is connected to simspark.
     * 
     * @return the connected port, or zero if not connected
     */
    public int getPort() {
        if(socket != null && checkConnection()) {
            return ((InetSocketAddress) socket.getRemoteSocketAddress()).getPort();
        }
        return 0;
    }
} // end class Simspark
