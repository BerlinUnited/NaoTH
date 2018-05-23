/*
 * ParameterPanel.java
 *
 * Created on 17. Juli 2008, 21:21
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.RobotControl;
import de.naoth.rc.components.treetable.ParameterDataModel;
import de.naoth.rc.components.treetable.TreeTable;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.server.Command;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */
public class ParameterPanelTree extends AbstractDialog
{
  @RCDialog(category = RCDialog.Category.Configure, name = "ParameterTree")
  @PluginImplementation
  public static class Plugin extends DialogPlugin<ParameterPanelTree>
  {
    @InjectPlugin
    public static RobotControl parent;
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
  }

  private final ParameterDataModel treeTableModel = new ParameterDataModel();
  private TreeTable myTreeTable;
  
  private final Command cmd_pi = new Command("Cognition:representation:print").addArg("PlayerInfo");
  private static final Pattern KEY_VALUE_PATTERN = Pattern.compile("^(?<key>.+)=(?<value>.*)$", Pattern.MULTILINE);
  
  public ParameterPanelTree()
  {
    initComponents();
	
    myTreeTable = new TreeTable(treeTableModel);
    jScrollPane.setViewportView(myTreeTable);
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jToolBar1 = new javax.swing.JToolBar();
        jToggleButtonList = new javax.swing.JToggleButton();
        jSeparator1 = new javax.swing.JToolBar.Separator();
        activeScheme = new javax.swing.JLabel();
        jSeparator2 = new javax.swing.JToolBar.Separator();
        jScrollPane = new javax.swing.JScrollPane();

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
        jToolBar1.add(jSeparator1);

        activeScheme.setText("scheme:");
        jToolBar1.add(activeScheme);
        jToolBar1.add(jSeparator2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 503, Short.MAX_VALUE)
            .addComponent(jScrollPane)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(jScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 287, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

private void jToggleButtonListActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jToggleButtonListActionPerformed
{//GEN-HEADEREND:event_jToggleButtonListActionPerformed
    listParameters();
    updateActiveScheme();
}//GEN-LAST:event_jToggleButtonListActionPerformed

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

  class ParameterListHandlerGet implements ObjectListener<byte[]>
  {
    private final ParameterListItem item;
    
    public ParameterListHandlerGet(ParameterListItem item) {
        this.item = item;
    }
      
    @Override
    public void newObjectReceived(byte[] object)
    {
        String str = new String(object);
        String[] params = str.split("\\n");
        
        for(String p: params) {
            final String[] s = p.split("=");
            ParameterDataModel.ParameterDataNode n = treeTableModel.insertPath(item.name + "." + s[0], "\\.");
            n.listener = null;
            
            String value = "";
            if (s.length == 2 && s[1] != null) {
                value = s[1];
            } else {
                n.setValue(value);
            }
            
            try { // try int
                n.setValue(Integer.parseInt(value));
            } catch(NumberFormatException exi) {
                try {// try double
                    n.setValue(Double.parseDouble(value));
                } catch (NumberFormatException exd) {
                    // NOTE: Boolean.parseBoolean returns false for every string != true
                    switch (value.toLowerCase()) {
                        case "true":
                            n.setValue(true);
                            break;
                        case "false":
                            n.setValue(false);
                            break;
                        default:
                            n.setValue(value);// string
                            break;
                    }
                }
            }

            
            
            n.listener = new ParameterDataModel.ValueChangedListener() {
                @Override
                public void valueChanged(Object value) {
                    System.out.println(item.name + "." + s[0] + " = " + value.toString());
                    
                    Command cmd = item.getCommandSET();
                    cmd.addArg(s[0], value.toString());
                    Plugin.commandExecutor.executeCommand(new ParameterListHandlerSet(), cmd);
                }
            };
        }
        
        myTreeTable.expandRoot();
        myTreeTable.revalidate();
        myTreeTable.repaint();
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
        System.out.println("[Paremeter Set] " + new String(object));
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
          ParameterListItem item = new ParameterListItem(owner, name);
          Plugin.commandExecutor.executeCommand(new ParameterListHandlerGet(item), item.getCommandGET());
        }
        
        jToggleButtonList.setSelected(false);
    }
    
    @Override
    public void errorOccured(String cause)
    {
      dispose();
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

  @Override
  public void dispose()
  {
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }
  
  private void updateActiveScheme() {
      /* command for PlayerInfo */
      Plugin.commandExecutor.executeCommand(new ObjectListener<byte[]>() {
            @Override
            public void newObjectReceived(byte[] object) {
                // extract representation from string
                Map<String, String> repr = new HashMap<>();
                Matcher m = KEY_VALUE_PATTERN.matcher(new String(object));
                while (m.find()) {
                    repr.put(m.group("key").trim(), m.group("value").trim());
                }
                String scheme = repr.getOrDefault("active scheme", "?");
                activeScheme.setText("scheme: " + (scheme.equals("-")?"default parameter":scheme));
            }

            @Override
            public void errorOccured(String cause) {/* ignore error */}
        }, cmd_pi);
  }
  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel activeScheme;
    private javax.swing.JScrollPane jScrollPane;
    private javax.swing.JToolBar.Separator jSeparator1;
    private javax.swing.JToolBar.Separator jSeparator2;
    private javax.swing.JToggleButton jToggleButtonList;
    private javax.swing.JToolBar jToolBar1;
    // End of variables declaration//GEN-END:variables
}
