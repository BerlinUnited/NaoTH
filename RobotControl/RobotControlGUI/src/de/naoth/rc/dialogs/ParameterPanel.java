/*
 * ParameterPanel.java
 *
 * Created on 17. Juli 2008, 21:21
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.server.Command;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.DefaultListModel;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.event.ListSelectionEvent;
import javax.swing.filechooser.FileNameExtensionFilter;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */
public class ParameterPanel extends AbstractDialog
{

  @RCDialog(category = RCDialog.Category.Configure, name = "Parameter")
  @PluginImplementation
  public static class Plugin extends DialogPlugin<ParameterPanel>
  {
    @InjectPlugin
    public static RobotControl parent;
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
  }

  //needed to synchronize the update procedure
  private final ArrayList<ParameterListItem> parameterLists = new ArrayList<ParameterListItem>();
  // number of sources which already performed ther update
  private int updateSources = 0;
  private final int EXPECTED_SOURCES = 2;
  
  final private String parameterSavePathKey = "parameter_save_path";
  final private String defaultConfigPath = "../../NaoTHSoccer/Config/scheme";
  
  public ParameterPanel()
  {
    initComponents();
	
    jTextArea.addKeyListener(new KeyAdapter()
    {

      @Override
      public void keyPressed(KeyEvent e)
      {
        int key = e.getKeyCode();
        if (key == KeyEvent.VK_ENTER)
        {
          sendParameters();
          
          int k = jTextArea.getCaretPosition();
          if(k > 0) {
            jTextArea.setCaretPosition(k-1);
          }
        }
      }
    });
    
    
    listOfParameters.addListSelectionListener((ListSelectionEvent e) -> {
        if(!e.getValueIsAdjusting())
        {
            getParameterList();
        }
    });
  }//end constructor
  
  ParameterListItem getCurrentParameterList() {
      if(listOfParameters.getSelectedIndex() != -1) {
        return listOfParameters.getModel().getElementAt(listOfParameters.getSelectedIndex());
      }
      return null;
  }
  
  ArrayList<ParameterListItem> getAllParameterLists() {
      ArrayList<ParameterListItem> result = new ArrayList<>();
      for (int i = 0; i < listOfParameters.getModel().getSize(); i++) {
        result.add(listOfParameters.getModel().getElementAt(i));
      }
      return result;
  }
  

    private void updateParameterLists() 
    {
        updateSources++;

        if (updateSources == EXPECTED_SOURCES) {
            
            // save selection
            ParameterListItem selectedList = getCurrentParameterList();

            DefaultListModel<ParameterListItem> model = new DefaultListModel();
            for (ParameterListItem i : parameterLists) {
                model.addElement(i);
            }
            
            listOfParameters.setModel(model);
            
            if(selectedList != null) {
                this.listOfParameters.setSelectedValue(selectedList, true);
            } else {
                this.listOfParameters.setSelectedIndex(0);
            }
            
            parameterLists.clear();
            updateSources = 0;
            
            System.out.println(selectedList);
            
        }
    }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        popupMenu = new javax.swing.JPopupMenu();
        miSaveAll = new javax.swing.JMenuItem();
        fcSaveParametersDialog = new javax.swing.JFileChooser();
        jToolBar1 = new javax.swing.JToolBar();
        jToggleButtonList = new javax.swing.JToggleButton();
        jToggleButtonRefresh = new javax.swing.JToggleButton();
        jButtonSend = new javax.swing.JButton();
        jSeparator1 = new javax.swing.JToolBar.Separator();
        btnSave = new javax.swing.JButton();
        btnPopupMenu = new javax.swing.JToggleButton();
        main = new javax.swing.JPanel();
        jSplitPane1 = new javax.swing.JSplitPane();
        names = new javax.swing.JScrollPane();
        listOfParameters = new javax.swing.JList<>();
        parameterPane = new javax.swing.JScrollPane();
        jTextArea = new javax.swing.JTextArea();

