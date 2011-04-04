/*
 * ParameterPanel.java
 *
 * Created on 17. Juli 2008, 21:21
 */
package de.hu_berlin.informatik.ki.nao.dialogs;

import de.hu_berlin.informatik.ki.nao.Dialog;
import de.hu_berlin.informatik.ki.nao.RobotControl;
import de.hu_berlin.informatik.ki.nao.server.Command;
import de.hu_berlin.informatik.ki.nao.server.CommandSender;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  gxy
 */
@PluginImplementation
public class ParameterPanel extends javax.swing.JPanel
  implements CommandSender, Dialog
{

  @InjectPlugin
  public RobotControl parent;

  private Command commandToExecute;
  //private String defaultConfigureFilePath;

  /** Creates new form ParameterPanel */
  public ParameterPanel()
  {
    initComponents();
  }

  @Init
  public void init()
  {
//    defaultConfigureFilePath = parent.getMainDirectory().getParentFile().getAbsolutePath() +
//      "/NaoController/";
  }

  public JPanel getPanel()
  {
    return this;
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
    cbParameterId = new javax.swing.JComboBox();
    jToggleButtonRefresh = new javax.swing.JToggleButton();
    jButtonSend = new javax.swing.JButton();
    jButtonStore = new javax.swing.JButton();
    jScrollPane1 = new javax.swing.JScrollPane();
    jTextArea = new javax.swing.JTextArea();

    jToolBar1.setRollover(true);

    cbParameterId.setEditable(true);
    cbParameterId.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "FieldInfo", "CameraSettings" }));
    cbParameterId.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        cbParameterIdActionPerformed(evt);
      }
    });
    jToolBar1.add(cbParameterId);

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

    jButtonSend.setText("Send");
    jButtonSend.setFocusable(false);
    jButtonSend.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    jButtonSend.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    jButtonSend.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonSendActionPerformed(evt);
      }
    });
    jToolBar1.add(jButtonSend);

    jButtonStore.setText("Store");
    jButtonStore.setFocusable(false);
    jButtonStore.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    jButtonStore.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    jButtonStore.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonStoreActionPerformed(evt);
      }
    });
    jToolBar1.add(jButtonStore);

    jTextArea.setColumns(20);
    jTextArea.setRows(5);
    jScrollPane1.setViewportView(jTextArea);

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 503, Short.MAX_VALUE)
      .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 503, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 281, Short.MAX_VALUE))
    );
  }// </editor-fold>//GEN-END:initComponents

private void jToggleButtonRefreshActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButtonRefreshActionPerformed
  if (jToggleButtonRefresh.isSelected())
  {
    refresh();
  }
}//GEN-LAST:event_jToggleButtonRefreshActionPerformed

private void jButtonSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSendActionPerformed
  if (parent.checkConnected())
  {
    Command cmd = new Command(this.cbParameterId.getSelectedItem().toString() + ":set");

    String text = this.jTextArea.getText();

    text = text.replaceAll("( |\t)+", "");
    String[] lines = text.split("(\n)+");
    for (String l : lines)
    {
      String[] splitted = l.split("=");
      if (splitted.length == 2)
      {
        String key = splitted[0].trim();
        String value = splitted[1].trim();
        // remove the last ;
        if (value.charAt(value.length() - 1) == ';')
        {
          value = value.substring(0, value.length() - 1);
        }

        cmd.addArg(key, value);
      }
    }
    sendCommand(cmd);
  }
  else
  {
    jToggleButtonRefresh.setSelected(false);
  }
}//GEN-LAST:event_jButtonSendActionPerformed

private void jButtonStoreActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonStoreActionPerformed
  if (parent.checkConnected())
  {
    Command command = new Command(cbParameterId.getSelectedItem().toString() + ":store");
    sendCommand(command);
  }
  else
  {
    jToggleButtonRefresh.setSelected(false);
  }
}//GEN-LAST:event_jButtonStoreActionPerformed

private void cbParameterIdActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbParameterIdActionPerformed
{//GEN-HEADEREND:event_cbParameterIdActionPerformed

  if ("comboBoxChanged".equals(evt.getActionCommand()))
  {
    refresh();
  }
}//GEN-LAST:event_cbParameterIdActionPerformed

  private void refresh()
  {
    if (parent.checkConnected())
    {
      Command cmd = new Command(cbParameterId.getSelectedItem().toString() + ":list");
      sendCommand(cmd);
    }
    else
    {
      jToggleButtonRefresh.setSelected(false);
    }
  }//end refresh

  private void sendCommand(Command command)
  {
    commandToExecute = command;
    parent.getMessageServer().executeSingleCommand(this, command);
  }

  public void handleResponse(byte[] result, Command originalCommand)
  {
    String strResult = new String(result);
    if (strResult.contains("[DebugServer] unknown command: "))
    {
      JOptionPane.showMessageDialog(this,
        "'" + cbParameterId.getSelectedItem().toString() + "' is not available!", "Parameter List", JOptionPane.ERROR_MESSAGE);
      jToggleButtonRefresh.setSelected(false);
    }
    else
    {
      if (originalCommand.getName().compareTo(cbParameterId.getSelectedItem().toString() + ":list") == 0)
      {
        this.jTextArea.setText(strResult);
        jToggleButtonRefresh.setSelected(false);
      }
      else
      {
//        if (originalCommand.getName().compareTo(cbParameterId.getSelectedItem().toString() + ":store") == 0)
//        {
//          int n = strResult.indexOf("\n");
//          if (0 != n)
//          {
//            String filename = defaultConfigureFilePath + "/" + (new String(result, 0, n));
//            n = n + 1;
//            String content = new String(result, n, result.length - n);
//
//            try
//            {
//              FileWriter fileWriter = new FileWriter(filename, false);
//              fileWriter.write(content);
//              fileWriter.close();
//              JOptionPane.showMessageDialog(this,
//                filename + " saved!", "Parameters List", JOptionPane.INFORMATION_MESSAGE);
//            }
//            catch (IOException ex)
//            {
//              JOptionPane.showMessageDialog(this,
//                ex.toString(), "Parameters List", JOptionPane.ERROR_MESSAGE);
//            }
//          }
//          else
//          {
//            JOptionPane.showMessageDialog(this, cbParameterId.getSelectedItem().toString() + " doesn't have configure file",
//              "Parameters List", JOptionPane.ERROR_MESSAGE);
//          }
//        }

        refresh();
      }
    }
  }//end handleResponse

  public void handleError(int code)
  {
    jToggleButtonRefresh.setSelected(false);
    JOptionPane.showMessageDialog(this,
              "Error occured, code " + code, "ERROR", JOptionPane.ERROR_MESSAGE);
  }//end handleError

  public Command getCurrentCommand()
  {
    return commandToExecute;
  }

  public void dispose()
  {
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }//end dispose
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JComboBox cbParameterId;
  private javax.swing.JButton jButtonSend;
  private javax.swing.JButton jButtonStore;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JTextArea jTextArea;
  private javax.swing.JToggleButton jToggleButtonRefresh;
  private javax.swing.JToolBar jToolBar1;
  // End of variables declaration//GEN-END:variables
}
