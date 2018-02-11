package de.naoth.rc.dialogs;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.stream.JsonReader;
import com.google.gson.stream.MalformedJsonException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.dataformats.SPLMessage;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Image;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.DefaultListModel;
import javax.swing.ImageIcon;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JTree;
import javax.swing.ListModel;
import javax.swing.ProgressMonitorInputStream;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.core.dialog.RCDialog;
import java.util.List;

/**
 *
 * @author philipp
 */
public class TeamCommLogViewer extends AbstractDialog
{
    @RCDialog(category = RCDialog.Category.Team, name = "TeamCommLogViewer")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<TeamCommLogViewer> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static TeamCommManager teamcomm;
    }//end Plugin
    
    private TreeMap<Long, Timestamp> messages;
    private long messages_cnt;
    private final DefaultListModel<Timestamp> listMessages = new DefaultListModel<>();
    
    private final DefaultMutableTreeNode treeRootNode;
    private final DefaultTreeModel treeModel;
    
    private TeamCommPlayer teamCommPlayer;

    /**
     * Creates new form TeamCommLogViewer
     */
    public TeamCommLogViewer() {
        treeRootNode = new DefaultMutableTreeNode("Messages in Timestamp");
        treeModel = new DefaultTreeModel(treeRootNode);
        initComponents();
        messageTree.setRootVisible(false);
        messageTree.setCellRenderer(new TreeCellRenderer());
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        teamCommFileChooser = new javax.swing.JFileChooser();
        pmConfig = new javax.swing.JPopupMenu();
        skipDelays = new javax.swing.JCheckBoxMenuItem();
        loopThrough = new javax.swing.JCheckBoxMenuItem();
        ignoreTimestamps = new javax.swing.JCheckBoxMenuItem();
        startWithSelection = new javax.swing.JCheckBoxMenuItem();
        jPanel1 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        timestampList = new javax.swing.JList<>();
        jScrollPane2 = new javax.swing.JScrollPane();
        messageTree = new javax.swing.JTree();
        jToolBar1 = new javax.swing.JToolBar();
        btnTCLF = new javax.swing.JButton();
        jSeparator1 = new javax.swing.JToolBar.Separator();
        btnConfig = new javax.swing.JToggleButton();
        btnPlay = new javax.swing.JToggleButton();
        btnStop = new javax.swing.JButton();
        jSeparator2 = new javax.swing.JToolBar.Separator();
        lblMessages = new javax.swing.JLabel();
        lblTimestamps = new javax.swing.JLabel();

        pmConfig.addPopupMenuListener(new javax.swing.event.PopupMenuListener() {
            public void popupMenuWillBecomeVisible(javax.swing.event.PopupMenuEvent evt) {
            }
            public void popupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {
                pmConfigPopupMenuWillBecomeInvisible(evt);
            }
            public void popupMenuCanceled(javax.swing.event.PopupMenuEvent evt) {
            }
        });

        skipDelays.setSelected(true);
        skipDelays.setText("skip delays");
        skipDelays.setToolTipText("Skips delays in the log file (>1s)");
        pmConfig.add(skipDelays);

        loopThrough.setText("loop");
        loopThrough.setToolTipText("Starts at the beginning, if end of the log file is reached");
        pmConfig.add(loopThrough);

        ignoreTimestamps.setSelected(true);
        ignoreTimestamps.setText("simulate timestamps");
        ignoreTimestamps.setToolTipText("Simulate the timestamps instead using the timestamps of the log file");
        pmConfig.add(ignoreTimestamps);

        startWithSelection.setSelected(true);
        startWithSelection.setText("start with selection");
        startWithSelection.setToolTipText("Begins playing with the selected timestamp");
        pmConfig.add(startWithSelection);

        jPanel1.setLayout(new java.awt.BorderLayout());

        timestampList.setModel(listMessages);
        timestampList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jScrollPane1.setViewportView(timestampList);

        jPanel1.add(jScrollPane1, java.awt.BorderLayout.LINE_START);

        messageTree.setModel(treeModel);
        jScrollPane2.setViewportView(messageTree);

        jPanel1.add(jScrollPane2, java.awt.BorderLayout.CENTER);

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        btnTCLF.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Open24.gif"))); // NOI18N
        btnTCLF.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnTCLFActionPerformed(evt);
            }
        });
        jToolBar1.add(btnTCLF);
        jToolBar1.add(jSeparator1);

        btnConfig.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Preferences24.gif"))); // NOI18N
        btnConfig.setToolTipText("Playing configuration");
        btnConfig.setFocusable(false);
        btnConfig.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnConfig.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnConfig.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnConfigActionPerformed(evt);
            }
        });
        jToolBar1.add(btnConfig);

        btnPlay.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Play24.gif"))); // NOI18N
        btnPlay.setToolTipText("Play TeamComm log file");
        btnPlay.setEnabled(false);
        btnPlay.setFocusable(false);
        btnPlay.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnPlay.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnPlay.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnPlayActionPerformed(evt);
            }
        });
        jToolBar1.add(btnPlay);

        btnStop.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Stop24.gif"))); // NOI18N
        btnStop.setToolTipText("Stop playing TeamComm log file");
        btnStop.setEnabled(false);
        btnStop.setFocusable(false);
        btnStop.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnStop.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnStop.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnStopActionPerformed(evt);
            }
        });
        jToolBar1.add(btnStop);
        jToolBar1.add(jSeparator2);
        jToolBar1.add(lblMessages);
        jToolBar1.add(lblTimestamps);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, 452, Short.MAX_VALUE)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btnTCLFActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnTCLFActionPerformed
        if(teamCommFileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                // before loading new file, cleanup old one
                resetMessages();
                // read the file
                this.messages_cnt = readMessagesFromJsonFile(teamCommFileChooser.getSelectedFile());
                
                this.lblMessages.setToolTipText("Total messages");
                this.lblMessages.setText(" M: "+String.valueOf(messages.size())+" ");
                this.lblTimestamps.setToolTipText("Unique timestamps");
                this.lblTimestamps.setText(" T:"+String.valueOf(messages_cnt)+" ");
                
                btnPlay.setEnabled(true);
                
                // cancel previous "TeamComm simulation"
                stopTeamCommPlayer();
            } catch (MalformedJsonException ex) {
                JOptionPane.showMessageDialog(null, "Unknown format!", "Unknown file format!", JOptionPane.WARNING_MESSAGE);
                return;
            } catch (FileNotFoundException ex) {
                JOptionPane.showMessageDialog(null, "File not found!", "Not found", JOptionPane.WARNING_MESSAGE);
            } catch (IOException ex) {
                Logger.getLogger(TeamCommLogViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
            showMessages();
        }
    }//GEN-LAST:event_btnTCLFActionPerformed

    private void btnPlayActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnPlayActionPerformed
        if(btnPlay.isSelected()) {
            btnStop.setEnabled(true);
            btnPlay.setToolTipText("Pause");
            messageTree.setEnabled(false); // while "playing" disable message view

            // TODO: if "startWithSelection" isn't enabled, "pause" doen't work!?!
            if(!startWithSelection.isSelected()) {
                timestampList.setSelectedIndex(0);
            }
            teamCommPlayer = new TeamCommPlayer(timestampList.getSelectedIndex());
            teamCommPlayer.start();
        } else {
            // "pause" player
            stopTeamCommPlayer();
            btnPlay.setToolTipText("Play TeamComm log file");
            messageTree.setEnabled(true);
            // select/show the last "played" timestamp
            if(!timestampList.isSelectionEmpty()) { updateTree(timestampList.getSelectedValue()); }
        }
    }//GEN-LAST:event_btnPlayActionPerformed

    private void btnStopActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnStopActionPerformed
        stopTeamCommPlayer();
        btnStop.setEnabled(false);
        btnPlay.setSelected(false);
        messageTree.setEnabled(true);
        // select/show the last "played" timestamp
        if(!timestampList.isSelectionEmpty()) { updateTree(timestampList.getSelectedValue()); }
    }//GEN-LAST:event_btnStopActionPerformed

    private void btnConfigActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnConfigActionPerformed
        pmConfig.show(this.btnConfig, 0, btnConfig.getHeight());
    }//GEN-LAST:event_btnConfigActionPerformed

    private void pmConfigPopupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {//GEN-FIRST:event_pmConfigPopupMenuWillBecomeInvisible
        btnConfig.setSelected(false);
    }//GEN-LAST:event_pmConfigPopupMenuWillBecomeInvisible

    private void resetMessages() {
        messages_cnt = 0;
        if(messages != null){ messages.clear(); }
        listMessages.clear();
        treeRootNode.removeAllChildren();
        treeModel.reload();
    }
    
    /**
     * Stops the TeamCommPlayer.
     */
    private void stopTeamCommPlayer() {
        if(teamCommPlayer != null) {
            try {
                teamCommPlayer.stopThread();
                teamCommPlayer.join();
            } catch (InterruptedException ex) {
            }
        }
    }
    
    /**
     * Reads TeamCommMessages from JSON file and returns the number of read messages.
     * @param f JSON file with the TeamCommMessages
     * @return  number of messages
     * @throws FileNotFoundException
     * @throws IOException 
     */
    private int readMessagesFromJsonFile(File f) throws FileNotFoundException, IOException {
        // show progress window (if necessary)
        ProgressMonitorInputStream pmis = new ProgressMonitorInputStream(this, "Reading TeamComm log file", new FileInputStream(f));
        // create/configure Gson-reader
        Gson json = new GsonBuilder().create();
        JsonReader jr = new JsonReader(new InputStreamReader(pmis));

        messages = new TreeMap<>();
        int message_cnt = 0;    
        int message_fail = 0;

        // read the json log file and add each msg to the collection
        jr.beginArray();
        while (jr.hasNext()) {
            TeamCommMessage p = json.fromJson(jr, TeamCommMessage.class);
            if(p.message != null) {
                // custom part doesn't get (de-)serialized, but we have everything in data!
                p.message.parseCustomFromData();
                if(!messages.containsKey(p.timestamp)) {
                    messages.put(p.timestamp, new Timestamp(p.timestamp));
                }
                messages.get(p.timestamp).addMessage(p);
                message_cnt++;
            } else {
                message_fail++;
            }
        }
        jr.endArray();
        jr.close();
        
        if(message_fail > 0) {
            // TODO: display, where it is useful?!
            System.out.println("Failed to read " + message_fail + " messages!");
        }
        
        return message_cnt;
    }
    
    /**
     * Shows the timestamps of the read log file in the jList.
     */
    private void showMessages() {
        // clear previous loaded messages
        timestampList.clearSelection();
        timestampList.addListSelectionListener(new SelectionListener());
        
        // add all messages to list (if available)
        if(messages == null || messages.size() == 0) {
            JOptionPane.showMessageDialog(null, "No messeges read ...", "No messages", JOptionPane.INFORMATION_MESSAGE);
        } else {
            for (Map.Entry<Long, Timestamp> entry : messages.entrySet()) {
                listMessages.addElement(entry.getValue());
            }
        }
    }
    
    /**
     * Updates the tree component and shows the TeamCommMessages of the timestamp.
     * @param ts 
     */
    public void updateTree(Timestamp ts) {
        // clear previous message tree
        treeRootNode.removeAllChildren();
        
        for (int i = 0; i < ts.messages.size(); i++) {
            TeamCommMessage teamCommMessage = ts.messages.get(i);
            MessageTreeNode msgNode = new MessageTreeNode("#"+(i+1)+" message", teamCommMessage.isOpponent());
            // add tree nodes for each message field
            SPLMessage spl = teamCommMessage.message;
            msgNode.add(new DefaultMutableTreeNode("playerNum=" + spl.playerNum));
            msgNode.add(new DefaultMutableTreeNode("teamNum=" + spl.teamNum));
            msgNode.add(new DefaultMutableTreeNode("fallen=" + spl.fallen));
            msgNode.add(new DefaultMutableTreeNode("pose_x=" + spl.pose_x));
            msgNode.add(new DefaultMutableTreeNode("pose_y=" + spl.pose_y));
            msgNode.add(new DefaultMutableTreeNode("pose_a=" + spl.pose_a));
            msgNode.add(new DefaultMutableTreeNode("walkingTo_x=" + spl.walkingTo_x));
            msgNode.add(new DefaultMutableTreeNode("walkingTo_y=" + spl.walkingTo_y));
            msgNode.add(new DefaultMutableTreeNode("shootingTo_x=" + spl.shootingTo_x));
            msgNode.add(new DefaultMutableTreeNode("shootingTo_y=" + spl.shootingTo_y));
            msgNode.add(new DefaultMutableTreeNode("ballAge=" + spl.ballAge));
            msgNode.add(new DefaultMutableTreeNode("ball_x=" + spl.ball_x));
            msgNode.add(new DefaultMutableTreeNode("ball_y=" + spl.ball_y));
            msgNode.add(new DefaultMutableTreeNode("ballVel_x=" + spl.ballVel_x));
            msgNode.add(new DefaultMutableTreeNode("ballVel_y=" + spl.ballVel_y));
            msgNode.add(new DefaultMutableTreeNode("suggestion=" + Arrays.toString(spl.suggestion)));
            msgNode.add(new DefaultMutableTreeNode("intention=" + spl.intention));
            msgNode.add(new DefaultMutableTreeNode("averageWalkSpeed=" + spl.averageWalkSpeed));
            msgNode.add(new DefaultMutableTreeNode("maxKickDistance=" + spl.maxKickDistance));
            msgNode.add(new DefaultMutableTreeNode("currentPositionConfidence=" + spl.currentPositionConfidence));
            msgNode.add(new DefaultMutableTreeNode("currentSideConfidence=" + spl.currentSideConfidence));
            msgNode.add(new DefaultMutableTreeNode("numOfDataBytes=" + spl.numOfDataBytes));
            msgNode.add(new DefaultMutableTreeNode("data=" + Arrays.toString(spl.data)));
//            msgNode.add(new DefaultMutableTreeNode("user=" + spl.user));
            treeRootNode.add(msgNode);
        }
        // show all messages
        treeModel.reload();
        for (int i = 0; i < messageTree.getRowCount(); i++) {
            messageTree.expandRow(i);
        }
    }
    
    private class Timestamp {
        public final long timestamp;
        public final ArrayList<TeamCommMessage> messages = new ArrayList<>();

        public Timestamp(long ts) {
            timestamp = ts;
        }
        
        public void addMessage(TeamCommMessage msg) {
            messages.add(msg);
        }
        
        public int size() {
            return messages.size();
        }
        
        public String toString() {
            return timestamp + " ("+messages.size()+")";
        }
    }
    
    /**
     * 
     */
    private class SelectionListener implements ListSelectionListener {
        @Override
        public void valueChanged(ListSelectionEvent lse) {
            // only update "message tree" if "active" and fire event only once
            if(messageTree.isEnabled() && !lse.getValueIsAdjusting() && !timestampList.isSelectionEmpty()) {
                updateTree(timestampList.getSelectedValue());
            }
        }
    }
    
    private class TreeCellRenderer extends DefaultTreeCellRenderer {
        private final ImageIcon ico_blue;
        private final ImageIcon ico_red;
        
        public TreeCellRenderer() {
            super();
            // message fields shouldn't have an icon
            this.setLeafIcon(null);
            // preload icons for "the" message 
            ImageIcon ico_red = new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/vrml/textures/button_red.png"));
            ImageIcon ico_blue = new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/vrml/textures/button_blue.png"));
            // scale image ...
            this.ico_red = scaleImage(ico_red, 24, 24);
            this.ico_blue = scaleImage(ico_blue, 24, 24);
        }
        
        @Override
        public Component getTreeCellRendererComponent(JTree jtree, Object o, boolean bln, boolean bln1, boolean bln2, int i, boolean bln3) {
            // get the default tree cell
            Component result = super.getTreeCellRendererComponent(jtree, o, bln, bln1, bln2, i, bln3);
            // if cell is "the message", set icon and font
            if(o instanceof MessageTreeNode) {
                result.setForeground(((MessageTreeNode) o).isOpponent?Color.RED:Color.BLUE);
                result.setFont(new Font("Sans Serif", Font.BOLD, 11));
                this.setIcon(((MessageTreeNode) o).isOpponent?this.ico_red:this.ico_blue);
            } else {
                // message fields get "normal" font
                result.setFont(new Font("Sans Serif", Font.PLAIN, 11));
            }
            return result;
        }
        
        /**
         * Scales the given image.
         * @param img
         * @param width
         * @param height
         * @return 
         */
        private ImageIcon scaleImage(ImageIcon img, int width, int height) {
            Image image = img.getImage(); // transform it
            Image newimg = image.getScaledInstance(width, height, java.awt.Image.SCALE_SMOOTH); // scale it the smooth way
            return new ImageIcon(newimg); // transform it back
        }
    }
    
    private class MessageTreeNode extends DefaultMutableTreeNode {
        boolean isOpponent;
        private MessageTreeNode(String string, boolean opponent) {
            super(string);
            isOpponent = opponent;
        }
    }
    
    private class TeamCommPlayer extends Thread {

        private final ListModel<Timestamp> timestamps;
        private int it;
        private long prevTimestamp = 0;
        private boolean isRunning = true;
        
        public TeamCommPlayer(int startIndex) {
            timestamps = timestampList.getModel();
            it = startIndex < 0 ? 0 : startIndex;
        }
        
        @Override
        public void run() {
            if(timestamps == null) {
                return;
            }
            while (isRunning && it < timestamps.getSize()) {
                // put current messages (of the timestamp) to the message drawing "buffer"
                Timestamp current = timestamps.getElementAt(it);
                // selects the currently "viewed" timestamp in the timestampList
                timestampList.setSelectedValue(current, true);
                List<TeamCommMessage> c = new ArrayList<>();
                for (TeamCommMessage teamCommMessage : current.messages) {
                    c.add(new TeamCommMessage(
                        // replaces the log file timestamp with the current timestamp; robot isn't "DEAD"!
                        ignoreTimestamps.isSelected() ? System.currentTimeMillis() : current.timestamp,
                        teamCommMessage.address,
                        teamCommMessage.message,
                        teamCommMessage.isOpponent())
                    );
                }
                Plugin.teamcomm.receivedMessages(c);
                // simulate the delay between the arrival of subsequent messages
                try {
                    long sleeping = prevTimestamp == 0 ? 0 : (current.timestamp - prevTimestamp);
                    // if delays should be skipped, every delay > 1 second is set to 33ms
                    sleep((skipDelays.isSelected() && sleeping > 1000) || (sleeping < 0) ? 33 : sleeping);
                } catch (InterruptedException ex) {}
                
                prevTimestamp = current.timestamp;
                it++;
                
                // if we should loop through the log file and reached the end - reset to the beginning
                if(loopThrough.isSelected() && it >= timestamps.getSize()) {
                    it = 0;
                    prevTimestamp = 0;
                }
            }
            
            // end of the log file
            isRunning = false;
            btnStop.doClick(); // handle it, as if the user clicke the stop button (update UI!)
        }
  
        public void stopThread() {
            isRunning = false;
            interrupt(); // if sleeping
        }
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btnConfig;
    private javax.swing.JToggleButton btnPlay;
    private javax.swing.JButton btnStop;
    private javax.swing.JButton btnTCLF;
    private javax.swing.JCheckBoxMenuItem ignoreTimestamps;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JToolBar.Separator jSeparator1;
    private javax.swing.JToolBar.Separator jSeparator2;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JLabel lblMessages;
    private javax.swing.JLabel lblTimestamps;
    private javax.swing.JCheckBoxMenuItem loopThrough;
    private javax.swing.JTree messageTree;
    private javax.swing.JPopupMenu pmConfig;
    private javax.swing.JCheckBoxMenuItem skipDelays;
    private javax.swing.JCheckBoxMenuItem startWithSelection;
    private javax.swing.JFileChooser teamCommFileChooser;
    private javax.swing.JList<Timestamp> timestampList;
    // End of variables declaration//GEN-END:variables
}
