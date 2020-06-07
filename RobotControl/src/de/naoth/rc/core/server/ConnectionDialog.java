/*
 * 
 */

/*
 * ConnectionDialog.java
 *
 * Created on 01.06.2009, 10:57:03
 */
package de.naoth.rc.core.server;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.net.InetAddress;
import java.util.Properties;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComponent;
import javax.swing.KeyStroke;

/**
 *
 * @author thomas
 */
public class ConnectionDialog extends javax.swing.JDialog
{

  private final MessageServer messageServer;
  private final Properties properties;
  private final ExecutorService executor = Executors.newCachedThreadPool();

  public ConnectionDialog(java.awt.Frame parent, MessageServer messageServer)
  {
      this(parent, messageServer, new Properties());
  }
          
  public ConnectionDialog(java.awt.Frame parent, MessageServer messageServer, Properties properties)
  {
    super(parent, true);
    initComponents();
    
    this.messageServer = messageServer;
    this.properties = properties;

    String host = this.properties.getProperty("hostname");
    if(host != null)
    {
      cbHost.setSelectedItem(host);
    }

    String port = this.properties.getProperty("port");
    if(port != null)
    {
      txtPort.setText(port);
    }

    this.getRootPane().setDefaultButton(this.btConnect);

    // close by pressing esc
    ActionListener actionListener = new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent actionEvent) {
        dispose();
      }
    };

    KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
    this.getRootPane().registerKeyboardAction(actionListener, stroke, JComponent.WHEN_IN_FOCUSED_WINDOW);
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        cbHost = new javax.swing.JComboBox();
        jLabel1 = new javax.swing.JLabel();
        txtPort = new javax.swing.JTextField();
        btConnect = new javax.swing.JButton();
        btCancel = new javax.swing.JButton();

        setTitle("Connect");
        setLocationByPlatform(true);

        cbHost.setEditable(true);
        cbHost.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "localhost" }));

        jLabel1.setText(":");

        txtPort.setText("5401");

        btConnect.setMnemonic('c');
        btConnect.setText("Connect");
        btConnect.setToolTipText("Connect with this IP and port");
        btConnect.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btConnectActionPerformed(evt);
            }
        });

        btCancel.setMnemonic('e');
        btCancel.setText("Cancel");
        btCancel.setToolTipText("Do not connect and close this dialog");
        btCancel.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btCancelActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(cbHost, javax.swing.GroupLayout.PREFERRED_SIZE, 189, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(txtPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(btConnect)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(btCancel)))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(txtPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(cbHost, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btConnect)
                    .addComponent(btCancel))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void btCancelActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btCancelActionPerformed
    {//GEN-HEADEREND:event_btCancelActionPerformed

      this.setVisible(false);

    }//GEN-LAST:event_btCancelActionPerformed

    private void btConnectActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btConnectActionPerformed
    {//GEN-HEADEREND:event_btConnectActionPerformed
        String host = (String) cbHost.getSelectedItem();
        int port = Integer.parseInt(txtPort.getText());
        
        if(this.messageServer.connect(host, port)) {
            addItemToCombobox(host);

            this.properties.put("hostname", host);
            this.properties.put("port", "" + port);

            setVisible(false);
        }
    }//GEN-LAST:event_btConnectActionPerformed

    // TODO: for now it's adjusted for the current NaoTH addresses. Make it more general.
    private void updateAvaliableHosts() {
        
        String[] ips = this.properties.getProperty("iplist","").split(",");
        
        for(final String ip: ips) {
            executor.submit(() -> {
                try {
                    InetAddress address = InetAddress.getByName(ip);
                    if(address.isReachable(500)) {
                        synchronized(cbHost) {
                            addItemToCombobox(ip);
                        }
                    }
                } catch (Exception e) { /* ignore exception */ }
            });
        }
    }
    
    @Override
    public void setVisible(boolean b) {
        // check for available hosts only when dialog is visible
        if(b) {
            updateAvaliableHosts();
        }
        super.setVisible(b);
    }
    
    private void addItemToCombobox(String host) {
        DefaultComboBoxModel model = (DefaultComboBoxModel) cbHost.getModel();
        if(model.getIndexOf(host) == -1) {
            cbHost.addItem(host);
        }
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btCancel;
    private javax.swing.JButton btConnect;
    private javax.swing.JComboBox cbHost;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JTextField txtPort;
    // End of variables declaration//GEN-END:variables
}