        popupMenu.setInvoker(btnPopupMenu);
        popupMenu.addPopupMenuListener(new javax.swing.event.PopupMenuListener() {
            public void popupMenuCanceled(javax.swing.event.PopupMenuEvent evt) {
            }
            public void popupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {
                popupMenuPopupMenuWillBecomeInvisible(evt);
            }
            public void popupMenuWillBecomeVisible(javax.swing.event.PopupMenuEvent evt) {
            }
        });

        miSaveAll.setText("Save all");
        miSaveAll.setToolTipText("Will retrieve all parameter configurations and saves them.");
        miSaveAll.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miSaveAllActionPerformed(evt);
            }
        });
        popupMenu.add(miSaveAll);

        fcSaveParametersDialog.setAcceptAllFileFilterUsed(false);
        fcSaveParametersDialog.setDialogType(javax.swing.JFileChooser.SAVE_DIALOG);
        fcSaveParametersDialog.setDialogTitle("Save configuration");
        fcSaveParametersDialog.setFileFilter(new FileNameExtensionFilter("Config files (*.cfg)", "cfg"));
        fcSaveParametersDialog.setSelectedFile(new java.io.File(".cfg"));

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        jToggleButtonList.setText("update");
        jToggleButtonList.setFocusable(false);
        jToggleButtonList.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jToggleButtonList.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToggleButtonList.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButtonListActionPerformed(evt);
            }
        });
        jToolBar1.add(jToggleButtonList);

        jToggleButtonRefresh.setText("Get");
        jToggleButtonRefresh.setFocusable(false);
        jToggleButtonRefresh.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jToggleButtonRefresh.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToggleButtonRefresh.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButtonRefreshActionPerformed(evt);
            }
        });
        jToolBar1.add(jToggleButtonRefresh);

        jButtonSend.setText("Set");
        jButtonSend.setFocusable(false);
        jButtonSend.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jButtonSend.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jButtonSend.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonSendActionPerformed(evt);
            }
        });
        jToolBar1.add(jButtonSend);
        jToolBar1.add(jSeparator1);

        btnSave.setText("Save");
        btnSave.setToolTipText("Saving current configuration (as shown).");
        btnSave.setFocusable(false);
        btnSave.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSave.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSaveActionPerformed(evt);
            }
        });
        jToolBar1.add(btnSave);

        btnPopupMenu.setText("▼");
        btnPopupMenu.setFocusable(false);
        btnPopupMenu.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnPopupMenu.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnPopupMenu.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnPopupMenuActionPerformed(evt);
            }
        });
        jToolBar1.add(btnPopupMenu);

        main.setLayout(new java.awt.BorderLayout());

        jSplitPane1.setDividerSize(3);

        names.setMinimumSize(new java.awt.Dimension(100, 23));

        listOfParameters.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        listOfParameters.setMaximumSize(new java.awt.Dimension(100, 80));
        names.setViewportView(listOfParameters);

        jSplitPane1.setLeftComponent(names);

        jTextArea.setColumns(20);
        jTextArea.setRows(5);
        parameterPane.setViewportView(jTextArea);

        jSplitPane1.setRightComponent(parameterPane);

        main.add(jSplitPane1, java.awt.BorderLayout.CENTER);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 521, Short.MAX_VALUE)
            .addComponent(main, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(main, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

private void jToggleButtonRefreshActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButtonRefreshActionPerformed
  if (jToggleButtonRefresh.isSelected())
  {
    getParameterList();
  }
}//GEN-LAST:event_jToggleButtonRefreshActionPerformed

private void jButtonSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSendActionPerformed
  sendParameters();
}//GEN-LAST:event_jButtonSendActionPerformed

