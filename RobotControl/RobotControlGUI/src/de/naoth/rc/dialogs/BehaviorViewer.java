/*
 * FieldViewer.java
 *
 * Created on 1. Mai 2008, 00:02
 */
package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.AbstractDialog;
import de.naoth.rc.Dialog;
import de.naoth.rc.DialogPlugin;
import de.naoth.rc.RobotControl;
import de.naoth.rc.dialogs.behaviorviewer.model.Symbol;
import de.naoth.rc.dialogs.behaviorviewer.XABSLBehavior;
import de.naoth.rc.dialogs.behaviorviewer.XABSLBehaviorFrame;
import de.naoth.rc.dialogs.behaviorviewer.XABSLProtoParser;
import de.naoth.rc.dialogs.drawings.Robot;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.logmanager.BlackBoard;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.logmanager.LogFrameListener;
import de.naoth.rc.manager.DebugDrawingManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.manager.SwingCommandExecutor;
import de.naoth.rc.messages.Messages;
import de.naoth.rc.messages.Messages.XABSLParameter;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.SortedSet;
import java.util.TreeSet;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.PatternSyntaxException;
import javax.swing.AbstractListModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */

public class BehaviorViewer extends AbstractDialog
  implements  Dialog
{

  @PluginImplementation
  public static class Plugin extends DialogPlugin<BehaviorViewer>
  {
      @InjectPlugin
      public static RobotControl parent;
      @InjectPlugin
      public static GenericManagerFactory genericManagerFactory;
      @InjectPlugin
      public static DebugDrawingManager debugDrawingManager;
      @InjectPlugin
      public static SwingCommandExecutor commandExecutor;
      @InjectPlugin
      public static LogFileEventManager logFileEventManager;
      @InjectPlugin
      public static DrawingEventManager drawingEventManager;
  }//end Plugin
  

  private final String fileWriteCommandName = "file::write";
  private final String behaviorPath = "Config/behavior-ic.dat";
  private final String setAgentCommand = "behavior:set_agent";
  private final String setAgentCommandParam = "agent";

  private final Command reloadBehaviorCommand = new Command("behavior:reload");
  private final Command getAgentCommand = new Command("behavior:get_agent");
  
  private final Command getBehaviorStateComplete = new Command("behavior:state");
  private final Command getBehaviorStateSparse = new Command("behavior:state_sparse");
  

  private final Command getListOfAgents = new Command("behavior:list_agents");

  ArrayList<XABSLBehaviorFrame> behaviorBuffer;
  private XABSLBehavior currentBehavior;
  public static final Color DARK_GREEN = new Color(0, 128, 0);
  public static final Font PLAIN_FONT = new Font("Sans Serif", Font.PLAIN, 11);
  public static final Font BOLD_FONT = new Font("Sans Serif", Font.BOLD, 11);
  public static final Font ITALIC_FONT = new Font("Sans Serif", Font.ITALIC, 11);
  final private String behaviorConfKey = "behavior";
  final private String defaultBehavior = "../NaoController/Config/behavior/behavior-ic.dat";

  private XABSLProtoParser behaviorParser = null;
  
  private boolean vetoSetAgent = false;
  //private String currentAgent = "";

  // max size of the buffer (25fps ~ 20s)
  private int maxNumberOfFrames = 500;
  
  
  private BehaviorFrameListener behaviorFrameListener = new BehaviorFrameListener();

  private BehaviorListener behaviorListener = new BehaviorListener();
  private BehaviorUpdateListener behaviorUpdateListener = new BehaviorUpdateListener();
  private LogBehaviorListener logBehaviorListener = new LogBehaviorListener();
  
  /** Creates new form BehaviorViewer */
  public BehaviorViewer()
  {
    initComponents();

    this.frameList.setModel(new BehaviorFrameListModel()); // new DefaultListModel());
    this.frameList.addListSelectionListener(behaviorFrameListener);

    this.frameList.setCellRenderer(new DefaultListCellRenderer() {
      final Color darkBlue = new Color(0.0f, 0.0f, 1.0f);
      final Color skyBlue = new Color(0.5f, 0.5f, 1.0f);
      final Color lightBlue = new Color(0.9f, 0.9f, 1.0f);

      @Override
      public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
        Component component = super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);

        if(!isSelected && index > 0)
        {
          int previousElement = (Integer)frameList.getModel().getElementAt(index-1);
          int currentValue = (Integer)value;

          if(currentValue - previousElement > 3)
            component.setBackground(darkBlue);
          else if(currentValue - previousElement > 2)
            component.setBackground(skyBlue);
          else if(currentValue - previousElement > 1)
            component.setBackground(lightBlue);
        }
        return component;
      }
    });


    this.behaviorBuffer = new ArrayList<>();
  }

  class SymbolComperator implements Comparator<Messages.XABSLParameter>
  {
    @Override
    public int compare(XABSLParameter o1, XABSLParameter o2) {
      return o1.getName().compareTo(o2.getName());
    }
  }//end SymbolComperator


  class BehaviorListener implements ObjectListener<byte[]>
  {
        @Override
        public void newObjectReceived(byte[] object) {
            try
            {
              Messages.BehaviorStateComplete behavior_msg = Messages.BehaviorStateComplete.parseFrom(object);
              
              behaviorParser = new XABSLProtoParser();
              currentBehavior =  behaviorParser.parse(behavior_msg);
            }
            catch(InvalidProtocolBufferException ex)
            {
              Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        @Override
        public void errorOccured(String cause) {
            
        }
  }
  
  class BehaviorUpdateListener implements ObjectListener<byte[]>
  {
        @Override
        public void newObjectReceived(byte[] object) {
            if(behaviorParser == null) {
                return;
            }
            
            try
            {
              Messages.BehaviorStateSparse status = Messages.BehaviorStateSparse.parseFrom(object);
              final XABSLBehaviorFrame frame = behaviorParser.parse(status);
              
             
              SwingUtilities.invokeLater(new Runnable() {
                  @Override
                  public void run() {
                      addFrame(frame);
                  }
              });
              
            }
            catch(InvalidProtocolBufferException ex)
            {
              Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        @Override
        public void errorOccured(String cause) {
            frameList.addListSelectionListener(behaviorFrameListener);
            Plugin.genericManagerFactory.getManager(getBehaviorStateSparse).removeListener(this);
            btReceiveLogData.setEnabled(true);
        }
  }
  
  

  class SymbolWatchCheckBoxListener implements ActionListener
  {
    private JCheckBox checkBox;
    private SortedSet<String> selectedSymbols;
    
    SymbolWatchCheckBoxListener(SortedSet<String> selectedSymbols, JCheckBox checkBox)
    {
      this.checkBox = checkBox;
      this.selectedSymbols = selectedSymbols;
      this.checkBox.setBackground(new Color(0.5f, 0.5f, 1.0f));
      this.checkBox.setOpaque(this.checkBox.isSelected());
    }

    @Override
    public void actionPerformed(ActionEvent e)
    {
      // HACK: checkBox.isSelected() is the old state of the box(!)
      String name = this.checkBox.getText();
      boolean value = this.checkBox.isSelected();

      if(value)
        this.selectedSymbols.add(name);
      else
        this.selectedSymbols.remove(name);
      
      this.checkBox.setOpaque(value);
    }//end actionPerformed
  }//end SymbolWatchCheckBoxListener
  
  private SortedSet<String> symbolsToWatch = new TreeSet<>();

  private void showFrame(XABSLBehaviorFrame frame)
  {
    if(frame != null)
    {
      StringBuilder watchBuffer = new StringBuilder();
      
      // input and output symbols
      StringBuffer inputBuffer = new StringBuffer();
      StringBuffer outputBuffer = new StringBuffer();

      for(String name: symbolsToWatch)
      {
                Symbol symbol = frame.getSymbolByName(name);
        // TODO: error treatment
        if(symbol == null) return;

        // remove the leading 
        String data_value = symbol.getValueAsString();

        // cut the leading enum type
        if(symbol instanceof Symbol.Enum)
        {
          data_value = data_value.replace(name+".", "");
        }
        
        XABSLBehaviorFrame.SymbolIOType type = frame.getSymbolIOType(name);
        
        if(type == XABSLBehaviorFrame.SymbolIOType.input)
        {
          inputBuffer.append("> ");
          inputBuffer.append(name)
                  .append(" = ")
                  .append(data_value)
                  .append(" (")
                  .append(symbol.getDataType())
                  .append(")")
                  .append("\n");
                   
        }
        else if(type == XABSLBehaviorFrame.SymbolIOType.output)
        {
          outputBuffer.append("< ");
          outputBuffer.append(name)
                  .append(" = ")
                  .append(data_value)
                  .append(" (")
                  .append(symbol.getDataType())
                  .append(")")
                  .append("\n");
        }
          
      }//end for
      
      if(inputBuffer.length() > 0)
      {
        watchBuffer.append("-- input symbols --\n");
        watchBuffer.append(inputBuffer).append("\n");
      }

      if(outputBuffer.length() > 0)
      {
        watchBuffer.append("-- output symbols --\n");
        watchBuffer.append(outputBuffer).append("\n");
      }

      this.symbolsWatchTextPanel.setText(watchBuffer.toString());

      // options
      this.behaviorTreePanel.setFrame(frame, currentBehavior);
      
      // some global visualizations
      //drawFrameOnFieldGlobal(frame);
      
    }//end if
  }//end showFrame

 private void drawStuff(XABSLBehaviorFrame frame)
 {
     try
    {
     double robot_x = Double.parseDouble(getSymbolValue(frame, "robot_pose.x"));
     double robot_y = Double.parseDouble(getSymbolValue(frame, "robot_pose.y"));
     double robot_r = Double.parseDouble(getSymbolValue(frame, "robot_pose.rotation"));
     
     Robot robot = new Robot(robot_x, robot_y, robot_r/180.0*Math.PI);
     
     Plugin.drawingEventManager.fireDrawingEvent(robot);
    }catch(Exception ex)
    {
        Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
    }
 }
  
  private void drawFrameOnFieldGlobal(XABSLBehaviorFrame frame)
  {
    try
    {
      StringBuilder msg = new StringBuilder();

      msg.append("DrawingOnField\n");
    
      double robot_x = Double.parseDouble(getSymbolValue(frame, "robot_pose.x"));
      double robot_y = Double.parseDouble(getSymbolValue(frame, "robot_pose.y"));
      double robot_r = Double.parseDouble(getSymbolValue(frame, "robot_pose.rotation"));

      // robot
      String team_color = "";//getSymbolValue(frame, "game.own_team_color");
      if(team_color.equals("game.team_color.blue"))
        msg.append("Pen:0000FF:10\n");
      else if(team_color.equals("game.team_color.red"))
        msg.append("Pen:FF0000:10\n");
      else
        msg.append("Pen:999999:10\n");

      msg.append("Robot:")
       .append(robot_x)
       .append(":")
       .append(robot_y)
       .append(":")
       .append(robot_r/180.0*Math.PI)
       .append("\n");

      // ball
      msg.append("Pen:AA6600:1\n");
      msg.append("FillOval:")
       .append(getSymbolValue(frame, "ball.position.field.x"))
       .append(":")
       .append(getSymbolValue(frame, "ball.position.field.y"))
       .append(":")
       .append(getSymbolValue(frame, "ball.radius"))
       .append(":")
       .append(getSymbolValue(frame, "ball.radius"))
       .append("\n");


      // motion request
      msg.append("Pen:000000:10\n");
      String motion_type = getSymbolValue(frame, "motion.type").replace("motion.type.", "");
      String player_number = getSymbolValue(frame, "game.player_number");
      msg.append("Text:")
       .append(robot_x + 150)
       .append(":")
       .append(robot_y + 150)
       .append(":")
       .append("(")
       .append(player_number)
       .append(") ")
       .append(motion_type)
       .append("\n");

      // walk request
      if(motion_type.equals("walk"))
      {
        double motion_x = Double.parseDouble(getSymbolValue(frame, "motion.walk_speed.x"));
        double motion_y = Double.parseDouble(getSymbolValue(frame, "motion.walk_speed.y"));
        msg.append("Arrow:")
           .append(robot_x)
           .append(":")
           .append(robot_y)
           .append(":")
           .append(motion_x)
           .append(":")
           .append(motion_y)
           .append("\n");
      }//end if

      //HACK: behavior viewer wants to use the field viewer to visualize data
      //Plugin.debugDrawingManager.handleResponse(msg.toString().getBytes(), null);
      //this.parent.getDebugDrawingManager().handleResponse(msg.toString().getBytes(), null);
      
    }catch(Exception ex)
    {
        Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
    }
  }//end drawFrameOnField

  
  private String getSymbolValue(XABSLBehaviorFrame frame, String name) throws Exception
  {
        Symbol s = frame.getSymbolByName(name);
    if(s == null)
    {
        throw new Exception("Symbol " + name + " is not existing");
    }
    return s.getValueAsString();
  }//end getDoubleSymbolValue


  
  
  public class BehaviorFrameListModel extends AbstractListModel 
  {
      public BehaviorFrameListModel() {
      }

      @Override
      public Object getElementAt(int index) {
        return behaviorBuffer.get(index).frameNumber;
      }

      @Override
      public int getSize() {
        return behaviorBuffer.size();
      }

      public void refresh()
      {
          fireContentsChanged(this, 0, getSize()-1);
      }
  }//end class BehaviorFrameListModel
  
  
  private void addFrame(XABSLBehaviorFrame status)
  {
    if(this.behaviorBuffer.size() >= maxNumberOfFrames)
    {
      this.behaviorBuffer.remove(0);
    }

    
    try{
      this.behaviorBuffer.add(status);
    }catch(Exception ex)
    {
      Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
      return;
    }
 
    //Select the new item and make it visible.
    BehaviorFrameListModel listModel = ((BehaviorFrameListModel) this.frameList.getModel());
    
    listModel.refresh();
    this.frameList.ensureIndexIsVisible(listModel.getSize()-1);
    this.frameList.setSelectedIndex(listModel.getSize()-1);
    //this.frameList.revalidate();
    
    drawStuff(status);
    showFrame(status);
    
  }//end addFrame

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        symbolChooser = new javax.swing.JDialog();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        jScrollPane1 = new javax.swing.JScrollPane();
        inputSymbolsBoxPanel = new javax.swing.JPanel();
        jScrollPane4 = new javax.swing.JScrollPane();
        outputSymbolsBoxPanel = new javax.swing.JPanel();
        sortSymbolsTextInput = new javax.swing.JTextField();
        jToolBar1 = new javax.swing.JToolBar();
        btReceive = new javax.swing.JToggleButton();
        btReceiveLogData = new javax.swing.JToggleButton();
        cbOnlyOptions = new javax.swing.JCheckBox();
        btSend = new javax.swing.JButton();
        cbAgents = new javax.swing.JComboBox();
        drawingPanel = new javax.swing.JPanel();
        jSplitPane = new javax.swing.JSplitPane();
        symbolsPanel = new javax.swing.JPanel();
        jScrollPane3 = new javax.swing.JScrollPane();
        symbolsWatchTextPanel = new javax.swing.JTextArea();
        jToolBar2 = new javax.swing.JToolBar();
        btAddWatch = new javax.swing.JButton();
        behaviorTreePanel = new de.naoth.rc.dialogs.behaviorviewer.BehaviorTreePanel();
        frameListPanel = new javax.swing.JScrollPane();
        frameList = new javax.swing.JList();

        symbolChooser.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        symbolChooser.setTitle("XABSL Symbols");
        symbolChooser.setAlwaysOnTop(true);
        symbolChooser.setLocationByPlatform(true);
        symbolChooser.setMinimumSize(new java.awt.Dimension(250, 350));

        jTabbedPane1.setBackground(new java.awt.Color(255, 255, 255));
        jTabbedPane1.setOpaque(true);

        jScrollPane1.setBackground(new java.awt.Color(255, 255, 255));

        inputSymbolsBoxPanel.setBackground(new java.awt.Color(255, 255, 255));
        inputSymbolsBoxPanel.setLayout(new javax.swing.BoxLayout(inputSymbolsBoxPanel, javax.swing.BoxLayout.Y_AXIS));
        jScrollPane1.setViewportView(inputSymbolsBoxPanel);

        jTabbedPane1.addTab("Input Symbols", jScrollPane1);

        outputSymbolsBoxPanel.setBackground(new java.awt.Color(255, 255, 255));
        outputSymbolsBoxPanel.setLayout(new javax.swing.BoxLayout(outputSymbolsBoxPanel, javax.swing.BoxLayout.Y_AXIS));
        jScrollPane4.setViewportView(outputSymbolsBoxPanel);

        jTabbedPane1.addTab("OutputSymbols", jScrollPane4);

        symbolChooser.getContentPane().add(jTabbedPane1, java.awt.BorderLayout.CENTER);

        sortSymbolsTextInput.addCaretListener(new javax.swing.event.CaretListener() {
            public void caretUpdate(javax.swing.event.CaretEvent evt) {
                sortSymbolsTextInputCaretUpdate(evt);
            }
        });
        symbolChooser.getContentPane().add(sortSymbolsTextInput, java.awt.BorderLayout.SOUTH);

        setLayout(new java.awt.BorderLayout());

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);
        jToolBar1.setBorderPainted(false);

        btReceive.setText("Receive");
        btReceive.setFocusable(false);
        btReceive.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReceive.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReceive.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceive);

        btReceiveLogData.setText("Receive Log");
        btReceiveLogData.setFocusable(false);
        btReceiveLogData.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReceiveLogData.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReceiveLogData.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveLogDataActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceiveLogData);

        cbOnlyOptions.setText("only options");
        cbOnlyOptions.setFocusable(false);
        cbOnlyOptions.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        jToolBar1.add(cbOnlyOptions);

        btSend.setText("Send to Robot");
        btSend.setFocusable(false);
        btSend.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btSend.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btSend.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btSendActionPerformed(evt);
            }
        });
        jToolBar1.add(btSend);

        cbAgents.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "no agents" }));
        cbAgents.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                cbAgentsMouseClicked(evt);
            }
        });
        cbAgents.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                cbAgentsItemStateChanged(evt);
            }
        });
        cbAgents.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbAgentsActionPerformed(evt);
            }
        });
        jToolBar1.add(cbAgents);

        add(jToolBar1, java.awt.BorderLayout.NORTH);

        drawingPanel.setBackground(new java.awt.Color(102, 102, 102));
        drawingPanel.setLayout(new java.awt.BorderLayout());

        jSplitPane.setBorder(javax.swing.BorderFactory.createLineBorder(jToolBar1.getBackground()));
        jSplitPane.setDividerLocation(500);
        jSplitPane.setDividerSize(10);
        jSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane.setResizeWeight(1.0);
        jSplitPane.setOneTouchExpandable(true);

        symbolsPanel.setLayout(new java.awt.BorderLayout());

        symbolsWatchTextPanel.setColumns(20);
        symbolsWatchTextPanel.setEditable(false);
        symbolsWatchTextPanel.setRows(5);
        jScrollPane3.setViewportView(symbolsWatchTextPanel);

        symbolsPanel.add(jScrollPane3, java.awt.BorderLayout.CENTER);

        jToolBar2.setFloatable(false);
        jToolBar2.setRollover(true);
        jToolBar2.setBorderPainted(false);

        btAddWatch.setText("Add Watch");
        btAddWatch.setFocusable(false);
        btAddWatch.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btAddWatch.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btAddWatch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btAddWatchActionPerformed(evt);
            }
        });
        jToolBar2.add(btAddWatch);

        symbolsPanel.add(jToolBar2, java.awt.BorderLayout.PAGE_END);

        jSplitPane.setRightComponent(symbolsPanel);
        jSplitPane.setLeftComponent(behaviorTreePanel);

        drawingPanel.add(jSplitPane, java.awt.BorderLayout.CENTER);

        frameListPanel.setBorder(javax.swing.BorderFactory.createLineBorder(jToolBar1.getBackground()));
        frameListPanel.setPreferredSize(new java.awt.Dimension(90, 132));

        frameList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        frameListPanel.setViewportView(frameList);

        drawingPanel.add(frameListPanel, java.awt.BorderLayout.WEST);

        add(drawingPanel, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

  private void btReceiveActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btReceiveActionPerformed
  {//GEN-HEADEREND:event_btReceiveActionPerformed
    if(btReceive.isSelected())
    {
      if(Plugin.parent.checkConnected())
      {
        sendCommand(getListOfAgents);
        sendCommand(getAgentCommand);
        //Plugin.genericManagerFactory.getManager(getExecutedBehaviorCommand).addListener(this);
        
        // TEST (Heinrich)
        Plugin.commandExecutor.executeCommand(this.behaviorListener, getBehaviorStateComplete);
        Plugin.genericManagerFactory.getManager(getBehaviorStateSparse).addListener(this.behaviorUpdateListener);
        
        // make the list of frames not clickable
        this.frameList.removeListSelectionListener(behaviorFrameListener);
        btReceiveLogData.setEnabled(false);
      }
      else
      {
        btReceive.setSelected(false);
        btReceiveLogData.setEnabled(true);
      }
    }
    else
    {
      this.frameList.addListSelectionListener(behaviorFrameListener);
      //Plugin.genericManagerFactory.getManager(getExecutedBehaviorCommand).removeListener(this);
      Plugin.genericManagerFactory.getManager(getBehaviorStateSparse).removeListener(this.behaviorUpdateListener);
      btReceiveLogData.setEnabled(true);
    }

}//GEN-LAST:event_btReceiveActionPerformed

    private void btSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btSendActionPerformed
      String behavior = Plugin.parent.getConfig().getProperty(behaviorConfKey, defaultBehavior);
      JFileChooser fileChooser = new JFileChooser(behavior);
      fileChooser.setFileFilter(new ICDATFileFilter());
      fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
      int result = fileChooser.showOpenDialog(this);

      if(result == JFileChooser.APPROVE_OPTION)
      {
        File file = fileChooser.getSelectedFile();
        try
        {
          FileReader fileReader = new FileReader(file);
          StringBuilder buffer = new StringBuilder();

          int c = fileReader.read();
          while(c != -1)
          {
            buffer.append((char) c);
            c = fileReader.read();
          }//end while
          fileReader.close();

          Command command = new Command(fileWriteCommandName)
                  .addArg("path", behaviorPath)
                  .addArg("content", buffer.toString());
          sendCommand(command);
          behavior = file.getPath();
          Plugin.parent.getConfig().setProperty(behaviorConfKey, behavior);
        }
        catch(IOException ex)
        {
          JOptionPane.showMessageDialog(this,
            "The behavior couldn't be sent.", "Sending Behavior", JOptionPane.ERROR_MESSAGE);
        }
      }//end if
    }//GEN-LAST:event_btSendActionPerformed

    private void btAddWatchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btAddWatchActionPerformed
      //TODO: don't delete every time
      this.inputSymbolsBoxPanel.removeAll();
      this.outputSymbolsBoxPanel.removeAll();

      
      if(currentBehavior != null)
      {
        for(Symbol s : currentBehavior.inputSymbols.values())
        {
            JCheckBox checkBox = new JCheckBox(s.name);
            checkBox.setSelected(this.symbolsToWatch.contains(s.name));
            checkBox.setOpaque(false);
            checkBox.setActionCommand("");
            checkBox.addActionListener(new SymbolWatchCheckBoxListener(this.symbolsToWatch, checkBox));

            this.inputSymbolsBoxPanel.add(checkBox);
        }

        for(Symbol s : currentBehavior.outputSymbols.values())
        {
            JCheckBox checkBox = new JCheckBox(s.name);
            checkBox.setSelected(this.symbolsToWatch.contains(s.name));
            checkBox.setOpaque(false);
            checkBox.setActionCommand("");
            checkBox.addActionListener(new SymbolWatchCheckBoxListener(this.symbolsToWatch, checkBox));

            this.outputSymbolsBoxPanel.add(checkBox);
        }
      }
    
      sortSymbols(this.sortSymbolsTextInput.getText());
      this.symbolChooser.setVisible(true);
    }//GEN-LAST:event_btAddWatchActionPerformed

    private void sortSymbolsTextInputCaretUpdate(javax.swing.event.CaretEvent evt) {//GEN-FIRST:event_sortSymbolsTextInputCaretUpdate
      sortSymbols(this.sortSymbolsTextInput.getText());
    }//GEN-LAST:event_sortSymbolsTextInputCaretUpdate

    private void cbAgentsItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbAgentsItemStateChanged
      if(!vetoSetAgent &&
              evt.getStateChange() == java.awt.event.ItemEvent.SELECTED
              && evt.getItem() != null)
      {
        String selectedAgent = evt.getItem().toString(); //cbAgents.getSelectedItem().toString();

        //if(selectedAgent.equals(currentAgent)) return;
        //System.out.println(selectedAgent);

        Command command = new Command(setAgentCommand)
                .addArg(setAgentCommandParam, selectedAgent.toString());
        sendCommand(command);
      }//end if
    }//GEN-LAST:event_cbAgentsItemStateChanged

    private void cbAgentsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbAgentsActionPerformed
      
    }//GEN-LAST:event_cbAgentsActionPerformed

    private void cbAgentsMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_cbAgentsMouseClicked
      
    }//GEN-LAST:event_cbAgentsMouseClicked

    private void btReceiveLogDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveLogDataActionPerformed
        if(btReceiveLogData.isSelected())
        {
            //frameListPanel.setVisible(false);
            btReceive.setEnabled(false);
            Plugin.logFileEventManager.addListener(logBehaviorListener);
        }
        else
        {
            //frameListPanel.setVisible(true);
            btReceive.setEnabled(true);
            Plugin.logFileEventManager.removeListener(logBehaviorListener);
        }
    }//GEN-LAST:event_btReceiveLogDataActionPerformed

  private class LogBehaviorListener implements LogFrameListener
  {
    @Override
    public void newFrame(BlackBoard b, int frameNumber) {
        
        try
        {
          LogDataFrame f = b.get("BehaviorStateComplete");
          Messages.BehaviorStateComplete status = Messages.BehaviorStateComplete.parseFrom(f.getData());
          
          behaviorParser = new XABSLProtoParser();
          currentBehavior = behaviorParser.parse(status);
        }
        catch(InvalidProtocolBufferException ex)
        {
          Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        try
        {
          LogDataFrame f = b.get("BehaviorStateSparse");
          Messages.BehaviorStateSparse status = Messages.BehaviorStateSparse.parseFrom(f.getData());
          final XABSLBehaviorFrame frame = behaviorParser.parse(status);
          addFrame(frame);
        }
        catch(InvalidProtocolBufferException ex)
        {
          Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
  }

  //TODO: check if the agent name is avaliable
  private void setAgent(String name)
  {
    this.vetoSetAgent = true;
    DefaultComboBoxModel model = ((DefaultComboBoxModel)cbAgents.getModel());
    model.setSelectedItem(name);
    this.vetoSetAgent = false;
  }//end setAgent

  private void setAgentList(String[] agents)
  {
    this.vetoSetAgent = true;
    DefaultComboBoxModel model = new DefaultComboBoxModel();
    
    if(agents.length == 0)
      model.addElement("no agents");
    else
    {
      for(int i = 0; i < agents.length; i++)
      {
        model.addElement(agents[i]);
      }
    }

    cbAgents.setModel(model);
    this.vetoSetAgent = false;
  }//end setAgentList
    
  private void sortSymbols(String searchString)
  {
    Component[] components = this.inputSymbolsBoxPanel.getComponents();
    for(int i = 0; i < components.length; i++)
    {
      JCheckBox checkBox = (JCheckBox)components[i];
      checkBox.setVisible(checkSearchTextMatch(checkBox.getText(),searchString));
    }//end for

    components = this.outputSymbolsBoxPanel.getComponents();
    for(int i = 0; i < components.length; i++)
    {
      JCheckBox checkBox = (JCheckBox)components[i];
      checkBox.setVisible(checkSearchTextMatch(checkBox.getText(),searchString));
    }//end for
  }//end sortSymbols

  private boolean checkSearchTextMatch(String text, String match)
  {
    try
    {
      return  text.startsWith(match) ||
              text.matches(match);
    }
    catch(PatternSyntaxException ex){}
    
    return false;
  }//end checkSearchTextMatch

  private void sendCommand(final Command command)
  {
    if( !Plugin.parent.checkConnected() )
      return;

    Plugin.parent.getMessageServer().executeSingleCommand(new CommandSender()
    {
      @Override
      public void handleResponse(byte[] result, Command originalCommand)
      {
        handleCommandResponse(result, originalCommand);
      }

      @Override
      public void handleError(int code)
      {
        JOptionPane.showMessageDialog(BehaviorViewer.this,
          "Could not send or receive behavior command, code " + code, "ERROR", JOptionPane.ERROR_MESSAGE);
      }

      @Override
      public Command getCurrentCommand()
      {
        return command;
      }
    });
  }//end sendCommand

  
  private void handleCommandResponse(byte[] result, Command originalCommand)
  {
    if(originalCommand.getName().equals(fileWriteCommandName))
    {
      sendCommand(reloadBehaviorCommand);
      JOptionPane.showMessageDialog(this,
        new String(result), "Sending Behavior", JOptionPane.INFORMATION_MESSAGE);
    }
    else if(originalCommand.getName().equals(reloadBehaviorCommand.getName()))
    {
      // HACK: the positive ansver "behavior reloaded" is defined in XABSLBehaviorControl.cpp
      if(!("behavior reloaded".equals(new String(result))))
      {
          JOptionPane.showMessageDialog(this,
            new String(result), "Sending Behavior", JOptionPane.ERROR_MESSAGE);
      }
    }
    else if(originalCommand.getName().startsWith(getAgentCommand.getName()))
    {
      //txtAgent.setText(new String(result));
      setAgent(new String(result));
    }
    else if(originalCommand.getName().startsWith(setAgentCommand))
    {
      String answer = new String(result);
      if(!answer.startsWith("resetted agent to"))
      {
        JOptionPane.showMessageDialog(this,
          answer, "ERROR", JOptionPane.ERROR_MESSAGE);
      }
      sendCommand(getAgentCommand);
    }
    else if(originalCommand.getName().equals(getListOfAgents.getName()))
    {
      String answer = new String(result);
      setAgentList(answer.split("\n"));
    }
  }//end handleCommandResponse
  

  class BehaviorFrameListener implements ListSelectionListener
  {
    @Override
    public void valueChanged(ListSelectionEvent e)
    {
      if(!e.getValueIsAdjusting() && behaviorBuffer.size() > 0 && frameList.getSelectedIndex() >= 0)
      {
        showFrame(behaviorBuffer.get(frameList.getSelectedIndex()));
      }
    }
  }//end class BehaviorFrameListener

  private class ICDATFileFilter extends javax.swing.filechooser.FileFilter
  {

    @Override
    public boolean accept(File file)
    {
      if(file.isDirectory())
      {
        return true;
      }
      String filename = file.getName();
      return filename.toLowerCase().endsWith("-ic.dat");
    }

    @Override
    public String getDescription()
    {
      return "Behavior Intemediate Code (*-ic.dat)";
    }

    @Override
    public String toString()
    {
      return "-ic.dat";
    }
  }//end class ICDATFileFilter

  @Override
  public void dispose()
  {
    //Plugin.genericManagerFactory.getManager(getExecutedBehaviorCommand).removeListener(this);
    Plugin.genericManagerFactory.getManager(getBehaviorStateSparse).removeListener(this.behaviorUpdateListener);
  }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private de.naoth.rc.dialogs.behaviorviewer.BehaviorTreePanel behaviorTreePanel;
    private javax.swing.JButton btAddWatch;
    private javax.swing.JToggleButton btReceive;
    private javax.swing.JToggleButton btReceiveLogData;
    private javax.swing.JButton btSend;
    private javax.swing.JComboBox cbAgents;
    private javax.swing.JCheckBox cbOnlyOptions;
    private javax.swing.JPanel drawingPanel;
    private javax.swing.JList frameList;
    private javax.swing.JScrollPane frameListPanel;
    private javax.swing.JPanel inputSymbolsBoxPanel;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JSplitPane jSplitPane;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JToolBar jToolBar2;
    private javax.swing.JPanel outputSymbolsBoxPanel;
    private javax.swing.JTextField sortSymbolsTextInput;
    private javax.swing.JDialog symbolChooser;
    private javax.swing.JPanel symbolsPanel;
    private javax.swing.JTextArea symbolsWatchTextPanel;
    // End of variables declaration//GEN-END:variables
}
