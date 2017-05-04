/**
 * TeamCommViewer.java
 *
 * Created on 08.11.2010, 21:41:28
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.teamcommviewer.TeamCommLogger;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcommviewer.RobotStatus;
import de.naoth.rc.components.teamcommviewer.RobotStatusPanel;
import de.naoth.rc.components.teamcommviewer.RobotStatusTable;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.components.teamcommviewer.RobotTeamCommListener;
import de.naoth.rc.components.teamcommviewer.TeamColorMenu;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.drawings.DrawingCollection;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JFileChooser;
import javax.swing.MenuElement;
import javax.swing.SwingUtilities;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public class TeamCommViewer extends AbstractDialog {

    @RCDialog(category = RCDialog.Category.Team, name = "Communication Viewer")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<TeamCommViewer> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static DrawingEventManager drawingEventManager;
        @InjectPlugin
        public static TeamCommManager teamcommManager;
    }//end Plugin

    private Timer timerCheckMessages;
    
    /**
     * The robot-TeamCommProvider for our own team.
     */
    private final RobotTeamCommListener listenerOwn = new RobotTeamCommListener(false);
    
    /**
     * The robot-TeamCommProvider for our own team.
     */
    private final RobotTeamCommListener listenerOpponent = new RobotTeamCommListener(true);

    /**
     * Receiver for the TeamMessages.
     */
    private final TeamCommMessageListener teamcommListener = new TeamCommMessageListener();
    
    /**
     * Buffer, which stores the last TeamCommMessage of every robot.
     */
    public final Map<String, TeamCommMessage> messageBuffer = Collections.synchronizedMap(new TreeMap<String, TeamCommMessage>());
    
    /**
     * Container for storing the current status of each robot.
     */
    private final TreeMap<String, RobotStatus> robotsMapSorted = new TreeMap<>();

    /**
     * Container for storing the team color.
     */
    private final TreeMap<Integer, Color> teamColor = new TreeMap<>();

    /**
     * Logger object, for logging TeamCommMessages.
     */
    private final TeamCommLogger log = new TeamCommLogger();

    /**
     * Creates new form TeamCommViewer
     */
    public TeamCommViewer() {
        initComponents();
        // collapse pane
        robotStatusSplitPane.setDividerLocation(0);/*2000, Integer.MAX_VALUE*/
        // add additional columns to popup menu
        addAdditionalColumnsToPopupMenu();
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
        pmAdditionalColumns = new javax.swing.JPopupMenu();
        pmTeamColor = new javax.swing.JPopupMenu();
        toolbarPanel = new javax.swing.JPanel();
        btListen = new javax.swing.JToggleButton();
        jLabel1 = new javax.swing.JLabel();
        portNumberOwn = new javax.swing.JFormattedTextField();
        jLabel2 = new javax.swing.JLabel();
        portNumberOpponent = new javax.swing.JFormattedTextField();
        btnTeamColor = new javax.swing.JToggleButton();
        btnRecord = new javax.swing.JToggleButton();
        btnStopRecording = new javax.swing.JButton();
        btnAddtionalColumns = new javax.swing.JToggleButton();
        contentPanel = new javax.swing.JPanel();
        robotStatusSplitPane = new javax.swing.JSplitPane();
        jScrollPane2 = new javax.swing.JScrollPane();
        robotStatusPanel = new javax.swing.JPanel();
        robotStatusTable = new de.naoth.rc.components.teamcommviewer.RobotStatusTable();

        teamCommFileChooser.setDialogType(javax.swing.JFileChooser.SAVE_DIALOG);
        teamCommFileChooser.setDialogTitle("Log file location");
        teamCommFileChooser.setSelectedFile(new File((new SimpleDateFormat("yyyy-MM-dd")).format(new Date())+"_TeamComm.log"));
        teamCommFileChooser.setToolTipText("");

        pmAdditionalColumns.addPopupMenuListener(new javax.swing.event.PopupMenuListener() {
            public void popupMenuWillBecomeVisible(javax.swing.event.PopupMenuEvent evt) {
            }
            public void popupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {
                pmAdditionalColumnsPopupMenuWillBecomeInvisible(evt);
            }
            public void popupMenuCanceled(javax.swing.event.PopupMenuEvent evt) {
            }
        });

        pmTeamColor.addPopupMenuListener(new javax.swing.event.PopupMenuListener() {
            public void popupMenuWillBecomeVisible(javax.swing.event.PopupMenuEvent evt) {
                pmTeamColorPopupMenuWillBecomeVisible(evt);
            }
            public void popupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {
                pmTeamColorPopupMenuWillBecomeInvisible(evt);
            }
            public void popupMenuCanceled(javax.swing.event.PopupMenuEvent evt) {
            }
        });

        setLayout(new java.awt.BorderLayout());

        toolbarPanel.setMaximumSize(new java.awt.Dimension(10, 10));
        toolbarPanel.setLayout(new javax.swing.BoxLayout(toolbarPanel, javax.swing.BoxLayout.LINE_AXIS));

        btListen.setText("Listen to TeamComm");
        btListen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btListenActionPerformed(evt);
            }
        });
        toolbarPanel.add(btListen);

        jLabel1.setText("Blue:");
        toolbarPanel.add(jLabel1);

        portNumberOwn.setColumns(6);
        portNumberOwn.setFormatterFactory(new javax.swing.text.DefaultFormatterFactory(new javax.swing.text.NumberFormatter(new java.text.DecimalFormat("#"))));
        portNumberOwn.setText("10004");
        portNumberOwn.setToolTipText("Own team port number");
        portNumberOwn.setMinimumSize(new java.awt.Dimension(76, 19));
        toolbarPanel.add(portNumberOwn);

        jLabel2.setText("Red:");
        toolbarPanel.add(jLabel2);

        portNumberOpponent.setColumns(6);
        portNumberOpponent.setFormatterFactory(new javax.swing.text.DefaultFormatterFactory(new javax.swing.text.NumberFormatter(new java.text.DecimalFormat("#"))));
        portNumberOpponent.setToolTipText("Opponent team port number");
        portNumberOpponent.setMinimumSize(new java.awt.Dimension(76, 19));
        toolbarPanel.add(portNumberOpponent);

        btnTeamColor.setIcon(new javax.swing.ImageIcon(getClass().getResource("/data/bibliothek/gui/dock/core/flap_auto.png"))); // NOI18N
        btnTeamColor.setToolTipText("Change color of teams.");
        btnTeamColor.setEnabled(false);
        btnTeamColor.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnTeamColorActionPerformed(evt);
            }
        });
        toolbarPanel.add(btnTeamColor);

        btnRecord.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/play.png"))); // NOI18N
        btnRecord.setText("Record TeamComm");
        btnRecord.setActionCommand("RecordTeamComm");
        btnRecord.setMaximumSize(new java.awt.Dimension(193, 25));
        btnRecord.setMinimumSize(new java.awt.Dimension(193, 25));
        btnRecord.setPreferredSize(new java.awt.Dimension(193, 25));
        btnRecord.setSelectedIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png"))); // NOI18N
        btnRecord.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnRecordActionPerformed(evt);
            }
        });
        toolbarPanel.add(btnRecord);

        btnStopRecording.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Stop24.gif"))); // NOI18N
        btnStopRecording.setToolTipText("Stops recording and closes log file.");
        btnStopRecording.setEnabled(false);
        btnStopRecording.setMaximumSize(new java.awt.Dimension(58, 25));
        btnStopRecording.setMinimumSize(new java.awt.Dimension(58, 25));
        btnStopRecording.setPreferredSize(new java.awt.Dimension(58, 25));
        btnStopRecording.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnStopRecordingActionPerformed(evt);
            }
        });
        toolbarPanel.add(btnStopRecording);

        btnAddtionalColumns.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Preferences16.gif"))); // NOI18N
        btnAddtionalColumns.setToolTipText("Add/Remove columns from RobotStatusTable");
        btnAddtionalColumns.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnAddtionalColumnsActionPerformed(evt);
            }
        });
        toolbarPanel.add(btnAddtionalColumns);

        add(toolbarPanel, java.awt.BorderLayout.NORTH);

        robotStatusSplitPane.setBorder(null);
        robotStatusSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        robotStatusSplitPane.setOneTouchExpandable(true);

        jScrollPane2.setMinimumSize(new java.awt.Dimension(0, 0));
        jScrollPane2.setPreferredSize(new java.awt.Dimension(0, 22));

        robotStatusPanel.setMinimumSize(new java.awt.Dimension(0, 0));
        robotStatusPanel.setLayout(new java.awt.GridLayout(5, 1, 0, 5));
        jScrollPane2.setViewportView(robotStatusPanel);

        robotStatusSplitPane.setLeftComponent(jScrollPane2);
        robotStatusSplitPane.setRightComponent(robotStatusTable);

        javax.swing.GroupLayout contentPanelLayout = new javax.swing.GroupLayout(contentPanel);
        contentPanel.setLayout(contentPanelLayout);
        contentPanelLayout.setHorizontalGroup(
            contentPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
            .addGroup(contentPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(robotStatusSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 867, Short.MAX_VALUE))
        );
        contentPanelLayout.setVerticalGroup(
            contentPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 518, Short.MAX_VALUE)
            .addGroup(contentPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(robotStatusSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 518, Short.MAX_VALUE))
        );

        add(contentPanel, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

    private void btListenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btListenActionPerformed
        try {
            // start listening to TeamComm ...
            if (this.btListen.isSelected()) {
                // get own port number and start/connect robot TeamCommProvider
                String ownPortRaw = portNumberOwn.getText().trim();
                if(!ownPortRaw.isEmpty()) {
                    int portOwn = Integer.parseInt(ownPortRaw);
                    listenerOwn.connect(portOwn);
                }
                // get opponent port number and start/connect robot TeamCommProvider
                String opponentPortRaw = portNumberOpponent.getText().trim();
                if (!opponentPortRaw.isEmpty()) {
                    int portOpponent = Integer.parseInt(opponentPortRaw);
                    listenerOpponent.connect(portOpponent);
                }
                // listen to TeamCommMessages
                Plugin.teamcommManager.addListener(teamcommListener);
                // start/schedule UI-updater
                this.timerCheckMessages = new Timer();
                this.timerCheckMessages.scheduleAtFixedRate(new TeamCommListenTask(), 100, 33);
                // update UI
                this.portNumberOwn.setEnabled(false);
                this.portNumberOpponent.setEnabled(false);
                this.robotStatusPanel.setVisible(true);
                this.robotStatusTable.setVisible(true);
            } else {
                // stops/disconnects the robot TeamCommProvider
                listenerOwn.disconnect();
                listenerOpponent.disconnect();
                // remove TeamCommMessage listener
                Plugin.teamcommManager.removeListener(teamcommListener);
                // stop UI-updater
                if(this.timerCheckMessages != null) {
                    this.timerCheckMessages.cancel();
                    this.timerCheckMessages.purge();
                    this.timerCheckMessages = null;
                }
                // clear collections and "enable" UI
                synchronized (messageBuffer) {
                    this.messageBuffer.clear();
                }
                this.robotsMapSorted.clear();
                // update UI
                this.robotStatusPanel.removeAll();
                this.robotStatusPanel.setVisible(false);
                this.robotStatusTable.setVisible(false);
                this.robotStatusTable.removeAll();
                this.portNumberOwn.setEnabled(true);
                this.portNumberOpponent.setEnabled(true);
                this.btnTeamColor.setEnabled(false);
            }
        } catch (NumberFormatException ex) {
            Helper.handleException("Invalid port number", ex);
        } catch (IOException | InterruptedException ex) {
            Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
        }
    }//GEN-LAST:event_btListenActionPerformed

    private void btnRecordActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnRecordActionPerformed
        // record-button got pressed
        if(btnRecord.isSelected()) {
            // log file already set; just enable logging
            if(this.log.isActive()) {
                this.log.resumeLogging();
                btnRecord.setSelected(true);
                setBtnRecordToolTipText(true);
            } else {
                // select log file and enable logging!
                this.setLogFile();
            }
        // release button
        } else {
            this.log.pauseLogging();
        }
    }//GEN-LAST:event_btnRecordActionPerformed

    private void btnStopRecordingActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnStopRecordingActionPerformed
        // stop log file recording, flush and close logfile
        this.log.stopLogging();
        // ... update UI
        btnRecord.setSelected(false);
        setBtnRecordToolTipText(false);
        btnStopRecording.setEnabled(false);
    }//GEN-LAST:event_btnStopRecordingActionPerformed

    private void btnAddtionalColumnsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnAddtionalColumnsActionPerformed
        if(btnAddtionalColumns.isSelected()) {
            pmAdditionalColumns.show(btnAddtionalColumns, 0, btnAddtionalColumns.getHeight());
        }
    }//GEN-LAST:event_btnAddtionalColumnsActionPerformed

    private void pmAdditionalColumnsPopupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {//GEN-FIRST:event_pmAdditionalColumnsPopupMenuWillBecomeInvisible
        btnAddtionalColumns.setSelected(false);
    }//GEN-LAST:event_pmAdditionalColumnsPopupMenuWillBecomeInvisible

    private void btnTeamColorActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnTeamColorActionPerformed
        if(btnTeamColor.isSelected()) {
           pmTeamColor.show(btnTeamColor, 0, btnTeamColor.getHeight());
        }
    }//GEN-LAST:event_btnTeamColorActionPerformed

    private void pmTeamColorPopupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {//GEN-FIRST:event_pmTeamColorPopupMenuWillBecomeInvisible
        btnTeamColor.setSelected(false);
    }//GEN-LAST:event_pmTeamColorPopupMenuWillBecomeInvisible

    private void pmTeamColorPopupMenuWillBecomeVisible(javax.swing.event.PopupMenuEvent evt) {//GEN-FIRST:event_pmTeamColorPopupMenuWillBecomeVisible
        // remove all previous menu items, we're creating them afterwards, so we doesn't miss any new teams
        pmTeamColor.removeAll();
        // iterate over all teams
        teamColor.entrySet().stream().forEach((entry) -> {
            // ... and create menu item
            TeamColorMenu teamMenu = new TeamColorMenu(entry.getKey(), entry.getValue());
            // add listener for "color change"
            teamMenu.addActionListener((e) -> {
                // the menu item has the (new) color
                TeamColorMenu tm = (TeamColorMenu)e.getSource();
                teamColor.replace(tm.teamNumber, tm.getBackground());
            });
            pmTeamColor.add(teamMenu);
        });
    }//GEN-LAST:event_pmTeamColorPopupMenuWillBecomeVisible

    @Override
    public void dispose() {
        // TODO: should we stop all listener-threads?!
        this.log.stopLogging();
    }

    private boolean setLogFile() {
        // open filechooser
        if(teamCommFileChooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            // if selected file has a file extension - use this, otherwise append ".log" to the filename
            File dfile = (teamCommFileChooser.getSelectedFile().getName().lastIndexOf(".") == -1) ? 
                new File(teamCommFileChooser.getSelectedFile()+".log") : 
                teamCommFileChooser.getSelectedFile();

            // set log file, start logging and update ui
            if(this.log.setLogFile(dfile)) {
                this.log.startLogging();
                btnRecord.setSelected(true);
                setBtnRecordToolTipText(true);
                btnStopRecording.setEnabled(true); // enable "stop"-button

                return true;
            }
        }
        
        btnRecord.setSelected(false); // prevent toggling!
        setBtnRecordToolTipText(false);
        
        return false;
    }
    
    private void setBtnRecordToolTipText(boolean stop){
        String name = this.log.isActive() ? this.log.getLogfile().getName() : "<not-set>";
        if(!stop) {
            btnRecord.setToolTipText("Start recording to: " + name);
        } else {
            btnRecord.setToolTipText("Pause recording to: " + name);
        }
        
    }
    
    private void saveColumnConfiguration() {
        ArrayList<String> items = new ArrayList<>();
        for (MenuElement item : pmAdditionalColumns.getSubElements()) {
            if(((JCheckBoxMenuItem)item).isSelected()) {
                items.add(((JCheckBoxMenuItem)item).getActionCommand());
            }
        }
        Plugin.parent.getConfig().setProperty(this.getClass().getName()+".ColumnConfig", items.stream().collect(Collectors.joining("|")));
    }
    
    /**
     * Adds table column configuration popup-menu.
     * The column configuration is read from the config file and is applied to the robot status table.
     * For each possible column, a action listener is added for updating the table and saving "new" configuration.
     */
    private void addAdditionalColumnsToPopupMenu() {
        // try to get column configuration
        String columnConfigStr = Plugin.parent.getConfig().getProperty(this.getClass().getName()+".ColumnConfig", "");
        List<String> columnConfig = columnConfigStr.isEmpty() ? null : Arrays.asList(columnConfigStr.split("\\|"));
        // add all available columns to configuration popup menu
        for (Iterator<RobotStatusTable.Column> it = robotStatusTable.ALL_COLUMNS.iterator(); it.hasNext();) {
            RobotStatusTable.Column col = it.next();
            // skip "empty" columns, e.g. the connect-button column
            if(col.name == null || col.name.isEmpty()) { continue; }
            JCheckBoxMenuItem item = new JCheckBoxMenuItem(col.name);
            // shows the default columns or - if set - the saved column configuration
            if((columnConfig == null && col.showbydefault) || (columnConfig != null && columnConfig.contains(col.name))) {
                robotStatusTable.addColumn(col.name);
                item.setSelected(true);
            }
            item.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    JCheckBoxMenuItem item = (JCheckBoxMenuItem)e.getSource();
                    if(item.isSelected()) {
                        robotStatusTable.addColumn(e.getActionCommand());
                    } else {
                        robotStatusTable.removeColumn(e.getActionCommand());
                    }
                    saveColumnConfiguration();
                }
            });
            pmAdditionalColumns.add(item);
        }
    }
    
    /**
     * Waits for new TeamCommMessages and writes them to the "message-buffer".
     */
    public class TeamCommMessageListener implements de.naoth.rc.components.teamcomm.TeamCommListener {

        @Override
        public void newTeamCommMessages(List<TeamCommMessage> messages) {
            if (!messages.isEmpty()) {
                synchronized (messageBuffer) {
                    // "convert" list to map using address as key
                    messageBuffer.putAll(messages.stream().collect(Collectors.toMap(
                        m -> m.address == null ? m.message.teamNum+".0.0."+m.message.playerNum : m.address,
                        m -> m,
                        (m1, m2) -> m2 // if two messages are from the same robot, take the last one
                    )));
                }
            }
        }
    }

    /**
     * Iterates periodically over message-buffer and updates the robotStatus and the UI.
     */
    private class TeamCommListenTask extends TimerTask {

        @Override
        public void run() {
            synchronized (messageBuffer) {
                if (messageBuffer.isEmpty()) {
                    return;
                }
                DrawingCollection drawings = new DrawingCollection();
                for (Entry<String, TeamCommMessage> msgEntry : messageBuffer.entrySet()) {
                    final String address = msgEntry.getKey();
                    final TeamCommMessage msg = msgEntry.getValue();
                    
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run() {
                            RobotStatus robotStatus = robotsMapSorted.get(address);
                            // ... new robot ...
                            if (robotStatus == null) {
                                robotStatus = new RobotStatus(Plugin.parent.getMessageServer(), address);
                                // adds RobotStatus to a container
                                robotsMapSorted.put(address, robotStatus);
                                // clears the panel-view
                                robotStatusPanel.removeAll();
                                // re-inserts all robotStatus to make sure the robots are ordered
                                robotsMapSorted.entrySet().stream().forEach((entry) -> {
                                    robotStatusPanel.add(new RobotStatusPanel(entry.getValue()));
                                });
                                // adds the robotStatus to the table-view
                                robotStatusTable.addRobot(robotStatus);
                                // activates team color picker
                                btnTeamColor.setEnabled(true);
                            }
                            // updates the robotStatus
                            robotStatus.updateStatus(msg.timestamp, msg.message);
                            // new team -> add color
                            if(!teamColor.containsKey((int)robotStatus.teamNum)) {
                                teamColor.put(
                                    (int)robotStatus.teamNum, 
                                    // opponent gets "red" by default
                                    msg.isOpponent() ? Color.RED : (
                                        // "our" team gets "blue" or another
                                        teamColor.isEmpty() ? 
                                            Color.BLUE : 
                                            // next color in the list ...
                                            TeamColorMenu.getNextColor(teamColor.lastEntry().getValue())
                                    )
                                );
                            }
                            robotStatus.robotColor =  teamColor.get((int)robotStatus.teamNum);
                        }
                    });
                    // if enabled, draw robots on the FieldViewer otherwise not
                    RobotStatus rs = robotsMapSorted.get(address);
                    if(rs != null && rs.showOnField) {
                        msg.message.draw(drawings, rs.robotColor, msg.isOpponent());
                    }
                } // end for
                Plugin.drawingEventManager.fireDrawingEvent(drawings);
            } // end synchronized
        } // end run
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btListen;
    private javax.swing.JToggleButton btnAddtionalColumns;
    private javax.swing.JToggleButton btnRecord;
    private javax.swing.JButton btnStopRecording;
    private javax.swing.JToggleButton btnTeamColor;
    private javax.swing.JPanel contentPanel;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JPopupMenu pmAdditionalColumns;
    private javax.swing.JPopupMenu pmTeamColor;
    private javax.swing.JFormattedTextField portNumberOpponent;
    private javax.swing.JFormattedTextField portNumberOwn;
    private javax.swing.JPanel robotStatusPanel;
    private javax.swing.JSplitPane robotStatusSplitPane;
    private de.naoth.rc.components.teamcommviewer.RobotStatusTable robotStatusTable;
    private de.naoth.rc.components.ExtendedFileChooser teamCommFileChooser;
    private javax.swing.JPanel toolbarPanel;
    // End of variables declaration//GEN-END:variables

}