private void jToggleButtonListActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jToggleButtonListActionPerformed
{//GEN-HEADEREND:event_jToggleButtonListActionPerformed
    listParameters();
}//GEN-LAST:event_jToggleButtonListActionPerformed

    private void btnSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSaveActionPerformed
        // check if a parameter configuration was selected ..
        ParameterListItem selecedList = getCurrentParameterList();
        if(selecedList == null) {
            JOptionPane.showMessageDialog(null, "You have to choose a parameter configuration!", "Error", JOptionPane.ERROR_MESSAGE);
        } else {
            
            String parameterPath = Plugin.parent.getConfig().getProperty(parameterSavePathKey, defaultConfigPath);
            String p = new File(parameterPath).getAbsolutePath();
            // configure the filechooser ...
            fcSaveParametersDialog.setCurrentDirectory(new File(parameterPath));
            fcSaveParametersDialog.setSelectedFile(new java.io.File(selecedList.name + ".cfg"));
            fcSaveParametersDialog.setFileSelectionMode(JFileChooser.FILES_ONLY);
            fcSaveParametersDialog.setAcceptAllFileFilterUsed(false);
            fcSaveParametersDialog.setDialogTitle("Save configuration ("+selecedList.toString()+")");
            fcSaveParametersDialog.setFileFilter(new FileNameExtensionFilter("Config files (*.cfg)", "cfg"));
            // show save dialog
            if(fcSaveParametersDialog.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {

                // if selected file has a file extension - use this, otherwise append ".cfg" to the filename
                File f = (fcSaveParametersDialog.getSelectedFile().getName().lastIndexOf(".") == -1) ? 
                    new File(fcSaveParametersDialog.getSelectedFile()+".cfg") : 
                    fcSaveParametersDialog.getSelectedFile();

                // check if file already exist and call back (if)
                if(!f.exists() || (f.exists() && JOptionPane.showConfirmDialog(this, "File exists, overwrite?", "Overwrite File?", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION)) {
                    try {
                        new FileWriter(f).close(); // trigger exception (if couldn't write)
                        
                        // create file and write parameter configuration to this file
                        this.writeParameterConfig(selecedList, f);
                    } catch (IOException ex) {
                        Logger.getLogger(ParameterPanel.class.getName()).log(Level.SEVERE, null, ex);
                        JOptionPane.showMessageDialog(null, "Selected file is not writeable!", "Not writeable", JOptionPane.ERROR_MESSAGE);
                    }
                }
                
                // save the path for later
                Plugin.parent.getConfig().setProperty(parameterSavePathKey, fcSaveParametersDialog.getSelectedFile().getParent());
            }
        }
    }//GEN-LAST:event_btnSaveActionPerformed

    private void btnPopupMenuActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnPopupMenuActionPerformed
        popupMenu.show(this.btnPopupMenu, 0, this.btnPopupMenu.getHeight());
    }//GEN-LAST:event_btnPopupMenuActionPerformed

    private void popupMenuPopupMenuWillBecomeInvisible(javax.swing.event.PopupMenuEvent evt) {//GEN-FIRST:event_popupMenuPopupMenuWillBecomeInvisible
        this.btnPopupMenu.setSelected(false);
    }//GEN-LAST:event_popupMenuPopupMenuWillBecomeInvisible

    private void miSaveAllActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miSaveAllActionPerformed
        // make sure, we're connected!
        if(Plugin.parent.checkConnected()) {
            // update ui after connecting ...
            ParameterListItem selecedList = getCurrentParameterList();
            if(selecedList == null) {
                listParameters();
            }
            // configure filechooser ...
            fcSaveParametersDialog.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            fcSaveParametersDialog.setAcceptAllFileFilterUsed(true);
            fcSaveParametersDialog.setDialogTitle("Save configuration (All)");
            fcSaveParametersDialog.resetChoosableFileFilters();
            // show save dialog
            if(fcSaveParametersDialog.showSaveDialog(this) == JFileChooser.APPROVE_OPTION)
            {
                if(JOptionPane.showConfirmDialog(this, "Any existing file will be overwritten!\nProceed?", "Overwrite Files?", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION)
                {
                    // the selected file should be a directory!
                    File d = fcSaveParametersDialog.getSelectedFile();
                    if(d.isDirectory())
                    {
                        try {
                            // trigger exception (if couldn't write)
                            File f = File.createTempFile("tmp_"+Math.random(), ".tmp", d);
                            f.delete();
                            // iterate over all listed parameter configurations
                            ArrayList<ParameterListItem> allParameterLists = getAllParameterLists();
                            for (ParameterListItem next : allParameterLists)
                            {
                                Plugin.commandExecutor.executeCommand(new ParameterWriterGet(
                                    next,
                                    new File(d.getPath()+File.separator+next.owner+"_"+next.name+".cfg")
                                ), next.getCommandGET());
                            }
                        } catch (IOException ex) {
                            Logger.getLogger(ParameterPanel.class.getName()).log(Level.SEVERE, null, ex);
                            JOptionPane.showMessageDialog(null, "Selected directory is not writeable!", "Not writeable", JOptionPane.ERROR_MESSAGE);
                        }
                    } else {
                        JOptionPane.showMessageDialog(null, "The selected file is not a directory!", "Not a directory", JOptionPane.WARNING_MESSAGE);
                    }
                }
            }
        }
    }//GEN-LAST:event_miSaveAllActionPerformed

    /**
     * Prepares the the parameter configuration for saving and/or sending to the nao.
     * @return the "prepared" parameter configuration
     */
    private Map<String,String> getText() {
        TreeMap<String, String> result = new TreeMap();
        String text = this.jTextArea.getText();
        text = text.replaceAll("( |\t)+", "");
        String[] lines = text.split("(\n)+");
        for (String l : lines) {
            String[] splitted = l.split("=");
            if (splitted.length == 2) {
                String key = splitted[0].trim();
                String value = splitted[1].trim();
                // remove the last ;
                if (value.charAt(value.length() - 1) == ';') {
                    value = value.substring(0, value.length() - 1);
                }
                result.put(key, value);
            }
        }//end for
        return result;
    }
    
    /**
     * Writes the given parameter configuration to the specified file.
     * @param p the parameter configuration
     * @param f the output file
     * @return true, if writing was successfull
     */
    private boolean writeParameterConfig(ParameterListItem p, File f) {
        try {
            BufferedWriter bf = new BufferedWriter(new FileWriter(f));
            bf.write("["+p.name+"]");
            //NOTE: we also could set the global property 'line.separator'
            //bf.newLine();
            // NOTE: we explicitely need '\n'
            bf.write('\n');
            for (Map.Entry<String, String> cfg : this.getText().entrySet()) {
                bf.write(cfg.getKey() + "=" + cfg.getValue());
                //bf.newLine();
                bf.write('\n');
            }
            bf.close();
            return true;
        } catch (IOException ex) {
            Logger.getLogger(ParameterPanel.class.getName()).log(Level.SEVERE, null, ex);
        }
        return false;
    }
    
  private class ParameterListItem
  {
      public final String owner;
      public final String name;
      
      public ParameterListItem(String owner, String name)
      {
          this.owner = owner;
          this.name = name;
      }
      
      Command getCommandGET() {
          return new Command(owner + ":ParameterList:get")
                  .addArg("<name>", name);
      }
      
      Command getCommandSET() {
          return new Command(owner + ":ParameterList:set")
                  .addArg("<name>", name);
      }
      
      @Override
      public String toString() 
      {
          return "[" + owner + "] " + name;
      }

      @Override
      public boolean equals(Object obj) {
        if(obj instanceof ParameterListItem) {
            ParameterListItem other = (ParameterListItem)obj;
            return owner.equals(other.owner) && name.equals(other.name);
        }
        return false;
      }
  }

  class ParameterWriterGet implements ObjectListener<byte[]>
  {
    private final ParameterListItem item;
    private final File f;
    
    public ParameterWriterGet(ParameterListItem item, File f) {
        this.item = item;
        this.f = f;
    }
      
    @Override
    public void newObjectReceived(byte[] object)
    {
        try {
            BufferedWriter bf = new BufferedWriter(new FileWriter(f));
            bf.write("["+item.name+"]");
            //bf.newLine();
            bf.write('\n');
            bf.write(new String(object));
            bf.close();
        } catch (IOException ex) {
            Logger.getLogger(ParameterPanel.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    @Override
    public void errorOccured(String cause)
    {
        Logger.getLogger(ParameterPanel.class.getName()).log(Level.SEVERE, null, cause);
    }
  }
  
  class ParameterListHandlerGet implements ObjectListener<byte[]>
  {
    @Override
    public void newObjectReceived(byte[] object)
    {
        // remember the carret
        int k = jTextArea.getCaretPosition();
        jTextArea.setText(new String(object));
        try {
            jTextArea.setCaretPosition(k);
        } catch(IllegalArgumentException ex) {
            // do nothing
            // could not set the caret at the right place, e.g.,
            // if the text is shorter now
        }
        jToggleButtonRefresh.setSelected(false);
    }
    
    @Override
    public void errorOccured(String cause)
    {
      dispose();
    }
  }
  
  class ParameterListHandlerSet implements ObjectListener<byte[]>
  {
    @Override
    public void newObjectReceived(byte[] object)
    {
        // do nothing
    }
    
    @Override
    public void errorOccured(String cause)
    {
      dispose();
    }
  }
  
  class ParameterListHandlerList implements ObjectListener<byte[]>
  {
    private final String owner;
    
    public ParameterListHandlerList(String owner)
    {
        this.owner = owner;
    }
    
    @Override
    public void newObjectReceived(byte[] object)
    {
        String strResult = new String(object);
        
        String[] parameterListNames = strResult.split("\n");
        for (String name : parameterListNames) {
          parameterLists.add(new ParameterListItem(owner, name));
        }
        
        jToggleButtonList.setSelected(false);
        updateParameterLists();
    }
    
    @Override
    public void errorOccured(String cause)
    {
      dispose();
    }
  }


private void sendParameters()
{
  if (Plugin.parent.checkConnected())
  {
    ParameterListItem item = getCurrentParameterList();
    if(item != null) {
        Command cmd = item.getCommandSET();
        for (Map.Entry<String, String> cfg : this.getText().entrySet()) {
            cmd.addArg(cfg.getKey(), cfg.getValue());
        }

        Plugin.commandExecutor.executeCommand(new ParameterListHandlerSet(), cmd);
    }
    
    // update everything
    //listParameters();
    
    // this is better, but less robust
    getParameterList();
  }
  else
  {
    jToggleButtonRefresh.setSelected(false);
  }
}

private void listParameters()
{
    if (Plugin.parent.checkConnected())
    {
      Plugin.commandExecutor.executeCommand(new ParameterListHandlerList("Cognition"), new Command("Cognition:ParameterList:list"));
      Plugin.commandExecutor.executeCommand(new ParameterListHandlerList("Motion"), new Command("Motion:ParameterList:list"));
    }
    else
    {
      jToggleButtonList.setSelected(false);
    }
}//end listParameters

  private void getParameterList()
  {
    if (Plugin.parent.checkConnected())
    {
      ParameterListItem item = getCurrentParameterList();
      if(item != null) {
          Plugin.commandExecutor.executeCommand(new ParameterListHandlerGet(), item.getCommandGET());
      }
    }
    else
    {
      jToggleButtonRefresh.setSelected(false);
    }
  }//end refresh

  @Override
  public void dispose()
  {
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }
  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btnPopupMenu;
    private javax.swing.JButton btnSave;
    private javax.swing.JFileChooser fcSaveParametersDialog;
    private javax.swing.JButton jButtonSend;
    private javax.swing.JToolBar.Separator jSeparator1;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JTextArea jTextArea;
    private javax.swing.JToggleButton jToggleButtonList;
    private javax.swing.JToggleButton jToggleButtonRefresh;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JList<ParameterListItem> listOfParameters;
    private javax.swing.JPanel main;
    private javax.swing.JMenuItem miSaveAll;
    private javax.swing.JScrollPane names;
    private javax.swing.JScrollPane parameterPane;
    private javax.swing.JPopupMenu popupMenu;
    // End of variables declaration//GEN-END:variables
}
