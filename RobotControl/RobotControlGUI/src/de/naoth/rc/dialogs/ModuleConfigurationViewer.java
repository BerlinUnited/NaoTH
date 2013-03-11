/*
 * 
 */

/*
 * ModuleConfigurationViewer.java
 *
 * Created on 02.12.2008, 19:09:17
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.AbstractDialog;
import de.naoth.rc.RobotControl;
import de.naoth.rc.checkboxtree.SelectableTreeNode;
import de.naoth.rc.dataformats.ModuleConfiguration;
import de.naoth.rc.dataformats.ModuleConfiguration.Node;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ModuleConfigurationManager;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Set;
import java.util.TreeSet;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JOptionPane;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class ModuleConfigurationViewer extends AbstractDialog
  implements ObjectListener<ModuleConfiguration>
{

  @InjectPlugin
  public RobotControl parent;
  @InjectPlugin
  public ModuleConfigurationManager moduleConfigurationManager;
  @InjectPlugin
  public GenericManagerFactory genericManagerFactory;
  
  //private VisualizationViewer<Node, Edge> vv;

  private final String commandStringStoreModules = "modules:store";
  private final String commandStringSetModules = "modules:set";
  
  
  SimpleModuleView simpleModuleView = new SimpleModuleView();
  ArrayList<String> activeModulesList = new ArrayList<String>(); 
  Set<String> representationsSet = new TreeSet<String>(); 
  ArrayList<JButton> activeButtonList = new ArrayList<JButton>();
  
  
  /** Creates new form ModuleConfigurationViewer */
  public ModuleConfigurationViewer()
  {
    initComponents();
    
    this.simpleModuleView.setLayout(null);
    this.jScrollPaneModulePane.setViewportView(simpleModuleView);
  }
  
  @Init
  @Override
  public void init()
  {
      moduleConfigurationManager.setModuleOwner((String)cbProcess.getSelectedItem());
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        fileChooser = new de.naoth.rc.dialogs.panels.ExtendedFileChooser();
        jToolBar1 = new javax.swing.JToolBar();
        jToggleButtonRefresh = new javax.swing.JToggleButton();
        btExport = new javax.swing.JButton();
        btSave = new javax.swing.JButton();
        btSend = new javax.swing.JButton();
        cbProcess = new javax.swing.JComboBox();
        jSplitPane1 = new javax.swing.JSplitPane();
        moduleTree = new javax.swing.JPanel();
        jScrollPaneModulePane = new javax.swing.JScrollPane();
        jScrollPane = new javax.swing.JScrollPane();
        moduleConfigTree = new de.naoth.rc.checkboxtree.CheckboxTree();

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        jToggleButtonRefresh.setText("Refresh");
        jToggleButtonRefresh.setFocusable(false);
        jToggleButtonRefresh.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jToggleButtonRefresh.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToggleButtonRefresh.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButtonRefreshActionPerformed(evt);
            }
        });
        jToolBar1.add(jToggleButtonRefresh);

        btExport.setText("Export");
        btExport.setToolTipText("export modules graph");
        btExport.setFocusable(false);
        btExport.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btExport.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btExport.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btExportActionPerformed(evt);
            }
        });
        jToolBar1.add(btExport);

        btSave.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Save24.gif"))); // NOI18N
        btSave.setToolTipText("save the module configuration localy");
        btSave.setFocusable(false);
        btSave.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btSave.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btSaveActionPerformed(evt);
            }
        });
        jToolBar1.add(btSave);

        btSend.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Export24.gif"))); // NOI18N
        btSend.setToolTipText("save the module configuration on the robot");
        btSend.setFocusable(false);
        btSend.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btSend.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btSend.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btSendActionPerformed(evt);
            }
        });
        jToolBar1.add(btSend);

        cbProcess.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Cognition", "Motion" }));
        cbProcess.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbProcessActionPerformed(evt);
            }
        });
        jToolBar1.add(cbProcess);

        jSplitPane1.setResizeWeight(1.0);

        moduleTree.setOpaque(false);
        moduleTree.setLayout(new java.awt.BorderLayout());

        jScrollPaneModulePane.setBorder(null);
        moduleTree.add(jScrollPaneModulePane, java.awt.BorderLayout.CENTER);

        jSplitPane1.setLeftComponent(moduleTree);

        jScrollPane.setBorder(null);
        jScrollPane.setPreferredSize(new java.awt.Dimension(200, 322));
        jScrollPane.setViewportView(moduleConfigTree);

        jSplitPane1.setRightComponent(jScrollPane);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 377, Short.MAX_VALUE)
            .addComponent(jSplitPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 377, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 302, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jToggleButtonRefreshActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButtonRefreshActionPerformed
      if(parent.checkConnected()) {
        if(jToggleButtonRefresh.isSelected()) {
          moduleConfigurationManager.addListener(this);
        } else {
          moduleConfigurationManager.removeListener(this);
        }
      } else {
        jToggleButtonRefresh.setSelected(false);
      }
}//GEN-LAST:event_jToggleButtonRefreshActionPerformed

    private void btExportActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btExportActionPerformed
      //ExportDialog export = new ExportDialog();
      //export.showExportDialog(this, "Export view as ...", this.vv, "export");
    }//GEN-LAST:event_btExportActionPerformed

    private void btSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btSaveActionPerformed
      fileChooser.showSaveDialog(this);
      File selectedFile = fileChooser.getSelectedFile();

      if(selectedFile == null) {
        return;
      }
      
      /*
      try
      {
        FileWriter fileWriter = new FileWriter(selectedFile, false);
        fileWriter.write("[modules]\n");
        
        for(Component comp: moduleConfigPanel.getComponents())
        {
          JCheckBox box = (JCheckBox)comp;
          String str = box.getText() + "=" + (box.isSelected()?"true":"false");
          fileWriter.write(str + "\n");
          //System.out.println(str);
        }//end for
        
        //fileWriter.write(buffer, 0, buffer.length);
        fileWriter.close();
      }
      catch(IOException e)
      {
        JOptionPane.showMessageDialog(this,
          e.toString(), "The file could not be written.", JOptionPane.ERROR_MESSAGE);
      }//end catch
       * 
       */
    }//GEN-LAST:event_btSaveActionPerformed

    private void btSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btSendActionPerformed
      sendCommand(new Command((String)cbProcess.getSelectedItem()+":"+commandStringStoreModules));
    }//GEN-LAST:event_btSendActionPerformed

    private void cbProcessActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbProcessActionPerformed
    {//GEN-HEADEREND:event_cbProcessActionPerformed
        moduleConfigurationManager.setModuleOwner((String)cbProcess.getSelectedItem());
        moduleConfigTree.clear();
    }//GEN-LAST:event_cbProcessActionPerformed

  @Override
  public JPanel getPanel()
  {
    return this;
  }//end getPanel

  @Override
  public void errorOccured(String cause)
  {
    moduleConfigurationManager.removeListener(this);
  }//end errorOccured

  @Override
  public void newObjectReceived(final ModuleConfiguration graph)
  {
    moduleConfigurationManager.removeListener(this);
    this.jToggleButtonRefresh.setSelected(false);
    String processName = (String)cbProcess.getSelectedItem();

    activeModulesList.clear();
    this.simpleModuleView.removeAll();
    representationsSet.clear();
    
    int height = 30;
    int offset_y = 10;
    
    int pos = 50;
    int posRep = 50;
    
    // update list and remove unused modules from graph
    ArrayList<Node> nodes = graph.getNodeList();
    for(ModuleConfiguration.Node n : nodes)
    {
      if(n.getType() == ModuleConfiguration.NodeType.Module)
      {
        String name = n.getName();
        
        // treatement for the modules which are located outside of the process
        if(!name.startsWith(processName.toLowerCase())) {
            int i = name.lastIndexOf(':');
            name = processName.toLowerCase() + name.substring(i);
        }
        
        SelectableTreeNode node = moduleConfigTree.insertPath(name, ':');
        node.setSelected(n.isEnabled());
        node.setTooltip(n.getName());
        node.getComponent().addActionListener(
                new ModuleCheckBoxListener(node.getComponent(), 
                    processName + ":" + commandStringSetModules));
        
        if(n.isEnabled())
        {
            activeModulesList.add(n.getName());
            JButton b = new JButton(n.getName());
            b.setSize(150, height);
            b.setLocation(40, pos);
            activeButtonList.add(b);
            this.simpleModuleView.add(b);
            pos += height+offset_y;
            
        }
        /*
        if(!isActive)
        {
          graph.removeVertex(n);
        }*/

      }
      else
      {
            JButton b = new JButton(n.getName());
            b.setSize(150, height);
            b.setLocation(300, posRep);
            activeButtonList.add(b);
            this.simpleModuleView.add(b);
            posRep += height+offset_y;
      }
    }//end for

    moduleConfigTree.expandPath(processName.toLowerCase(), ':');
    moduleConfigTree.repaint();
    
    simpleModuleView.setPreferredSize(new Dimension(600, 1000));
    simpleModuleView.repaint();
    /*
    // update and configure graph
    if(vv != null)
    {
      moduleTree.remove(vv);
    }

    // use minimum spanning forest for layout
    MinimumSpanningForest2<Node, Edge> mstCreator
      = new MinimumSpanningForest2<Node, Edge>(graph, new DelegateForest<Node, Edge>(),
      DelegateTree.<Node,Edge>getFactory(),
      new Transformer<Edge, Double>()
    {

      public Double transform(Edge e)
      {
        Node p = graph.getSource(e);
        return new Double(graph.getVertexCount() - graph.getSuccessorCount(p));
      }
    });
    Forest<Node,Edge> mst = mstCreator.getForest();
    
    TreeLayout<Node,Edge> baseLayout = new TreeLayout<Node, Edge>(mst);
    //TreeLayout<Node,Edge> baseLayout = new BalloonLayout<Node, Edge>(mst);
    //TreeLayout<Node,Edge> baseLayout = new RadialTreeLayout<Node, Edge>(mst);
    
    StaticLayout<Node,Edge> layout = new StaticLayout<Node, Edge>(graph, baseLayout);
        
    vv = new VisualizationViewer<Node, Edge>(layout);

    // zooming and selecting
    DefaultModalGraphMouse<Node, Edge> mouse =
      new DefaultModalGraphMouse<Node, Edge>();
    vv.setGraphMouse(mouse);
    vv.addKeyListener(mouse.getModeKeyListener());

    // enable selecting the nodes
    mouse.setMode(ModalGraphMouse.Mode.TRANSFORMING);

    // paint modules gray and representations blue
    vv.getRenderContext().setVertexFillPaintTransformer(new Transformer<Node, Paint>()
    {

      public Paint transform(Node n)
      {
        if(n.getType() == ModuleConfiguration.NodeType.Module)
        {
          return Color.lightGray;
        }
        else
        {
          return new Color(128, 128, 255);
        }
      }
    });
    // also paint edges depending if required or providing
    final ModuleConfiguration finalGraph = graph;
    Transformer<Edge, Paint> tEdge = new Transformer<Edge, Paint>()
    {

      public Paint transform(Edge e)
      {
        if(finalGraph.getSource(e).getType() == ModuleConfiguration.NodeType.Module)
        {
          // provided
          return Color.black;
        }
        else
        {
          // required
          return Color.BLUE;
        }
      }
    };
    vv.getRenderContext().setEdgeDrawPaintTransformer(tEdge);
    vv.getRenderContext().setArrowDrawPaintTransformer(tEdge);

    // use name as label
    vv.getRenderContext().setVertexLabelTransformer(new Transformer<Node, String>()
    {

      public String transform(Node n)
      {
        return n.getName();
      }
    });

    // shape should be a rectangle around the label
    vv.getRenderContext().setVertexShapeTransformer(
      new VertexLabelAsShapeRenderer<Node, Edge>(vv.getRenderContext()));
    vv.getRenderer().getVertexLabelRenderer().setPosition(Position.CNTR);

    moduleTree.add(vv, BorderLayout.CENTER);
    moduleTree.validate();
*/
  }//end newObjectReceived

  
  class SimpleModuleView extends JPanel
  {
      
      @Override
      public synchronized void paintComponent(Graphics g)
      {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        
        int y = 50;
        for(String module: activeModulesList)
        {
            g2d.drawLine(100, y, 350, y);
            y += 40;
        }//end for
        
        /*
        int moduleHeight = 40;
        int moduleWidth = 100;
        int i = 0;
        int offsetX = 30;
        int offsetY = 30;
        
        for(String module: activeModulesList)
        {
            g2d.translate(offsetX, offsetY+(i*moduleHeight+10));
            g2d.drawRect(0, 0, moduleWidth, moduleHeight);
            g2d.drawString(module, 20, 30);
            g2d.translate(-offsetX, -(offsetY+(i*moduleHeight+10)));
            i++;
        }//end for
        
        */
      }//end paintComponent
      
  }//end class SimpleModuleView
  
  

  private void sendCommand(Command command)
  {
    final Command commandToExecute = command;
    final JPanel thisFinal = this;
    
    this.parent.getMessageServer().executeSingleCommand(new CommandSender()
    {

      @Override
      public void handleResponse(byte[] result, Command originalCommand)
      {
        if(originalCommand.getName().equals(commandStringStoreModules))
        {
          String response = new String(result);
          if(response.startsWith("ERROR"))
          {
            JOptionPane.showMessageDialog(thisFinal,
              response.substring(6), "ERROR", JOptionPane.ERROR_MESSAGE);
          }else
          {
            JOptionPane.showMessageDialog(thisFinal,
              response, "Info", JOptionPane.INFORMATION_MESSAGE);
          }
        }
      }//end handleResponse

      @Override
      public void handleError(int code)
      {
        JOptionPane.showMessageDialog(thisFinal,
              "Error occure while comunication, code " + code, "ERROR", JOptionPane.ERROR_MESSAGE);
      }

      @Override
      public Command getCurrentCommand()
      {
        return commandToExecute;
      }
    }, command);
  }//end sendCommand


  
  class ModuleCheckBoxListener implements ActionListener, ObjectListener<byte[]>
  {
    JCheckBox checkBox;
    Command currentCommand;
    String commandString;
            
    public ModuleCheckBoxListener(JCheckBox checkBox, String commandString)
    {
      this.checkBox = checkBox;
      this.currentCommand = null;
      this.commandString = commandString;
    }

    @Override
    public void actionPerformed(java.awt.event.ActionEvent evt)
    {
      if(currentCommand != null) {
        genericManagerFactory.getManager(currentCommand).removeListener(this);
      }

      currentCommand = new Command(commandString);
      currentCommand.addArg(checkBox.getText(), checkBox.isSelected() ? "on" : "off");

      genericManagerFactory.getManager(currentCommand).addListener(this);
    }//end actionPerformed

    @Override
    public void errorOccured(String cause)
    {
      if(currentCommand != null) {
        genericManagerFactory.getManager(currentCommand).removeListener(this);
      }
      currentCommand = null;
      System.err.println(cause);
    }//end errorOccured

    @Override
    public void newObjectReceived(byte[] object)
    {
      String str = new String(object);
      String[] res = str.split("( |\n|\r|\t)+");
      
      if(res.length == 3 && 
         res[0].equals("set") &&
         res[1].equals(checkBox.getText())) 
      {
        this.checkBox.setSelected(res[2].equals("on"));
      } else {
          errorOccured(str);
      }
      
      if(currentCommand != null) {
        genericManagerFactory.getManager(currentCommand).removeListener(this);
      }
      currentCommand = null;
    }//end newObjectReceived
  }//end ModuleCheckBoxListener


  @Override
  public void dispose()
  {
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }//end dispose
  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btExport;
    private javax.swing.JButton btSave;
    private javax.swing.JButton btSend;
    private javax.swing.JComboBox cbProcess;
    private de.naoth.rc.dialogs.panels.ExtendedFileChooser fileChooser;
    private javax.swing.JScrollPane jScrollPane;
    private javax.swing.JScrollPane jScrollPaneModulePane;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JToggleButton jToggleButtonRefresh;
    private javax.swing.JToolBar jToolBar1;
    private de.naoth.rc.checkboxtree.CheckboxTree moduleConfigTree;
    private javax.swing.JPanel moduleTree;
    // End of variables declaration//GEN-END:variables
}//end class ModuleConfigurationViewer
