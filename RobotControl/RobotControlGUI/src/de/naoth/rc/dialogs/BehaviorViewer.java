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
import de.naoth.rc.manager.DebugDrawingManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.messages.Messages;
import de.naoth.rc.messages.Messages.XABSLAction;
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
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
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
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTree;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.event.TreeExpansionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeCellRenderer;
import javax.swing.tree.TreePath;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */

public class BehaviorViewer extends AbstractDialog
  implements
  ObjectListener<byte[]>,
  Dialog,
  TreeExpansionListener
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
  }//end Plugin
  

  private final String fileWriteCommandName = "file::write";
  private final String behaviorPath = "Config/behavior-ic.dat";
  private final String setAgentCommand = "behavior:set_agent";
  private final String setAgentCommandParam = "agent";

  private final Command reloadBehaviorCommand = new Command("behavior:reload");
  private final Command getAgentCommand = new Command("behavior:get_agent");
  private final Command getExecutedBehaviorCommand = new Command("behavior:status");
  private final Command enableUpdateBehaviorStatusCommand =
          new Command("XABSL:update_status").addArg("on");
  private final Command disableUpdateBehaviorStatusCommand =
          new Command("XABSL:update_status").addArg("off");

  private final Command getListOfAgents = new Command("behavior:list_agents");

  ArrayList<XABSLFrame> behaviorBuffer;
  private HashMap<String, Boolean> actionExpanded;
  public static final Color DARK_GREEN = new Color(0, 128, 0);
  public static final Font PLAIN_FONT = new Font("Sans Serif", Font.PLAIN, 11);
  public static final Font BOLD_FONT = new Font("Sans Serif", Font.BOLD, 11);
  public static final Font ITALIC_FONT = new Font("Sans Serif", Font.ITALIC, 11);
  final private String behaviorConfKey = "behavior";
  final private String defaultBehavior = "../NaoController/Config/behavior/behavior-ic.dat";

  private XABSLFramePrototype framePrototype = null;

  private boolean vetoSetAgent = false;
  //private String currentAgent = "";

  // max size of the buffer (25fps ~ 20s)
  private int maxNumberOfFrames = 500;
  
  
  private BehaviorFrameListener behaviorFrameListener = new BehaviorFrameListener();

  /** Creates new form BehaviorViewer */
  public BehaviorViewer()
  {
    actionExpanded = new HashMap<String, Boolean>();

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


    this.behaviorBuffer = new ArrayList<XABSLFrame>();
    
    createNewTree(null);
  }


  private void createNewTree(DefaultMutableTreeNode root)
  {
    if(root == null)
    {
      root = new DefaultMutableTreeNode("Behavior");
    }

    DefaultTreeModel model = new DefaultTreeModel(root);

    JTree newTree = new JTree();
    newTree.setModel(model);
    
    // expand all by default
    for(int i=0; i < newTree.getRowCount(); i++)
    {
      newTree.expandRow(i);
    }
    // collapse all requested
    Enumeration e = root.depthFirstEnumeration();
    while(e.hasMoreElements())
    {
      Object o = e.nextElement();
      if(o instanceof DefaultMutableTreeNode)
      {
        DefaultMutableTreeNode n = (DefaultMutableTreeNode) o;
        if(n.getUserObject() instanceof Messages.XABSLAction)
        {
          Messages.XABSLAction a = (XABSLAction) n.getUserObject();
          if(Boolean.FALSE.equals(actionExpanded.get(a.getName())))
          {
            newTree.collapsePath(new TreePath(n.getPath()));
          }
        }
      }
    }//end while

    newTree.setVisible(true);
    newTree.addTreeExpansionListener(this);
    newTree.setDoubleBuffered(false);
    newTree.setCellRenderer(new XABSLTreeRenderer());

    //TODO: this are preparation for jumping to the sourse, when an option is clicked
    newTree.addTreeSelectionListener(new TreeSelectionListener() {
      @Override
      public void valueChanged(TreeSelectionEvent e) {
        Object[] path = e.getPath().getPath();

        // get the leafe
        Object userObject = ((DefaultMutableTreeNode)path[path.length-1]).getUserObject();
        if(userObject instanceof Messages.XABSLAction)
        {
          StringBuilder sb = new StringBuilder();
          sb.append(((Messages.XABSLAction)userObject).getName())
            .append(':')
            .append(((Messages.XABSLAction)userObject).getActiveState());
          System.out.println(sb.toString());
        }
      }
    });


    //scrollTree.setViewportView(newTree);
    this.scrollTreePanel.removeAll();
    this.scrollTreePanel.add(newTree);
    this.scrollTreePanel.validate();
  }//end createNewTree

  @Override
  public void errorOccured(String cause)
  {
    btReceiveExecutionPath.setSelected(false);
    sendCommand(disableUpdateBehaviorStatusCommand);
    Plugin.genericManagerFactory.getManager(getExecutedBehaviorCommand).removeListener(this);
    //parent.getGenericManager(getExecutedBehaviorCommand).removeListener(this);

    this.frameList.addListSelectionListener(behaviorFrameListener);
    
    JOptionPane.showMessageDialog(null,
      cause, "Error", JOptionPane.ERROR_MESSAGE);
  }//end errorOccured


  class SymbolComperator implements Comparator<Messages.XABSLParameter>
  {
    @Override
    public int compare(XABSLParameter o1, XABSLParameter o2) {
      return o1.getName().compareTo(o2.getName());
    }
  }//end SymbolComperator

  @Override
  public void newObjectReceived(byte[] object)
  {
    // don't accept empty messages
    if(object == null || object.length == 0)
      return;
    
    try
    {
      Messages.BehaviorStatus status = Messages.BehaviorStatus.parseFrom(object);

      //setAgent(status.getAgent());

      // disable selection
      if(frameList.getSelectedIndex() > -1)
      {
        frameList.clearSelection();
      }
      
      if(status.hasErrorMessage() && status.getErrorMessage().length() > 0)
      {
        errorOccured(status.getErrorMessage());
      }
      else
      {
        // show and add
        addFrame(status);
        showFrame(this.behaviorBuffer.get(this.behaviorBuffer.size()-1));
      }
    }
    catch(InvalidProtocolBufferException ex)
    {
      Logger.getLogger(BehaviorViewer.class.getName()).log(Level.SEVERE, null, ex);
    }

  }//end newObjectReceived

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
  
  private SortedSet<String> symbolsToWatch = new TreeSet<String>();

  private void showFrame(XABSLFrame frame)
  {
    if(frame != null)
    {
      StringBuilder watchBuffer = new StringBuilder();
      
      // input and output symbols
      StringBuffer inputBuffer = new StringBuffer();
      StringBuffer outputBuffer = new StringBuffer();

      for(String name: symbolsToWatch)
      {
        SymbolId id = this.framePrototype.symbolRegistry.get(name);
        // TODO: error treatment
        if(id == null) return;

        // remove the leading 
        String data_value = frame.getStringValue(id);

        // cut the leading enum type
        if(id.data_type == SymbolId.DataType.Enum)
          data_value = data_value.replace(name+".", "");

        if(id.io_type == SymbolId.IOType.input)
          inputBuffer.append("> ")
                   .append(name)
                   .append(" = ")
                   .append(data_value)
                   .append(" (")
                   .append(id.data_type.name())
                   .append(")")
                   .append("\n");
        else
          outputBuffer.append("< ")
                   .append(name)
                   .append(" = ")
                   .append(data_value)
                   .append(" (")
                   .append(id.data_type.name())
                   .append(")")
                   .append("\n");
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
      DefaultMutableTreeNode treeRoot = new DefaultMutableTreeNode("Behavior (" + frame.agent + ")");
      for(Messages.XABSLAction a : frame.activeActions)
      {
        treeRoot.add(actionToNode(a));
      }

      createNewTree(treeRoot);

      // some global visualizations
      drawFrameOnFieldGlobal(frame);
      
    }//end if
  }//end showFrame

  
  private void drawFrameOnFieldGlobal(XABSLFrame frame)
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

      Plugin.debugDrawingManager.handleResponse(msg.toString().getBytes(), null);
      //this.parent.getDebugDrawingManager().handleResponse(msg.toString().getBytes(), null);
      
    }catch(Exception ex)
    {
        ex.printStackTrace();
    }
  }//end drawFrameOnField

  
  private String getSymbolValue(XABSLFrame frame, String name) throws Exception
  {
    SymbolId id = this.framePrototype.symbolRegistry.get(name);
    if(id == null)
        throw new Exception("Symbol " + name + " is not existing");

    return frame.getStringValue(id);
  }//end getDoubleSymbolValue

  
  private static class SymbolId
  {
    public enum IOType{input, output}
    public enum DataType{Decimal, Boolean, Enum}

    public SymbolId(int idx, DataType data_type, IOType io_type) {
      this.idx = idx;
      this.data_type = data_type;
      this.io_type = io_type;
    }

    public final int idx;
    public final DataType data_type;
    public final IOType io_type;
  }//end class SymbolId

  
  private class XABSLFramePrototype
  {
    public int numberOfInputSymbolsDecimal = 0;
    public int numberOfInputSymbolsBoolean = 0;
    public int numberOfInputSymbolsEnum = 0;

    public int numberOfOutputSymbolsDecimal = 0;
    public int numberOfOutputSymbolsBoolean = 0;
    public int numberOfOutputSymbolsEnum = 0;

    HashMap<String, SymbolId> symbolRegistry = new HashMap<String, SymbolId>();
  }//end class XABSLFramePrototype

  
  private class XABSLFrame
  {
    public XABSLFrame(XABSLFramePrototype proto, Messages.BehaviorStatus status) throws Exception
    {
      this.inputSymbolValuesDecimal = new double[proto.numberOfInputSymbolsDecimal];
      this.inputSymbolValuesBoolean = new boolean[proto.numberOfInputSymbolsBoolean];
      this.inputSymbolValuesEnum = new String[proto.numberOfInputSymbolsEnum];

      this.outputSymbolValuesDecimal = new double[proto.numberOfOutputSymbolsDecimal];
      this.outputSymbolValuesBoolean = new boolean[proto.numberOfOutputSymbolsBoolean];
      this.outputSymbolValuesEnum = new String[proto.numberOfOutputSymbolsEnum];

      this.activeActions = new ArrayList<Messages.XABSLAction>();

      this.frameNumber = status.getFrameNumber();
      
      for(Messages.XABSLParameter p : status.getInputSymbolsList())
      {
        int idx = proto.symbolRegistry.get(p.getName()).idx;
        if(idx == -1) throw new Exception("wrong idx for symbol " + p.getName());

        if(p.getType() == Messages.XABSLParameter.ParamType.Decimal && p.hasDecimalValue())
          inputSymbolValuesDecimal[idx] = p.getDecimalValue();
        else if(p.getType() == Messages.XABSLParameter.ParamType.Boolean && p.hasBoolValue())
          inputSymbolValuesBoolean[idx] = p.getBoolValue();
        else if(p.getType() == Messages.XABSLParameter.ParamType.Enum && p.hasEnumValue())
          inputSymbolValuesEnum[idx] = p.getEnumValue();
      }//end for

      for(Messages.XABSLParameter p : status.getOutputSymbolsList())
      {
        int idx = proto.symbolRegistry.get(p.getName()).idx;
        if(idx == -1) throw new Exception("wrong idx for symbol " + p.getName());

        if(p.getType() == Messages.XABSLParameter.ParamType.Decimal && p.hasDecimalValue())
          outputSymbolValuesDecimal[idx] = p.getDecimalValue();
        else if(p.getType() == Messages.XABSLParameter.ParamType.Boolean && p.hasBoolValue())
          outputSymbolValuesBoolean[idx] = p.getBoolValue();
        else if(p.getType() == Messages.XABSLParameter.ParamType.Enum && p.hasEnumValue())
          outputSymbolValuesEnum[idx] = p.getEnumValue();
      }//end for

      for(Messages.XABSLAction a : status.getActiveRootActionsList())
      {
        this.activeActions.add(a);
      }

      this.agent = status.getAgent();
    }//end constructor
    
    public final double[] inputSymbolValuesDecimal;
    public final boolean[] inputSymbolValuesBoolean;
    public final String[] inputSymbolValuesEnum;

    public final double[] outputSymbolValuesDecimal;
    public final boolean[] outputSymbolValuesBoolean;
    public final String[] outputSymbolValuesEnum;

    public final ArrayList<Messages.XABSLAction> activeActions;
    public final String agent;

    public final int frameNumber;
    
    public String getStringValue(SymbolId id)
    {
      if(id.io_type == SymbolId.IOType.input)
      {
        switch(id.data_type)
        {
          case Decimal: return "" + inputSymbolValuesDecimal[id.idx];
          case Boolean: return "" + inputSymbolValuesBoolean[id.idx];
          case Enum: return "" + inputSymbolValuesEnum[id.idx];
          default: return "";
        }//end switch
      }
      else
      {
        switch(id.data_type)
        {
          case Decimal: return "" + outputSymbolValuesDecimal[id.idx];
          case Boolean: return "" + outputSymbolValuesBoolean[id.idx];
          case Enum: return "" + outputSymbolValuesEnum[id.idx];
          default: return "";
        }//end switch
      }
    }//end getStringValue
  }//end class XABSLFrame

  public DefaultMutableTreeNode actionToNode(Messages.XABSLAction a)
  {
    DefaultMutableTreeNode result = new DefaultMutableTreeNode(a);

    if(!cbOnlyOptions.isSelected())
    {
      // add parameters
      for(Messages.XABSLParameter p : a.getParametersList())
      {
        result.add(new DefaultMutableTreeNode(p));
      }
    }

    List<Messages.XABSLAction> subOptions = a.getActiveSubActionsList();
    if(subOptions.size() > 0)
    {

      // 1. add sub options
      for(Messages.XABSLAction sub : subOptions)
      {
        if(!cbOnlyOptions.isSelected() || sub.getType() == XABSLAction.ActionType.Option)
        {
          result.add(actionToNode(sub));
        }
      }
    }//end if

    return result;
  }//end actionToNode


  @Override
  public JPanel getPanel()
  {
    return this;
  }

  private XABSLFramePrototype createPrototype(Messages.BehaviorStatus status)
  {
    XABSLFramePrototype prototype = new XABSLFramePrototype();
    
    for(Messages.XABSLParameter p : status.getInputSymbolsList())
    {
      SymbolId id = null;
      if(p.getType() == Messages.XABSLParameter.ParamType.Decimal && p.hasDecimalValue())
      {
        id = new SymbolId(prototype.numberOfInputSymbolsDecimal++, SymbolId.DataType.Decimal, SymbolId.IOType.input);
      }
      else if(p.getType() == Messages.XABSLParameter.ParamType.Boolean && p.hasBoolValue())
      {
        id = new SymbolId(prototype.numberOfInputSymbolsBoolean++, SymbolId.DataType.Boolean, SymbolId.IOType.input);
      }
      else if(p.getType() == Messages.XABSLParameter.ParamType.Enum && p.hasEnumValue())
      {
        id = new SymbolId(prototype.numberOfInputSymbolsEnum++, SymbolId.DataType.Enum, SymbolId.IOType.input);
      }
      prototype.symbolRegistry.put(p.getName(), id);
    }//end for

    for(Messages.XABSLParameter p : status.getOutputSymbolsList())
    {
      SymbolId id = null;
      if(p.getType() == Messages.XABSLParameter.ParamType.Decimal && p.hasDecimalValue())
      {
        id = new SymbolId(prototype.numberOfOutputSymbolsDecimal++, SymbolId.DataType.Decimal, SymbolId.IOType.output);
      }
      else if(p.getType() == Messages.XABSLParameter.ParamType.Boolean && p.hasBoolValue())
      {
        id = new SymbolId(prototype.numberOfOutputSymbolsBoolean++, SymbolId.DataType.Boolean, SymbolId.IOType.output);
      }
      else if(p.getType() == Messages.XABSLParameter.ParamType.Enum && p.hasEnumValue())
      {
        id = new SymbolId(prototype.numberOfOutputSymbolsEnum++, SymbolId.DataType.Enum, SymbolId.IOType.output);
      }
      prototype.symbolRegistry.put(p.getName(), id);
    }//end for

    return prototype;
  }//end createPrototype

  
  
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
  
  
  
  
  private void addFrame(Messages.BehaviorStatus status)
  {
    //DefaultListModel listModel = ((DefaultListModel) this.frameList.getModel());
    
    if(this.behaviorBuffer.size() >= maxNumberOfFrames)
    {
      this.behaviorBuffer.remove(0);
      //listModel.remove(0);
    }

    // create the prototype
    if(framePrototype == null)
    {
      framePrototype = createPrototype(status);
    }//end if

    try{
      this.behaviorBuffer.add(new XABSLFrame(framePrototype, status));
    }catch(Exception e)
    {
      e.printStackTrace();
      return;
    }
   /*
    listModel.addElement(new Integer(status.getFrameNumber()));
    this.frameList.setSelectedIndex(listModel.getSize()-1);
    this.frameList.ensureIndexIsVisible(listModel.getSize()-1);
    */
    
    //Select the new item and make it visible.
    BehaviorFrameListModel listModel = ((BehaviorFrameListModel) this.frameList.getModel());
    //this.frameList.setSelectedIndex(0);
    
    
    listModel.refresh();
    this.frameList.ensureIndexIsVisible(listModel.getSize()-1);
    this.frameList.setSelectedIndex(listModel.getSize()-1);
    //this.frameList.revalidate();
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
    btReceiveExecutionPath = new javax.swing.JToggleButton();
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
    scrollTree = new javax.swing.JScrollPane();
    scrollTreePanel = new javax.swing.JPanel();
    jScrollPane2 = new javax.swing.JScrollPane();
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

    jToolBar1.setRollover(true);
    jToolBar1.setBorderPainted(false);

    btReceiveExecutionPath.setText("Receive Execution Path");
    btReceiveExecutionPath.setFocusable(false);
    btReceiveExecutionPath.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    btReceiveExecutionPath.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    btReceiveExecutionPath.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        btReceiveExecutionPathActionPerformed(evt);
      }
    });
    jToolBar1.add(btReceiveExecutionPath);

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

    jSplitPane.setDividerLocation(500);
    jSplitPane.setDividerSize(10);
    jSplitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
    jSplitPane.setResizeWeight(1.0);
    jSplitPane.setOneTouchExpandable(true);

    symbolsWatchTextPanel.setColumns(20);
    symbolsWatchTextPanel.setRows(5);
    jScrollPane3.setViewportView(symbolsWatchTextPanel);

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

    javax.swing.GroupLayout symbolsPanelLayout = new javax.swing.GroupLayout(symbolsPanel);
    symbolsPanel.setLayout(symbolsPanelLayout);
    symbolsPanelLayout.setHorizontalGroup(
      symbolsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 308, Short.MAX_VALUE)
      .addComponent(jToolBar2, javax.swing.GroupLayout.DEFAULT_SIZE, 308, Short.MAX_VALUE)
    );
    symbolsPanelLayout.setVerticalGroup(
      symbolsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, symbolsPanelLayout.createSequentialGroup()
        .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE)
        .addGap(0, 0, 0)
        .addComponent(jToolBar2, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE))
    );

    jSplitPane.setRightComponent(symbolsPanel);

    scrollTree.setDoubleBuffered(true);

    scrollTreePanel.setLayout(new java.awt.BorderLayout());
    scrollTree.setViewportView(scrollTreePanel);

    jSplitPane.setLeftComponent(scrollTree);

    drawingPanel.add(jSplitPane, java.awt.BorderLayout.CENTER);

    jScrollPane2.setPreferredSize(new java.awt.Dimension(90, 132));

    frameList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
    jScrollPane2.setViewportView(frameList);

    drawingPanel.add(jScrollPane2, java.awt.BorderLayout.WEST);

    add(drawingPanel, java.awt.BorderLayout.CENTER);
  }// </editor-fold>//GEN-END:initComponents

  private void btReceiveExecutionPathActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btReceiveExecutionPathActionPerformed
  {//GEN-HEADEREND:event_btReceiveExecutionPathActionPerformed
    if(btReceiveExecutionPath.isSelected())
    {
      if(Plugin.parent.checkConnected())
      {
        sendCommand(enableUpdateBehaviorStatusCommand);
        sendCommand(getListOfAgents);
        sendCommand(getAgentCommand);
        Plugin.genericManagerFactory.getManager(getExecutedBehaviorCommand).addListener(this);
        //parent.getGenericManager(getExecutedBehaviorCommand).addListener(this);
        this.frameList.removeListSelectionListener(behaviorFrameListener);
      }
      else
      {
        btReceiveExecutionPath.setSelected(false);
      }
    }
    else
    {
      this.frameList.addListSelectionListener(behaviorFrameListener);
      sendCommand(disableUpdateBehaviorStatusCommand);
      Plugin.genericManagerFactory.getManager(getExecutedBehaviorCommand).removeListener(this);
      //parent.getGenericManager(getExecutedBehaviorCommand).removeListener(this);
    }

}//GEN-LAST:event_btReceiveExecutionPathActionPerformed

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

      if(this.framePrototype == null || this.framePrototype.symbolRegistry == null)
          return;
      
      for(String name: this.framePrototype.symbolRegistry.keySet())
      {
        JCheckBox checkBox = new JCheckBox(name);
        checkBox.setSelected(this.symbolsToWatch.contains(name));
        checkBox.setOpaque(false);
        checkBox.setActionCommand("");
        checkBox.addActionListener(new SymbolWatchCheckBoxListener(this.symbolsToWatch, checkBox));

        SymbolId id = this.framePrototype.symbolRegistry.get(name);
        if(id.io_type == SymbolId.IOType.input)
          this.inputSymbolsBoxPanel.add(checkBox);
        else
          this.outputSymbolsBoxPanel.add(checkBox);
      }//end for

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

  private void sendCommand(Command command)
  {
    if( !Plugin.parent.checkConnected() )
      return;

    final Command commandToExecute = command;
    final BehaviorViewer thisFinal = this;

    Plugin.parent.getMessageServer().executeSingleCommand(new CommandSender()
    {

      @Override
      public void handleResponse(byte[] result, Command originalCommand)
      {
        if(originalCommand.getName().equals(fileWriteCommandName))
        {
          sendCommand(reloadBehaviorCommand);
          JOptionPane.showMessageDialog(thisFinal,
            new String(result), "Sending Behavior", JOptionPane.INFORMATION_MESSAGE);
        }
        else if(originalCommand.getName().equals(reloadBehaviorCommand.getName()))
        {
          // HACK: the positive ansver "behavior reloaded" is defined in XABSLBehaviorControl.cpp
          if(!("behavior reloaded".equals(new String(result))))
          {
              JOptionPane.showMessageDialog(thisFinal,
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
            JOptionPane.showMessageDialog(thisFinal,
              answer, "ERROR", JOptionPane.ERROR_MESSAGE);
          }
          sendCommand(getAgentCommand);
        }
        else if(originalCommand.getName().equals(getListOfAgents.getName()))
        {
          String answer = new String(result);
          setAgentList(answer.split("\n"));
        }
        //end if
      }//end handleResponse

      @Override
      public void handleError(int code)
      {
        JOptionPane.showMessageDialog(thisFinal,
          "Could not send or receive behavior command, code " + code, "ERROR", JOptionPane.ERROR_MESSAGE);
      }

      @Override
      public Command getCurrentCommand()
      {
        return commandToExecute;
      }
    }, command);
  }//end sendCommand

  @Override
  public void treeExpanded(TreeExpansionEvent event)
  {
    if(event.getPath().getLastPathComponent() instanceof DefaultMutableTreeNode)
    {
      DefaultMutableTreeNode n = (DefaultMutableTreeNode) event.getPath().getLastPathComponent();
      if(n.getUserObject() instanceof Messages.XABSLAction)
      {
        actionExpanded.put(((Messages.XABSLAction) n.getUserObject()).getName(),
          Boolean.TRUE);
      }
    }
  }//end treeExpanded

  @Override
  public void treeCollapsed(TreeExpansionEvent event)
  {
    if(event.getPath().getLastPathComponent() instanceof DefaultMutableTreeNode)
    {
      DefaultMutableTreeNode n = (DefaultMutableTreeNode) event.getPath().getLastPathComponent();
      if(n.getUserObject() instanceof Messages.XABSLAction)
      {
        actionExpanded.put(((Messages.XABSLAction) n.getUserObject()).getName(),
          Boolean.FALSE);
      }
    }
  }//end treeCollapsed

  class BehaviorFrameListener implements ListSelectionListener
  {
    @Override
    public void valueChanged(ListSelectionEvent e)
    {
      if(!e.getValueIsAdjusting() && behaviorBuffer.size() > 0 && frameList.getSelectedIndex() >= 0)
      {
        showFrame(behaviorBuffer.get(frameList.getSelectedIndex()));
      }//end if
    }//end valueChanged
  }//end class BehaviorFrameListener

  class XABSLTreeRenderer implements TreeCellRenderer
  {

    @Override
    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus)
    {
      if(value != null)
      {
        DefaultMutableTreeNode n = (DefaultMutableTreeNode) value;

        // default values for color and font
        Color color = Color.black;
        Font font = PLAIN_FONT;
        StringBuilder text = new StringBuilder();

        if(n.getUserObject() instanceof Messages.XABSLAction)
        {
          Messages.XABSLAction a = (Messages.XABSLAction) n.getUserObject();


          if(a.getType() == Messages.XABSLAction.ActionType.Option)
          {
            color = DARK_GREEN;
            font = BOLD_FONT;
            text.append(a.getName());
            text.append(" [");
            text.append(a.getTimeOfExecution());
            text.append(" ms] - ");
            text.append(a.getActiveState());
            text.append(" [");
            text.append(a.getStateTime());
            text.append(" ms]");
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.BasicBehavior)
          {
            text.append(a.getName());
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.BooleanOutputSymbol)
          {
            text.append(a.getName());
            text.append("=");
            text.append(a.getBoolValue());
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.DecimalOutputSymbol)
          {
            text.append(a.getName());
            text.append("=");
            text.append(a.getDecimalValue());
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.EnumOutputSymbol)
          {
            text.append(a.getName());
            text.append("=");
            String tmp_value = a.getEnumValue().replace(a.getName()+".", "");
            text.append(tmp_value);
          }
          
        }
        else if(n.getUserObject() instanceof Messages.XABSLParameter)
        {
          Messages.XABSLParameter p = (XABSLParameter) n.getUserObject();
          color = Color.DARK_GRAY;
          font = ITALIC_FONT;

          text.append("@");
          text.append(p.getName());
          if(p.getType() == XABSLParameter.ParamType.Boolean)
          {
            text.append("=");
            text.append(p.getBoolValue());
          }
          else if(p.getType() == XABSLParameter.ParamType.Decimal)
          {
            text.append("=");
            text.append(p.getDecimalValue());
          }
          if(p.getType() == XABSLParameter.ParamType.Enum)
          {
            text.append("=");
            String tmp_value = p.getEnumValue().replace(p.getName()+".", "");
            text.append(tmp_value);
          }
        }
        else
        {
          text.append(n.getUserObject().toString());
        }

        JLabel label = new JLabel(text.toString());
        label.setForeground(color);
        label.setFont(font);
        return label;
      }
      else
      {
        return new JLabel("null");
      }

    }
  }//end class XABSLTreeRenderer

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
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }//end dispose
    
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton btAddWatch;
  private javax.swing.JToggleButton btReceiveExecutionPath;
  private javax.swing.JButton btSend;
  private javax.swing.JComboBox cbAgents;
  private javax.swing.JCheckBox cbOnlyOptions;
  private javax.swing.JPanel drawingPanel;
  private javax.swing.JList frameList;
  private javax.swing.JPanel inputSymbolsBoxPanel;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JScrollPane jScrollPane2;
  private javax.swing.JScrollPane jScrollPane3;
  private javax.swing.JScrollPane jScrollPane4;
  private javax.swing.JSplitPane jSplitPane;
  private javax.swing.JTabbedPane jTabbedPane1;
  private javax.swing.JToolBar jToolBar1;
  private javax.swing.JToolBar jToolBar2;
  private javax.swing.JPanel outputSymbolsBoxPanel;
  private javax.swing.JScrollPane scrollTree;
  private javax.swing.JPanel scrollTreePanel;
  private javax.swing.JTextField sortSymbolsTextInput;
  private javax.swing.JDialog symbolChooser;
  private javax.swing.JPanel symbolsPanel;
  private javax.swing.JTextArea symbolsWatchTextPanel;
  // End of variables declaration//GEN-END:variables
}
