/*
 * 
 */

/*
 * ModuleConfigurationViewer.java
 *
 * Created on 02.12.2008, 19:09:17
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.RobotControl;
import de.naoth.rc.components.S20BinaryLookup;
import de.naoth.rc.components.checkboxtree.SelectableTreeNode;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.dataformats.ModuleConfiguration;
import de.naoth.rc.dataformats.ModuleConfiguration.Node;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ModuleConfigurationManager;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;
import java.awt.Color;
import java.awt.Component;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.List;
import java.util.stream.Collectors;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JCheckBox;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.tree.TreePath;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public class ModuleConfigurationViewer extends AbstractDialog
        implements ObjectListener<ModuleConfiguration>
{

    @RCDialog(category = RCDialog.Category.Configure, name = "Modules")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<ModuleConfigurationViewer>
    {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static ModuleConfigurationManager moduleConfigurationManager;
        @InjectPlugin
        public static SwingCommandExecutor commandExecutor;
    }//end Plugin

    private final String commandStringStoreModules = "modules:store";
    private final String commandStringSetModules = "modules:set";

    ModuleConfiguration moduleGraph = null;

    private final S20BinaryLookup cbModulesSearch;
    private final S20BinaryLookup cbRepresentationsSearch;

    public ModuleConfigurationViewer()
    {
        initComponents();

        // search within the comboboxes
        this.cbModulesSearch = new S20BinaryLookup(this.cbModules);
        this.cbRepresentationsSearch = new S20BinaryLookup(this.cbRepresentations);
        // only the leafe nodes can be modified
        this.moduleConfigTree.setNonLeafNodesEditable(false);

        this.modulePanel.setTree(moduleConfigTree);
        this.modulePanel.setProcessName((String) cbProcess.getSelectedItem());

        this.errorList.setCellRenderer(new DefaultListCellRenderer() {
            final Color error   = new Color(1.0f, 0.0f, 0.0f, 0.3f);
            final Color warning = new Color(1.0f, 1.0f, 0.0f, 0.4f);
            final Color info    = Color.white;

            @Override
            public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
              Component component = super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);

              if(isSelected) {
                //component.setBackground(lightBlue);
              } else {
                switch(((ListComponent)value).getLevel())
                {
                    case ERROR: 
                        component.setBackground(error);
                        break;
                    case WARNING:
                        component.setBackground(warning);
                        break;
                    case NOTE:
                    default:
                        component.setBackground(info);
                }
                
              }
              return component;
            }
        });
        
        /*
        ArrayList<ListComponent> listComponents = new ArrayList<ListComponent>();
        Node n = null;
        listComponents.add(ListComponent.error(null, "wefwefw"));
        listComponents.add(ListComponent.warning(null, "wefwefw2"));
        listComponents.add(ListComponent.error(null, "wefwefw3"));
        listComponents.add(ListComponent.info(null, "wefwefw"));
        errorList.setListData(listComponents.toArray());
        */
    }

    @Init
    @Override
    public void init()
    {
        Plugin.moduleConfigurationManager.setModuleOwner((String) cbProcess.getSelectedItem());
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        fileChooser = new de.naoth.rc.components.ExtendedFileChooser();
        jToolBar1 = new javax.swing.JToolBar();
        jToggleButtonRefresh = new javax.swing.JToggleButton();
        btSave = new javax.swing.JButton();
        btSend = new javax.swing.JButton();
        cbProcess = new javax.swing.JComboBox();
        cbModules = new javax.swing.JComboBox();
        cbRepresentations = new javax.swing.JComboBox();
        jSplitPane1 = new javax.swing.JSplitPane();
        modulePanel = new de.naoth.rc.components.SimpleModulePanel();
        jSplitPane2 = new javax.swing.JSplitPane();
        jScrollPane = new javax.swing.JScrollPane();
        moduleConfigTree = new de.naoth.rc.components.checkboxtree.CheckboxTree();
        errorPanel = new javax.swing.JPanel();
        jToolBar2 = new javax.swing.JToolBar();
        btErrors = new javax.swing.JToggleButton();
        btWarnings = new javax.swing.JToggleButton();
        btNotification = new javax.swing.JToggleButton();
        jScrollPane2 = new javax.swing.JScrollPane();
        errorList = new javax.swing.JList();

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

        cbModules.setEditable(true);
        cbModules.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "<empty>" }));
        cbModules.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbModulesActionPerformed(evt);
            }
        });
        jToolBar1.add(cbModules);

        cbRepresentations.setEditable(true);
        cbRepresentations.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "<empty>" }));
        cbRepresentations.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbRepresentationsActionPerformed(evt);
            }
        });
        jToolBar1.add(cbRepresentations);

        jSplitPane1.setDividerLocation(600);
        jSplitPane1.setResizeWeight(1.0);
        jSplitPane1.setLeftComponent(modulePanel);

        jSplitPane2.setDividerLocation(300);
        jSplitPane2.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        jSplitPane2.setOneTouchExpandable(true);

        jScrollPane.setBorder(null);
        jScrollPane.setPreferredSize(new java.awt.Dimension(200, 322));
        jScrollPane.setViewportView(moduleConfigTree);

        jSplitPane2.setTopComponent(jScrollPane);

        errorPanel.setLayout(new java.awt.BorderLayout());

        jToolBar2.setFloatable(false);
        jToolBar2.setRollover(true);

        btErrors.setSelected(true);
        btErrors.setText("Errors");
        btErrors.setFocusable(false);
        btErrors.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btErrors.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btErrors.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btErrorsActionPerformed(evt);
            }
        });
        jToolBar2.add(btErrors);

        btWarnings.setSelected(true);
        btWarnings.setText("Warnings");
        btWarnings.setFocusable(false);
        btWarnings.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btWarnings.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btWarnings.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btWarningsActionPerformed(evt);
            }
        });
        jToolBar2.add(btWarnings);

        btNotification.setSelected(true);
        btNotification.setText("Notification");
        btNotification.setFocusable(false);
        btNotification.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btNotification.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btNotification.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btNotificationActionPerformed(evt);
            }
        });
        jToolBar2.add(btNotification);

        errorPanel.add(jToolBar2, java.awt.BorderLayout.NORTH);

        errorList.setToolTipText("");
        errorList.setSelectionBackground(new java.awt.Color(255, 255, 255));
        errorList.addListSelectionListener(new javax.swing.event.ListSelectionListener() {
            public void valueChanged(javax.swing.event.ListSelectionEvent evt) {
                errorListValueChanged(evt);
            }
        });
        jScrollPane2.setViewportView(errorList);

        errorPanel.add(jScrollPane2, java.awt.BorderLayout.CENTER);

        jSplitPane2.setBottomComponent(errorPanel);

        jSplitPane1.setRightComponent(jSplitPane2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 672, Short.MAX_VALUE)
            .addComponent(jSplitPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 357, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jToggleButtonRefreshActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButtonRefreshActionPerformed
        if (Plugin.parent.checkConnected())
        {
            if (jToggleButtonRefresh.isSelected()) {
                Plugin.moduleConfigurationManager.addListener(this);
            } else {
                Plugin.moduleConfigurationManager.removeListener(this);
            }
        }
        else
        {
            jToggleButtonRefresh.setSelected(false);
        }
}//GEN-LAST:event_jToggleButtonRefreshActionPerformed

    private void btSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btSaveActionPerformed
        JOptionPane.showMessageDialog(this,
                "This doesn't work now.", "INFO", JOptionPane.INFORMATION_MESSAGE);
        /*
         fileChooser.showSaveDialog(this);
         File selectedFile = fileChooser.getSelectedFile();

         if(selectedFile == null) {
         return;
         }
      
      
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
        sendCommand(new Command((String) cbProcess.getSelectedItem() + ":" + commandStringStoreModules));
    }//GEN-LAST:event_btSendActionPerformed

    private void cbProcessActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbProcessActionPerformed
    {//GEN-HEADEREND:event_cbProcessActionPerformed
        String process = (String) cbProcess.getSelectedItem();
        Plugin.moduleConfigurationManager.setModuleOwner(process);
        this.moduleConfigTree.clear();

        this.modulePanel.setProcessName(process);
        this.modulePanel.setNode(null);
    }//GEN-LAST:event_cbProcessActionPerformed

    private void cbModulesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbModulesActionPerformed
        Node node = (Node) this.cbModules.getSelectedItem();
        if (this.moduleGraph == null || node == null)
        {
            return;
        }
        for (ModuleConfiguration.Node n : this.moduleGraph.getNodeList())
        {
            if (node.equals(n))
            {
                this.modulePanel.setNode(n);
                break;
            }
        }
    }//GEN-LAST:event_cbModulesActionPerformed

    private void cbRepresentationsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbRepresentationsActionPerformed
        Node node = (Node) this.cbRepresentations.getSelectedItem();
        if (this.moduleGraph == null || node == null)
        {
            return;
        }
        for (ModuleConfiguration.Node n : this.moduleGraph.getNodeList())
        {
            if (node.equals(n))
            {
                this.modulePanel.setNode(n);
                break;
            }
        }
    }//GEN-LAST:event_cbRepresentationsActionPerformed

    private void btNotificationActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btNotificationActionPerformed
    {//GEN-HEADEREND:event_btNotificationActionPerformed
        if (Plugin.parent.checkConnected()) {
            makeCheck();
        }
    }//GEN-LAST:event_btNotificationActionPerformed

    private void btErrorsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btErrorsActionPerformed
        if (Plugin.parent.checkConnected()) {
            makeCheck();
        }
    }//GEN-LAST:event_btErrorsActionPerformed

    private void btWarningsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btWarningsActionPerformed
        if (Plugin.parent.checkConnected()) {
            makeCheck();
        }
    }//GEN-LAST:event_btWarningsActionPerformed

    private void errorListValueChanged(javax.swing.event.ListSelectionEvent evt) {//GEN-FIRST:event_errorListValueChanged
        if (errorList.getSelectedValue() != null)
        {
            ListComponent c = (ListComponent) errorList.getSelectedValue();
            modulePanel.setNode(c.getNode());
        }
    }//GEN-LAST:event_errorListValueChanged

    @Override
    public void newObjectReceived(final ModuleConfiguration graph)
    {
        Plugin.moduleConfigurationManager.removeListener(this);
        SwingUtilities.invokeLater(() -> {updateModuleConfiguration(graph);});
    }
    
    @Override
    public void errorOccured(String cause)
    {
        Plugin.moduleConfigurationManager.removeListener(this);
    }

    private void nodeExpander(String strPath, String sep) {
        ArrayList<Object> oPath = new ArrayList<>();
        String[] parts = strPath.split(sep);
        List<SelectableTreeNode> nodes = Arrays.asList(moduleConfigTree.getRootNode());

        for (String part : parts) {
            for (SelectableTreeNode node : nodes) {
                if (node.getText().equals(part)) {
                    oPath.add(node);
                    nodes = Collections.list(node.children());
                    break;
                }
            }
        }
        moduleConfigTree.expandPath(new TreePath(oPath.toArray()));
    }
    
    private void updateModuleConfiguration(final ModuleConfiguration graph)
    {
        this.moduleGraph = graph;
        // get expanded nodes
        Enumeration<TreePath> expendedNodes = moduleConfigTree.getExpandedDescendants(new TreePath(moduleConfigTree.getModel().getRoot()));
        // save last scrollbar position (viewport)
        final Point scrollPosition = jScrollPane.getViewport().getViewPosition();
        // save last selection
        TreePath selection = moduleConfigTree.getSelectionPath();

        this.cbModules.removeAllItems();
        this.cbRepresentations.removeAllItems();
        this.moduleConfigTree.clear(); // clear before adding/updating new ones

        ArrayList<Node> modules = new ArrayList<Node>();
        ArrayList<Node> representations = new ArrayList<Node>();

        this.jToggleButtonRefresh.setSelected(false);
        String processName = (String) cbProcess.getSelectedItem();

        // update list and remove unused modules from graph
        ArrayList<Node> nodes = graph.getNodeList();
        for (ModuleConfiguration.Node n : nodes)
        {
            if (n.getType() == ModuleConfiguration.NodeType.Module)
            {
                String path = n.getCleanPathForProcess(processName);

                SelectableTreeNode node = moduleConfigTree.insertPath(path, ':');
                node.setSelected(n.isEnabled());
                node.setTooltip(n.getName());
                node.getComponent().addActionListener(
                        new ModuleCheckBoxListener(node.getComponent(),
                                processName + ":" + commandStringSetModules, n));

                modules.add(n);
            }
            else if (n.getType() == ModuleConfiguration.NodeType.Represenation)
            {
                representations.add(n);
            }
        }//end for

        // update the combo boxes
        Collections.sort(modules, new CormpareIgnoreCase());
        Collections.sort(representations, new CormpareIgnoreCase());
        this.cbModules.setModel(new javax.swing.DefaultComboBoxModel(modules.toArray()));
        this.cbRepresentations.setModel(new javax.swing.DefaultComboBoxModel(representations.toArray()));
        this.cbModulesSearch.revalidateModel();
        this.cbRepresentationsSearch.revalidateModel();

        moduleConfigTree.cleanTree();
        moduleConfigTree.expandPath(processName, ':');
        moduleConfigTree.repaint();
        // previously expended nodes ...
        if (expendedNodes != null) {
            // get "restored"
            while (expendedNodes.hasMoreElements()) {
                String path = Arrays.stream(expendedNodes.nextElement().getPath()).map((t) -> { return t.toString(); }).collect(Collectors.joining(":"));
                nodeExpander(path, ":");
            }
        }
        // restore selection
        if(selection != null) {
            // we need to remove the root node ... :/
            String path = Arrays.stream(selection.getPath()).map((t) -> { return t.toString(); }).collect(Collectors.joining(":"));
            moduleConfigTree.selectNode(path.substring(moduleConfigTree.getModel().getRoot().toString().length()+1), ':');
        }
        // restore last scrollbar position (viewport)
        
        jScrollPane.getViewport().setViewPosition(scrollPosition);

        //check for unprovided or not required Representations
        makeCheck();
    }//end newObjectReceived

    /**
     * checks for references in the module config trees that are not required or
     * provided by any modules
     */
    public void makeCheck()
    {
        errorList.removeAll();
        ArrayList<ListComponent> notification = new ArrayList<ListComponent>();
        ArrayList<ListComponent> warnings = new ArrayList<ListComponent>();
        ArrayList<ListComponent> errors = new ArrayList<ListComponent>();
        
        for (Node n : this.moduleGraph.getNodeList())
        {
            if (n.isEnabled() && (n.getType() == ModuleConfiguration.NodeType.Represenation))
            {
                boolean required = n.isProvide();
                boolean provided = n.isRequire();
                if(n.require.isEmpty()) {
                    notification.add(ListComponent.note(n, n.getName() + " is required but not provided ("+n.require.size()+")"));
                } else if(n.provide.isEmpty()) {
                    notification.add(ListComponent.note(n, n.getName() + " is provided but not required ("+n.provide.size()+")"));
                } else if (required && !provided) {
                    errors.add(ListComponent.error(n, n.getName() + " is required but not provided ("+n.require.size()+")"));
                } else if(provided && !required) {
                    warnings.add(ListComponent.warning(n, n.getName() + " is provided but not required ("+n.provide.size()+")"));
                }

                //put data into errorList
                ArrayList<ListComponent> listComponents = new ArrayList<ListComponent>();
                if(this.btErrors.isSelected()) {
                    listComponents.addAll(errors);
                }
                if(this.btWarnings.isSelected()) {
                    listComponents.addAll(warnings);
                }
                if(this.btNotification.isSelected()) {
                    listComponents.addAll(notification);
                }
                errorList.setListData(listComponents.toArray());

                updateModuleErrorStatus(errors.size(), warnings.size(), notification.size());
            }
        }

    }
    
    private void updateModuleErrorStatus(int numberErrors, int numberWarnings, int numberNotifications) {
        this.btErrors.setText(String.format("%d Errors", numberErrors));
        this.btWarnings.setText(String.format("%d Warnings", numberWarnings));
        this.btNotification.setText(String.format("%d Notifications", numberNotifications));
    }

    /**
     * saves a warning for a nor required or not provided representation and a
     * reference to this representation
     */
    static class ListComponent
    {
        enum Level {
            NOTE,
            WARNING,
            ERROR
        }
        
        private Node n;
        private final String message;
        private Level level = Level.NOTE;

        public static ListComponent error(Node n, String message) {
            return new ListComponent(Level.ERROR, n, message);
        }
        
        public static ListComponent note(Node n, String message) {
            return new ListComponent(Level.NOTE, n, message);
        }
        
        public static ListComponent warning(Node n, String message) {
            return new ListComponent(Level.WARNING, n, message);
        }
        
        public ListComponent(Level level, Node n, String message)
        {
            this.n = n;
            this.level = level;
            this.message = level.name() + ": " + message;
        }
        
        public ListComponent(Node n, String message)
        {
            this.n = n;
            this.message = message;
        }

        public Node getNode()
        {
            return n;
        }
        
        public Level getLevel() {
            return level;
        }

        @Override
        public String toString()
        {
            return message;
        }
    }
    
    public class CormpareIgnoreCase implements Comparator<Object>
    {

        public int compare(Object o1, Object o2)
        {
            String s1 = o1.toString().toLowerCase();
            String s2 = o2.toString().toLowerCase();
            return s1.compareTo(s2);
        }
    }

    private void sendCommand(Command command)
    {
        if (!Plugin.parent.checkConnected())
        {
            return;
        }

        Plugin.parent.getMessageServer().executeSingleCommand(new DialogCommandSender(this, command)
        {
            @Override
            public void handleResponse(byte[] result, Command originalCommand)
            {
                handleResponseSaveModules(result, originalCommand);
            }
        });

    }//end sendCommand

    abstract class DialogCommandSender implements CommandSender
    {

        private final Command command;
        private final Component parent;

        DialogCommandSender(Component parent, Command command)
        {
            this.command = command;
            this.parent = parent;
        }

        @Override
        public void handleError(int code)
        {
            JOptionPane.showMessageDialog(parent,
                    "Error occure while comunication, code " + code,
                    "ERROR", JOptionPane.ERROR_MESSAGE);
        }

        @Override
        public Command getCurrentCommand()
        {
            return command;
        }
    }//end DialogCommandSender

    public void handleResponseSaveModules(byte[] result, Command originalCommand)
    {
        String response = new String(result);

        if (response.startsWith("ERROR"))
        {
            JOptionPane.showMessageDialog(this,
                    response.substring(6), "ERROR", JOptionPane.ERROR_MESSAGE);
        }
        else
        {
            JOptionPane.showMessageDialog(this,
                    response, "INFO", JOptionPane.INFORMATION_MESSAGE);

        }
    }

    class ModuleCheckBoxListener implements ActionListener, ObjectListener<byte[]>
    {
        JCheckBox checkBox;
        String commandString;
        Node node;

        public ModuleCheckBoxListener(JCheckBox checkBox, String commandString, Node node)
        {
            this.checkBox = checkBox;
            this.commandString = commandString;
            this.node = node;
        }

        @Override
        public void actionPerformed(java.awt.event.ActionEvent evt)
        {
            Plugin.commandExecutor.executeCommand(this, 
                new Command(commandString).addArg(checkBox.getText(), checkBox.isSelected() ? "on" : "off")
            );
            
            // update the module panel
            modulePanel.setNode(node);
        }//end actionPerformed

        @Override
        public void errorOccured(String cause)
        {
            System.err.println(cause);
        }

        @Override
        public void newObjectReceived(byte[] object)
        {
            String str = new String(object);
            String[] res = str.split("( |\n|\r|\t)+");

            if (   res.length == 3
                && res[0].equals("set")
                && res[1].equals(checkBox.getText()))
            {
                this.checkBox.setSelected(res[2].equals("on"));
            }
            else {
                errorOccured(str);
            }
        }//end newObjectReceived
    }//end ModuleCheckBoxListener

    @Override
    public void dispose()
    {
        System.out.println("Dispose is not implemented for: " + this.getClass().getName());
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btErrors;
    private javax.swing.JToggleButton btNotification;
    private javax.swing.JButton btSave;
    private javax.swing.JButton btSend;
    private javax.swing.JToggleButton btWarnings;
    private javax.swing.JComboBox cbModules;
    private javax.swing.JComboBox cbProcess;
    private javax.swing.JComboBox cbRepresentations;
    private javax.swing.JList errorList;
    private javax.swing.JPanel errorPanel;
    private de.naoth.rc.components.ExtendedFileChooser fileChooser;
    private javax.swing.JScrollPane jScrollPane;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JSplitPane jSplitPane2;
    private javax.swing.JToggleButton jToggleButtonRefresh;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JToolBar jToolBar2;
    private de.naoth.rc.components.checkboxtree.CheckboxTree moduleConfigTree;
    private de.naoth.rc.components.SimpleModulePanel modulePanel;
    // End of variables declaration//GEN-END:variables
}//end class ModuleConfigurationViewer
