package de.naoth.rc.components.teamcommviewer;

import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.math.Vector2D;
import de.naoth.rc.core.messages.TeamMessageOuterClass;
import de.naoth.rc.core.server.ConnectionStatusEvent;
import de.naoth.rc.core.server.ConnectionStatusListener;
import de.naoth.rc.core.server.MessageServer;
import java.awt.Color;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * Representation of all known information about a robot.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RobotStatus 
{
    public final static long MAX_TIME_BEFORE_DEAD = 5000; //ms
    
    public static final Color COLOR_INFO = new Color(0.0f, 1.0f, 0.0f, 0.5f);
    public static final Color COLOR_WARNING = new Color(1.0f, 1.0f, 0.0f, 0.5f);
    public static final Color COLOR_DANGER = new Color(1.0f, 0.0f, 0.0f, 0.5f);

    private final RingBuffer timestamps = new RingBuffer(10);
    private final MessageServer messageServer;
    private final ArrayList<RobotStatusListener> listener = new ArrayList<>();
    
    private final StringProperty ipAddress = new SimpleStringProperty("");
    public StringProperty ipAddressProperty() { return ipAddress; }
    public String getIpAddress() { return ipAddress.get(); }
    
    private final BooleanProperty isConnected = new SimpleBooleanProperty(false);
    public BooleanProperty isConnectedProperty() { return isConnected; }
    public boolean getIsConnected(){ return isConnected.get(); }

    private final IntegerProperty playerNum = new SimpleIntegerProperty(0);
    public IntegerProperty playerNumProperty() { return playerNum; };
    public int getPlayerNum() { return playerNum.get(); }
    
    private final IntegerProperty teamNum = new SimpleIntegerProperty(0);
    public IntegerProperty teamNumProperty() { return teamNum; };
    public int getTeamNum() { return teamNum.get(); }
    
    public final ObjectProperty<javafx.scene.paint.Color> robotColor = new SimpleObjectProperty<>(javafx.scene.paint.Color.WHITE); //color, "color", javafx.scene.paint.Color.WHITE
    public ObjectProperty getRobotColor() { return robotColor; }
    public Color getRobotColorAwt() { return new Color((float)robotColor.get().getRed(), (float)robotColor.get().getGreen(), (float)robotColor.get().getBlue(), (float)robotColor.get().getOpacity()); }
    public void setRobotColorAwt(Color color) { float[] c = color.getRGBComponents(null); robotColor.set(new javafx.scene.paint.Color(c[0], c[1], c[2], c[3])); }
    
    private final DoubleProperty msgPerSecond = new SimpleDoubleProperty(0.0);
    public DoubleProperty msgPerSecondProperty() { return msgPerSecond; };
    public double getMsgPerSecond() { return msgPerSecond.get(); }
    
    private final DoubleProperty ballAge = new SimpleDoubleProperty(0.0);
    public DoubleProperty ballAgeProperty() { return ballAge; };
    public double getBallAge() { return ballAge.get(); }
    
    private final BooleanProperty fallen = new SimpleBooleanProperty(false);
    public BooleanProperty fallenProperty() { return fallen; }
    public boolean getFallen() { return fallen.get(); }
    
    private final BooleanProperty isDead = new SimpleBooleanProperty(false);
    public BooleanProperty isDeadProperty() { return isDead; }
    public boolean getIsDead() { return isDead.get(); }
    
    // TODO: isDead is a computed value and needs to be reset after a certain 
    //       time if no update was received.
    //       This method is passive and does not require an internal thread 
    //       to be up to date
    //public boolean isDead() {
    //    return ((System.currentTimeMillis() > lastSeen + MAX_TIME_BEFORE_DEAD) || this.msgPerSecond.get() <= 0.0);
    //}
    
    private final DoubleProperty temperature = new SimpleDoubleProperty(0.0);
    public DoubleProperty temperatureProperty() { return temperature; };
    public double getTemperature() { return temperature.get(); }
    
    private final DoubleProperty cpuTemperature = new SimpleDoubleProperty(0.0);
    public DoubleProperty cpuTemperatureProperty() { return cpuTemperature; };
    public double getCpuTemperature() { return cpuTemperature.get(); }
    
    private final DoubleProperty batteryCharge = new SimpleDoubleProperty(0.0);
    public DoubleProperty batteryChargeProperty() { return batteryCharge; };
    public double getBatteryCharge() { return batteryCharge.get(); }
    
    private final DoubleProperty timeToBall = new SimpleDoubleProperty(0.0);
    public DoubleProperty timeToBallProperty() { return timeToBall; };
    public double getTimeToBall() { return timeToBall.get(); }
    
    private final BooleanProperty wantsToBeStriker = new SimpleBooleanProperty(false);
    public BooleanProperty wantsToBeStrikerProperty() { return wantsToBeStriker; }
    public boolean getWantsToBeStriker() { return wantsToBeStriker.get(); }
    
    private final BooleanProperty wasStriker = new SimpleBooleanProperty(false);
    public BooleanProperty wasStrikerProperty() { return wasStriker; }
    public boolean getWasStriker() { return wasStriker.get(); }

    private final StringProperty robotState = new SimpleStringProperty("");
    public StringProperty robotStateProperty() { return robotState; }
    public String getRobotState() { return robotState.get(); }
    
    private final BooleanProperty whistleDetected = new SimpleBooleanProperty(false);
    public BooleanProperty whistleDetectedProperty() { return whistleDetected; }
    public boolean getWhistleDetected() { return whistleDetected.get(); }
    
    private final ObjectProperty<Vector2D> teamBall = new SimpleObjectProperty<>();
    public ObjectProperty teamBallProperty() { return teamBall; }
    public Vector2D getTeamBall() { return teamBall.get(); }
    
    private final BooleanProperty showOnField = new SimpleBooleanProperty(true);
    public BooleanProperty showOnFieldProperty() { return showOnField; }
    public boolean getShowOnField() { return showOnField.get(); }
    public void setShowOnField(boolean b) { showOnField.set(b); }
    
    private final StringProperty robotRoleStatic = new SimpleStringProperty("");
    public StringProperty robotRoleStaticProperty() { return robotRoleStatic; }
    public String getRobotRoleStatic() { return robotRoleStatic.get(); }
    
    private final StringProperty robotRoleDynamic = new SimpleStringProperty("");
    public StringProperty robotRoleDynamicProperty() { return robotRoleDynamic; }
    public String getRobotRoleDynamic() { return robotRoleDynamic.get(); }
    
    public SPLMessage lastMessage = null;
    public boolean isOpponent;

    private final ScheduledExecutorService executorService;
    private ScheduledFuture timer = null;
    
    /**
     * Creates new form RobotStatus
     * @param messageServer
     * @param ipAddress
     * @param isOpponent
     */
    public RobotStatus(MessageServer messageServer, String ipAddress, boolean isOpponent) {
        this(messageServer, ipAddress, isOpponent, false);
    }

    public RobotStatus(MessageServer messageServer, String ipAddress, boolean isOpponent, boolean useIsDeadTimer) {
        this.messageServer = messageServer;
        this.ipAddress.set(ipAddress);
        this.isOpponent = isOpponent;
        if(useIsDeadTimer) {
            executorService = Executors.newSingleThreadScheduledExecutor();
        } else {
            executorService = null;
        }
        // when robot is 'dead', set msgPerSecond to zero
        isDead.addListener((s, o, n) -> {
            if(n) {
                msgPerSecond.set(0.0);
            }
        });

        this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() 
        {
            @Override
            public void connected(ConnectionStatusEvent event) {
                isConnected.set(true);
                statusChanged();
            }

            @Override
            public void disconnected(ConnectionStatusEvent event) {
                isConnected.set(false);
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

    /**
     * Updates all information with the given message.
     * @param timestamp
     * @param msg 
     */
    public void updateStatus(long timestamp, SPLMessage msg) 
    {
        this.lastMessage = msg;
        
        this.teamNum.set(msg.teamNum);
        this.playerNum.set(msg.playerNum);

        // don't add the timestamp if it did not change compared to the last time
        long lastSeen = Long.MIN_VALUE;
        if (!timestamps.isEmpty()) {
            lastSeen = timestamps.get(timestamps.size() - 1);
        }
        // TODO: in which cases does this happen? 
        //   If timestamp < lastSeen, then it is an old message 
        //   and we should discard it completely(?)
        //   Why do we process old messages?
        if (lastSeen < timestamp) {
            timestamps.add(timestamp);
            lastSeen = timestamp;
        }
        
        if(executorService == null) {
            this.isDead.set(((System.currentTimeMillis() > lastSeen + MAX_TIME_BEFORE_DEAD) || this.msgPerSecond.get() <= 0.0));
        } else {
            // cancel the old timer (let it finish if it's already running)
            if(timer != null) {
                timer.cancel(false);
            }

            this.isDead.set(false);
            
            // set the robot status to "dead" after a certain time without update
            timer = executorService.schedule(() -> isDead.set(true), MAX_TIME_BEFORE_DEAD, TimeUnit.MILLISECONDS);
        }
        
        this.msgPerSecond.set(calculateMsgPerSecond());
        this.fallen.set(msg.fallen == 1);
        this.ballAge.set(msg.ballAge);

        if (msg.user != null) {
            this.temperature.set(msg.user.getTemperature());
            this.cpuTemperature.set(msg.user.getCpuTemperature());
            this.batteryCharge.set(msg.user.getBatteryCharge() * 100.0f);
            this.timeToBall.set(msg.user.getTimeToBall());
            this.wantsToBeStriker.set(msg.user.getWantsToBeStriker());
            this.wasStriker.set(msg.user.getWasStriker());
            if(msg.user.hasIsPenalized()) {
                this.robotState.set(msg.user.getIsPenalized()?"penalized":"playing");
            } else {
                this.robotState.set(msg.user.getRobotState().name());
            }
//            this.whistleDetected = msg.user.getWhistleDetected(); // used in another branch!
            this.teamBall.set(new Vector2D(msg.user.getTeamBall().getX(), msg.user.getTeamBall().getY()));
            this.robotRoleStatic.set(msg.user.getRobotRole().getRoleStatic().name());
            this.robotRoleDynamic.set(msg.user.getRobotRole().getRoleDynamic().name());
        } else if(msg.mixedHeader != null) {
            this.temperature.set(-1);
            this.cpuTemperature.set(-1);
            this.batteryCharge.set(-1);
            this.timeToBall.set(-1);
            this.wantsToBeStriker.set(msg.mixedHeader.isStriker);
            this.wasStriker.set(msg.mixedHeader.isStriker);

            this.robotState.set(msg.mixedHeader.isPenalized > 0?"penalized":"playing");
            this.whistleDetected.set(msg.mixedHeader.whistleDetected > 0);
            this.teamBall.set(new Vector2D(Double.POSITIVE_INFINITY,Double.POSITIVE_INFINITY));
            
            this.robotRoleStatic.set(TeamMessageOuterClass.getDescriptor().findEnumTypeByName("RobotRoleStatic").findValueByNumber(msg.mixedHeader.role).getName());
            this.robotRoleDynamic.set(msg.mixedHeader.isStriker ? TeamMessageOuterClass.getDescriptor().findEnumTypeByName("RobotRoleDynamic").findValueByNumber(3).getName() : TeamMessageOuterClass.getDescriptor().findEnumTypeByName("RobotRoleDynamic").findValueByNumber(0).getName());
        } else {
            this.temperature.set(-1);
            this.cpuTemperature.set(-1);
            this.batteryCharge.set(-1);
            this.timeToBall.set(-1);
            this.wantsToBeStriker.set(false);
            this.wasStriker.set(false);
            this.robotState.set("unknown");
            this.whistleDetected.set(false);
            this.teamBall.set(new Vector2D(Double.POSITIVE_INFINITY,Double.POSITIVE_INFINITY));
            this.robotRoleStatic.set("unknown");
            this.robotRoleDynamic.set("none");
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
    
    /**
     * Informs all listener of the changed status.
     */
    private void statusChanged() {
        listener.forEach((l) -> {
            l.statusChanged(this);
        });
    }
    
    public boolean connect() {
        if (!this.messageServer.isConnected()) {
            String host = this.ipAddress.get();
            int port = 5401;
            // if the ip address contains a ':', the port is included!
            if(host.contains(":")){
                String[] parts = host.split(":");
                host = parts[0];
                // if we can't parse the port, ignore it
                try {
                    port = Integer.parseInt(parts[1]);
                } catch (Exception e) {
                    // don't report
                }
            }
            return this.messageServer.connect(host, port);
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
