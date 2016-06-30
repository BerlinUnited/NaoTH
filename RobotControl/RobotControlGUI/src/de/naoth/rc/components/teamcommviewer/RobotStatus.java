package de.naoth.rc.components.teamcommviewer;

//import static de.naoth.rc.components.teamcommviewer.RobotStatusPanel.MAX_TIME_BEFORE_DEAD;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import java.awt.Color;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RobotStatus {

    public final static long MAX_TIME_BEFORE_DEAD = 5000; //ms
    private final RingBuffer timestamps = new RingBuffer(10);
    private final MessageServer messageServer;
    public String ipAddress = "";
    public boolean isConnected = false;

    public static final Color COLOR_INFO = new Color(0.0f, 1.0f, 0.0f, 0.5f);
    public static final Color COLOR_WARNING = new Color(1.0f, 1.0f, 0.0f, 0.5f);
    public static final Color COLOR_DANGER = new Color(1.0f, 0.0f, 0.0f, 0.5f);

    public byte playerNum;
    public byte teamNum;
    public Color robotColor = Color.WHITE;
    public double msgPerSecond;
    public float ballAge;
    public byte fallen;
    public boolean isDead;
    private ArrayList<RobotStatusListener> listener = new ArrayList<>();

    public float temperature;
    public float batteryCharge;
    public float timeToBall;
    public boolean wantsToBeStriker;
    public boolean wasStriker;
    public boolean isPenalized;
    
    /**
     * Creates new form RobotStatus
     */
    public RobotStatus(MessageServer messageServer, String ipAddress) {
        this.messageServer = messageServer;
        this.ipAddress = ipAddress;

        this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() {

            @Override
            public void connected(ConnectionStatusEvent event) {
                isConnected = true;
                statusChanged();
            }

            @Override
            public void disconnected(ConnectionStatusEvent event) {
                isConnected = false;
                statusChanged();
            }
        });
    }
    
    public void addListener(RobotStatusListener l) {
        listener.add(l);
    }
    public void removeListener(RobotStatusListener l) {
        listener.remove(l);
    }

    public void updateStatus(long timestamp, SPLMessage msg) {
        this.teamNum = msg.teamNum;
        this.playerNum = msg.playerNum;

        // don't add the timestamp if it did not change compared to the last time
        long lastSeen = Long.MIN_VALUE;
        if (!timestamps.isEmpty()) {
            lastSeen = timestamps.get(timestamps.size() - 1);
        }
        if (lastSeen < timestamp) {
            timestamps.add(timestamp);
            lastSeen = timestamp;
        }
        this.isDead = ((System.currentTimeMillis() - lastSeen) > MAX_TIME_BEFORE_DEAD || this.msgPerSecond <= 0.0);
        this.msgPerSecond = calculateMsgPerSecond();
        this.fallen = msg.fallen;
        this.ballAge = msg.ballAge;

        if (msg.user != null) {
            this.temperature = msg.user.getTemperature();
            this.batteryCharge = msg.user.getBatteryCharge() * 100.0f;
            this.timeToBall = msg.user.getTimeToBall();
//            this.wantsToBeStriker = msg.user.getWantsToBeStriker();
            this.wasStriker = msg.user.getWasStriker();
            this.isPenalized = msg.user.getIsPenalized();
        } else {
            this.temperature = -1;
            this.batteryCharge = -1;
            this.timeToBall = -1;
            this.wantsToBeStriker = false;
            this.wasStriker = false;
            this.isPenalized = false;
        }
        this.statusChanged();
    }

    private double calculateMsgPerSecond() {
        long sumOfDiffs = 0;
        int numberOfEntries = 0;
        Iterator<Long> it = timestamps.iterator();
        if (it.hasNext()) {
            long t1 = it.next();
            while (it.hasNext()) {
                long t2 = it.next();
                long diff = t2 - t1;
                if (diff > 0) {
                    sumOfDiffs += (t2 - t1);
                    numberOfEntries++;
                }
                t1 = t2;
            }
        }
        if (numberOfEntries > 0) {
            return 1000.0 / ((double) sumOfDiffs / (double) numberOfEntries);
        } else {
            return 0.0;
        }
    }
    
    private void statusChanged() {
        for (RobotStatusListener l : listener) {
            l.statusChanged();
        }
    }
    
    public boolean connect() {
        if (!this.messageServer.isConnected()) {
            try {
                // TODO: fix port 5401
                this.messageServer.connect(this.ipAddress, 5401);
            } catch (IOException ex) {
                Logger.getLogger(RobotStatusPanel.class.getName()).log(Level.SEVERE, "Coult not connect.", ex);
                return false;
            }
        }
        return true;
    }
    
    private class RingBuffer extends ArrayList<Long> {

        private final int size;

        public RingBuffer(int size) {
            this.size = size;
        }

        @Override
        public boolean add(Long v) {
            boolean r = super.add(v);

            if (size() > size) {
                remove(0);
            }

            return r;
        }
    }
}
