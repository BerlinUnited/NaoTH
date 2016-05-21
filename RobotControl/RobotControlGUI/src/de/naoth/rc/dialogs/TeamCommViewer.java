/**
 * TeamCommViewer.java
 *
 * Created on 08.11.2010, 21:41:28
 */
package de.naoth.rc.dialogs;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.stream.JsonWriter;
import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.RobotStatus;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.drawings.DrawingCollection;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.LinearGradientPaint;
import java.awt.RenderingHints;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.AsynchronousCloseException;
import java.nio.channels.DatagramChannel;
import java.text.SimpleDateFormat;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeMap;
import java.util.Vector;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JTable;
import javax.swing.SwingUtilities;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableCellRenderer;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public class TeamCommViewer extends AbstractDialog {

    @PluginImplementation
    public static class Plugin extends DialogPlugin<TeamCommViewer> {

        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static DrawingEventManager drawingEventManager;
    }//end Plugin

    private Timer timerCheckMessages;
    private final TeamCommListener listenerOwn = new TeamCommListener(false);
    private final TeamCommListener listenerOpponent = new TeamCommListener(true);
    private final HashMap<String, RobotStatus> robotsMap = new HashMap<>();

    private final Map<String, TeamCommMessage> messageMap = Collections.synchronizedMap(new TreeMap<String, TeamCommMessage>());

    private final Color magenta = new Color(210, 180, 200);
    private final Color cyan = new Color(180, 210, 255);
    
    private LogFileWriter logfile = null;
    private final ConcurrentLinkedQueue<TeamCommMessage> logfileQueue = new ConcurrentLinkedQueue<TeamCommMessage>();
    private boolean logfileQueueAppend = false;
    
    /**
     * Creates new form TeamCommViewer
     */
    public TeamCommViewer() {
        initComponents();
        
        // msg/s
        robotStatusTable.getColumnModel().getColumn(3).setCellRenderer(new DefaultTableCellRenderer(){
            @Override
            public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
                return super.getTableCellRendererComponent(table, String.format("%4.2f", value), isSelected, hasFocus, row, column);
            }
        });
        // Temperature
        robotStatusTable.getColumnModel().getColumn(6).setCellRenderer(new DefaultTableCellRenderer(){
            @Override
            public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
                float temp = (float)value;
                if(temp>20) {
                    float ratio = (temp-20) / 80.0f; // normalize temp
                    Color c1 = Color.blue; Color c2 = Color.red;
                    int red = (int)(c2.getRed()*ratio+c1.getRed()*(1-ratio));
                    int green = (int)(c2.getGreen()*ratio+c1.getGreen()*(1-ratio));
                    int blue = (int)(c2.getBlue()*ratio+c1.getBlue()*(1-ratio));
                    this.setForeground(new Color(red, green, blue));
                } else {
                    this.setForeground(Color.black);
                }
                
                return super.getTableCellRendererComponent(table, temp==-1?"?":String.format(" %3.1f °C", value), isSelected, hasFocus, row, column);
            }
        });
        // Battery
        robotStatusTable.getColumnModel().getColumn(7).setCellRenderer(new BatteryRenderer());
        // sort via Ip
        robotStatusTable.getRowSorter().toggleSortOrder(2);
        // collapse pane
        robotStatusSplitPane.setDividerLocation(Integer.MAX_VALUE);/*2000*/
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        teamCommFileChooser = new de.naoth.rc.components.ExtendedFileChooser();
        btListen = new javax.swing.JToggleButton();
        portNumberOwn = new javax.swing.JFormattedTextField();
        portNumberOpponent = new javax.swing.JFormattedTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        btnRecord = new javax.swing.JToggleButton();
        btnStopRecording = new javax.swing.JButton();
        jPanel2 = new javax.swing.JPanel();
        robotStatusSplitPane = new javax.swing.JSplitPane();
        jScrollPane2 = new javax.swing.JScrollPane();
        robotStatusPanel = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        robotStatusTable = new javax.swing.JTable();

        teamCommFileChooser.setDialogType(javax.swing.JFileChooser.SAVE_DIALOG);
        teamCommFileChooser.setDialogTitle("Log file location");
        teamCommFileChooser.setSelectedFile(new File((new SimpleDateFormat("yyyy-MM-dd")).format(new Date())+"_TeamComm.log"));
        teamCommFileChooser.setToolTipText("");

        btListen.setText("Listen to TeamComm");
        btListen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btListenActionPerformed(evt);
            }
        });

        portNumberOwn.setColumns(6);
        portNumberOwn.setFormatterFactory(new javax.swing.text.DefaultFormatterFactory(new javax.swing.text.NumberFormatter(new java.text.DecimalFormat("#"))));
        portNumberOwn.setText("10004");
        portNumberOwn.setToolTipText("Own team port number");

        portNumberOpponent.setColumns(6);
        portNumberOpponent.setFormatterFactory(new javax.swing.text.DefaultFormatterFactory(new javax.swing.text.NumberFormatter(new java.text.DecimalFormat("#"))));
        portNumberOpponent.setToolTipText("Opponent team port number");

        jLabel1.setText("Blue:");

        jLabel2.setText("Red:");

        btnRecord.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/play.png"))); // NOI18N
        btnRecord.setText("Record TeamComm");
        btnRecord.setActionCommand("RecordTeamComm");
        btnRecord.setSelectedIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png"))); // NOI18N
        btnRecord.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnRecordActionPerformed(evt);
            }
        });

        btnStopRecording.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Stop24.gif"))); // NOI18N
        btnStopRecording.setToolTipText("Stops recording and closes log file.");
        btnStopRecording.setEnabled(false);
        btnStopRecording.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnStopRecordingActionPerformed(evt);
            }
        });

        robotStatusSplitPane.setBorder(null);
        robotStatusSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        robotStatusSplitPane.setOneTouchExpandable(true);

        robotStatusPanel.setLayout(new java.awt.GridLayout(5, 1, 0, 5));
        jScrollPane2.setViewportView(robotStatusPanel);

        robotStatusSplitPane.setLeftComponent(jScrollPane2);

        jScrollPane1.setMinimumSize(new java.awt.Dimension(0, 0));
        jScrollPane1.setPreferredSize(new java.awt.Dimension(0, 0));

        robotStatusTable.setAutoCreateRowSorter(true);
        robotStatusTable.setModel(new RobotTableModel());
        robotStatusTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_LAST_COLUMN);
        robotStatusTable.setRowSelectionAllowed(false);
        jScrollPane1.setViewportView(robotStatusTable);

        robotStatusSplitPane.setRightComponent(jScrollPane1);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
            .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(robotStatusSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 838, Short.MAX_VALUE))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 452, Short.MAX_VALUE)
            .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(robotStatusSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 452, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(btListen)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(portNumberOwn, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(portNumberOpponent, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(12, 12, 12)
                        .addComponent(btnRecord)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnStopRecording)
                        .addGap(0, 124, Short.MAX_VALUE)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(20, 20, 20)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(btnStopRecording, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
                    .addComponent(btnRecord, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(btListen)
                        .addComponent(portNumberOwn, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(portNumberOpponent, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(jLabel1)
                        .addComponent(jLabel2)))
                .addGap(18, 18, 18)
                .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btListenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btListenActionPerformed

        try {
            if (this.btListen.isSelected()) {
                String ownPortRaw = portNumberOwn.getText().trim();
                if(!ownPortRaw.isEmpty()) {
                    int portOwn = Integer.parseInt(ownPortRaw);
                    listenerOwn.connect(portOwn);
                }
                String opponentPortRaw = portNumberOpponent.getText().trim();
                if (!opponentPortRaw.isEmpty()) {
                    int portOpponent = Integer.parseInt(opponentPortRaw);
                    listenerOpponent.connect(portOpponent);
                }

                this.timerCheckMessages = new Timer();
                this.timerCheckMessages.scheduleAtFixedRate(new TeamCommListenTask(), 100, 33);
                this.portNumberOwn.setEnabled(false);
                this.portNumberOpponent.setEnabled(false);
                this.robotStatusPanel.setVisible(true);
                this.robotStatusTable.setVisible(true);

            } else {

                if(this.timerCheckMessages != null) {
                    this.timerCheckMessages.cancel();
                    this.timerCheckMessages.purge();
                    this.timerCheckMessages = null;
                }
                
                listenerOwn.disconnect();
                listenerOpponent.disconnect();

                synchronized (messageMap) {
                    messageMap.clear();
                    this.robotsMap.clear();
                    this.robotStatusPanel.removeAll();
                    this.robotStatusPanel.setVisible(false);
                    this.robotStatusTable.setVisible(false);
                    ((RobotTableModel)this.robotStatusTable.getModel()).removeAll();
                    this.portNumberOwn.setEnabled(true);
                    this.portNumberOpponent.setEnabled(true);
                }
            }
        } catch (NumberFormatException ex) {
            Helper.handleException("Invalid port number", ex);
        } catch (InterruptedException ex) {
            ex.printStackTrace(System.err);
        } catch (IOException ex) {
            ex.printStackTrace(System.err);
        }
    }//GEN-LAST:event_btListenActionPerformed

    private void btnRecordActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnRecordActionPerformed
        // button got pressed
        if(btnRecord.isSelected()) {
            // log file already set; just enable logging
            if(this.logfile != null) {
                logfileQueueAppend = true;
                btnRecord.setSelected(true);
                setBtnRecordToolTipText(true);
            } else {
                // select log file and enable logging!
                this.setLogFile();
            }
        // release button
        } else {
            if(logfile != null) { // be sure log file is set
                logfileQueueAppend = false;
                setBtnRecordToolTipText(false);
            }
        }
    }//GEN-LAST:event_btnRecordActionPerformed

    private void btnStopRecordingActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnStopRecordingActionPerformed
        // stop log file recording, flush and close logfile, update UI
        logfileQueueAppend = false;
        closingLogfile();
        btnRecord.setSelected(false);
        setBtnRecordToolTipText(false);
        btnStopRecording.setEnabled(false);
    }//GEN-LAST:event_btnStopRecordingActionPerformed

    @Override
    public void dispose() {
        closingLogfile();
    }

    @Override
    public void destroy() {
        System.err.println("destroy");
        super.destroy(); //To change body of generated methods, choose Tools | Templates.
    }
    
    private boolean setLogFile() {
        // open filechooser
        if(teamCommFileChooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            // if selected file has a file extension - use this, otherwise append ".log" to the filename
            File dfile = (teamCommFileChooser.getSelectedFile().getName().lastIndexOf(".") == -1) ? 
                new File(teamCommFileChooser.getSelectedFile()+".log") : 
                teamCommFileChooser.getSelectedFile();

            if(dfile.canWrite()) {
                try {
                    // make sure there is no open log file ..
                    closingLogfile();

                    // create new log file
                    dfile.createNewFile();
                    logfile = new LogFileWriter(dfile);
                    logfile.start();

                    btnRecord.setSelected(true);
                    setBtnRecordToolTipText(true);
                    btnStopRecording.setEnabled(true); // enable "stop"-button

                    return true;
                } catch (IOException ex) {
                    Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
                    JOptionPane.showMessageDialog(null, "IO-Error occurred, see application log.", "Exception", JOptionPane.ERROR_MESSAGE);
                } catch (Throwable ex) {
                    Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            } else {
                JOptionPane.showMessageDialog(null, "Selected log file is not writeable!", "Not writeable", JOptionPane.ERROR_MESSAGE);
            }
        }
        
        btnRecord.setSelected(false); // prevent toggling!
        setBtnRecordToolTipText(false);
        
        return false;
    }
    
    private void setBtnRecordToolTipText(boolean stop){
        String name = this.logfile == null ? "<not-set>" : this.logfile.getLogfileName();
        if(!stop) {
            btnRecord.setToolTipText("Start recording to: " + name);
        } else {
            btnRecord.setToolTipText("Pause recording to: " + name);
        }
        
    }
    
    private void closingLogfile() {
        if(logfile != null) {
            try {
                logfile.close();
                logfile.join();
                logfile = null;
            } catch (InterruptedException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    private class TeamCommListenTask extends TimerTask {

        @Override
        public void run() {
            synchronized (messageMap) {
                if (messageMap.isEmpty()) {
                    return;
                }

                DrawingCollection drawings = new DrawingCollection();

                for (Entry<String, TeamCommMessage> msgEntry : messageMap.entrySet()) {
                    final String address = msgEntry.getKey();
                    final TeamCommMessage msg = msgEntry.getValue();

                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run() {
                            RobotStatus robotStatus = robotsMap.get(address);
                            if (robotStatus == null) {
                                robotStatus = new RobotStatus(Plugin.parent.getMessageServer(), address, msg.isOpponent() ? magenta : cyan);
                                robotsMap.put(address, robotStatus);
                                robotStatusPanel.add(robotStatus);
                                ((RobotTableModel)robotStatusTable.getModel()).addRobot(robotStatus);
                            }
                            robotStatus.setStatus(msg.timestamp, msg.message);
                            ((RobotTableModel)robotStatusTable.getModel()).fireTableDataChanged();
                        }
                    });
                    
                    msg.message.draw(drawings, msg.isOpponent() ? Color.RED : Color.BLUE, msg.isOpponent());
                }

                TeamCommViewer.Plugin.drawingEventManager.fireDrawingEvent(drawings);

            } // end synchronized
        } // end run
    }

    
    public class TeamCommMessage {

        public TeamCommMessage(long timestamp, SPLMessage message, boolean isOpponent) {
            this.timestamp = timestamp;
            this.message = message;
            this.isOpponent = isOpponent;
        }

        public final long timestamp;
        public final SPLMessage message;
        private final boolean isOpponent;

        public boolean isOpponent() {
            return isOpponent;
        }
    }
    
    public class TeamCommListener implements Runnable {
        private DatagramChannel channel;
        private Thread trigger;

        private final ByteBuffer readBuffer;

        private final boolean isOpponent;

        public TeamCommListener(boolean isOpponent) {
            this.readBuffer = ByteBuffer.allocateDirect(SPLMessage.SPL_STANDARD_MESSAGE_SIZE);
            this.readBuffer.order(ByteOrder.LITTLE_ENDIAN);
            this.isOpponent = isOpponent;
        }

        boolean isConnected() {
            return this.channel != null && this.trigger != null;
        }

        public void connect(int port) throws IOException, InterruptedException {
            disconnect();

            this.channel = DatagramChannel.open();
            this.channel.configureBlocking(true);
            this.channel.bind(new InetSocketAddress(InetAddress.getByName("0.0.0.0"), port));

            this.trigger = new Thread(this);
            this.trigger.start();
        }

        public void disconnect() throws IOException, InterruptedException {
            if (this.channel != null) {
                this.channel.close();
                this.channel = null;
            }
            if (this.trigger != null) {
                this.trigger.join();
                this.trigger = null;
            }
        }

        @Override
        public void run() {
            try {
                while (true) {
                    this.readBuffer.clear();
                    SocketAddress address = this.channel.receive(this.readBuffer);
                    this.readBuffer.flip();

                    try {
                        long timestamp = System.currentTimeMillis();
                        SPLMessage spl_msg = new SPLMessage(this.readBuffer);
                        TeamCommMessage tc_msg = new TeamCommMessage(timestamp, spl_msg, this.isOpponent);
                        
                        if(logfileQueueAppend) {
                            logfileQueue.add(tc_msg);
                        }

                        if (address instanceof InetSocketAddress) {
                            messageMap.put(((InetSocketAddress) address).getHostString(), tc_msg);
                        }

                    } catch (Exception ex) {
                        Logger.getLogger(TeamCommViewer.class.getName()).log(Level.INFO, null, ex);
                    }

                }
            } catch (AsynchronousCloseException ex) {
                /* socket was closed, that's fine */
            } catch (SocketException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }//end class TeamCommListener
    
    private class LogFileWriter extends Thread {
        private final File log;
        private final Gson json;
        private JsonWriter jw;
        public boolean running = true;
        
        public LogFileWriter(File log) {
            this.log = log;
            this.json = new GsonBuilder()
                // shouldn't be any ... just for safety reasons
                .disableInnerClassSerialization()
                // skip Variable "user" in SPLMessage
                /*
                .addSerializationExclusionStrategy(new ExclusionStrategy() {
                    @Override
                    public boolean shouldSkipField(FieldAttributes fieldAttributes) {
                        return fieldAttributes.getName().compareTo("user") == 0;
                    }

                    @Override
                    public boolean shouldSkipClass(Class<?> aClass) {
                        return false;
                    }
                })*/
                // write NaN value without throwing error/warning
                .serializeSpecialFloatingPointValues()
                .create();
        }

        public void run() {
            try {
                // use JsonWriter to write the enclosing Json-Array ...
                this.jw = new JsonWriter(new FileWriter(this.log));
                // open "global"/"enclosing" Array
                this.jw.beginArray();
                
                while (running) {
                    TeamCommMessage msg = logfileQueue.poll();
                    if(msg != null) {
                        writeMessage(msg.message, msg.timestamp, msg.isOpponent);
                    }
                }
                
                // flush buffer and close
                this.jw.endArray();
                this.jw.close();
            } catch (IOException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        
        public void close() {
            running = false;
        }
        
        public String getLogfileName() {
            return this.log != null ? this.log.getName() : "";
        }
        
        public void writeMessage(SPLMessage msg, long timestamp, boolean isOpponent) throws IOException {
            this.jw.beginObject();
            this.jw.name("timestamp");
            this.jw.value(timestamp);
            this.jw.name("isOpponent");
            this.jw.value(isOpponent);
            this.jw.name("spl");
            this.jw.jsonValue(this.json.toJson(msg));
            this.jw.endObject();
        }
    }
    
    private class RobotTableModel extends AbstractTableModel {
        private final Vector<RobotStatus> robots = new Vector();
        
        public void addRobot(RobotStatus robot) {
            robots.add(robot);
            this.fireTableDataChanged();
        }
        
        public void removeAll() {
            robots.clear();
            this.fireTableDataChanged();
        }
        
        @Override
        public int getRowCount() {
            return robots.size();
        }

        @Override
        public int getColumnCount() {
            return 8;
        }

        @Override
        public Object getValueAt(int rowIndex, int columnIndex) {
            RobotStatus robot = robots.get(rowIndex);
            switch(columnIndex) {
                case 0: return robot.teamNum;
                case 1: return robot.playerNum;
                case 2: return robot.getIpAdress();
                case 3: return robot.msgPerSecond;
                case 4: return robot.ballAge;
                case 5: return robot.isDead?"DEAD":(robot.fallen==1?"FALLEN":"NOT FALLEN");
                case 6: return robot.temperature;
                case 7: return robot.batteryCharge;
                default: return null;
            }
        }

        @Override
        public String getColumnName(int column) {
            switch(column) {
                case 0: return "#TN";
                case 1: return "#PN";
                case 2: return "IP";
                case 3: return "msg/s";
                case 4: return "BallAge (s)";
                case 5: return "State";
                case 6: return "Temperature";
                case 7: return "Battery";
                default: return null;
            }
        }

        @Override
        public Class<?> getColumnClass(int columnIndex) {
            return Object.class;
        }
    }
    
    private class BatteryRenderer extends DefaultTableCellRenderer {
        private final float[] dist = { 0.0f , 1.0f };
        private final Color[] colors = { Color.red, Color.green };
        
        @Override
        protected void paintComponent(Graphics g) {
            Graphics2D g2d = (Graphics2D) g;
            float x = Float.parseFloat(getText());
            if(x == -1) {
                this.setOpaque(true);
                this.setText("?");
            } else {
                LinearGradientPaint gp = new LinearGradientPaint(0, 0, getWidth(), getHeight(), dist, colors);
                g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2d.setPaint(gp);
                g2d.fillRect(0, 0, (int)(getWidth()*x/100.0), getHeight());
                this.setOpaque(false);
                this.setText(String.format("%3.1f%%",x));
            }
            super.paintComponent(g2d);
        }
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btListen;
    private javax.swing.JToggleButton btnRecord;
    private javax.swing.JButton btnStopRecording;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JFormattedTextField portNumberOpponent;
    private javax.swing.JFormattedTextField portNumberOwn;
    private javax.swing.JPanel robotStatusPanel;
    private javax.swing.JSplitPane robotStatusSplitPane;
    private javax.swing.JTable robotStatusTable;
    private de.naoth.rc.components.ExtendedFileChooser teamCommFileChooser;
    // End of variables declaration//GEN-END:variables

}
