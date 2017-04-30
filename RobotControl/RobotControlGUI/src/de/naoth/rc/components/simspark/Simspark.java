package de.naoth.rc.components.simspark;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

/**
 * Base class for communicating with Simspark.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
abstract class Simspark extends Thread {

    protected DataInputStream in;
    protected DataOutputStream out;
    protected Socket socket;
    protected boolean isRunning;

    public Simspark() {
    }

    public void connect(String host, int port) throws IOException {
        socket = new Socket(host, port);

        in = new DataInputStream(socket.getInputStream());
        out = new DataOutputStream(socket.getOutputStream());

        isRunning = true;
        
        this.start();
    }

    public void disconnect() throws IOException, InterruptedException {
        isRunning = false;
        this.join();
        in.close();
        out.close();
        socket.close();
    }

    /**
     * Sends an agent message to the server.
     * <p/>
     * This method formats an agent message (String of SimSpark effector
     * messages) according to the network protocol and sends it to the server.
     * <p/>
     * The content of the agent message is not validated.
     *
     * @param msg Agent message with effector commands.
     * @param out
     */
    public void sendAgentMessage(String msg) {

        byte[] body = msg.getBytes();

        //comments by the authors of magma from Offenburg:
        // FIXME: this is to compensate a server bug that clients responding too
        // quickly get problems
        // long runtime = 0;
        // boolean slowedDown = false;
        // long slowDownTime = 0;
        // int minWaitTime = 1000000;
        // do {
        // runtime = System.nanoTime() - startTime;
        // if (runtime < minWaitTime && !slowedDown) {
        // slowDownTime = minWaitTime - runtime;
        // slowedDown = true;
        // }
        // } while (runtime < minWaitTime);
        // if (slowedDown) {
        // logger.log(Level.FINE, "slowedDown sending message by: {0}",
        // slowDownTime);
        // }
        // Header of the message, specifies the length of the message:
        // "The length prefix is a 32 bit unsigned integer in network order, i.e. big 
        // endian notation with the most significant bits transferred first." 
        // (cited from 
        // http://simspark.sourceforge.net/wiki/index.php/Network_Protocol, 14.1.2012)
        int len = body.length;
        int byte0 = (len >> 24) & 0xFF;
        int byte1 = (len >> 16) & 0xFF;
        int byte2 = (len >> 8) & 0xFF;
        int byte3 = len & 0xFF;

        try {
            out.writeByte((byte) byte0);
            out.writeByte((byte) byte1);
            out.writeByte((byte) byte2);
            out.writeByte((byte) byte3);
            out.write(body);
            out.flush();
        } catch (IOException e) {
            System.out.println("Error writing to socket. Has the server been shut down?");
        }
        if (msg != "(syn)") {
            System.out.println("sended msg: " + msg);
        }
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
    public String getServerMessage() {
        String msg = "keine Nachricht";
        byte[] result;
        int length;

        try {
            if (in.available() == 0) {
                return null;
            }
            int byte0 = in.read();
            int byte1 = in.read();
            int byte2 = in.read();
            int byte3 = in.read();
            length = byte0 << 24 | byte1 << 16 | byte2 << 8 | byte3; // analyzes
            // the header
            int total = 0;

            if (length < 0) {
                // server was shutdown
                System.out.println("Server ist down.");
            }

            result = new byte[length];
            while (total < length) {
                total += in.read(result, total, length - total);
            }
            msg = new String(result, 0, length, "UTF-8");
        } catch (IOException e) {
            System.out.println("Error when reading from socket. Has the server been shut down?");
            return null;
        }
        return msg;
    }
} // end class Simspark
