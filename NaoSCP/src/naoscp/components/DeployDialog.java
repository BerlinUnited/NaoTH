/*
 */
package naoscp.components;

import java.io.File;
import javax.swing.JFileChooser;

/**
 *
 * @author Robert Martin
 */
public class DeployDialog extends javax.swing.JDialog {
    
    public static enum OPTION {
        APPROVE, CANCEL;
    }
    
    private OPTION option = OPTION.CANCEL;
    
    private File path = null;
    
    private boolean unmountSelected = false;

    /**
     * Creates new form DeployDialog
     */
    public DeployDialog(java.awt.Frame parent) {
        super(parent, true);
        initComponents();
        
        String os = System.getProperty("os.name");
        
        if (!os.startsWith("Linux")) {
            this.unmountButton.setVisible(false);
        }
        
        this.jFileChooser.setControlButtonsAreShown(false);
        this.jFileChooser.setFileHidingEnabled(false);
        
        this.jFileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        this.jFileChooser.setAcceptAllFileFilterUsed(false);
        
        this.usbPanel.addFileChooser(this.jFileChooser);
    }
    
    public OPTION showOpenDialog() {
        this.usbPanel.refresh();
        this.setVisible(true);
        
        return this.option;
    }
    
    public void closeUSBStorageDevice() {
        if(this.unmountSelected) {
            this.usbPanel.closeUSBStorageDevice();
        }
    }
    
    public File getSelectedFile(){
        return this.path;
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jFileChooser = new javax.swing.JFileChooser();
        cancelButton = new javax.swing.JButton();
        usbPanel = new naoscp.components.USBPanel();
        openButton = new javax.swing.JButton();
        unmountButton = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setResizable(false);

        cancelButton.setText("Cancel");
        cancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cancelButtonActionPerformed(evt);
            }
        });

        openButton.setText("Write");
        openButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openButtonActionPerformed(evt);
            }
        });

        unmountButton.setText("Write & unmount");
        unmountButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                unmountButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jFileChooser, javax.swing.GroupLayout.DEFAULT_SIZE, 571, Short.MAX_VALUE)
                .addContainerGap())
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(openButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(unmountButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(cancelButton)
                .addGap(23, 23, 23))
            .addGroup(layout.createSequentialGroup()
                .addComponent(usbPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jFileChooser, javax.swing.GroupLayout.PREFERRED_SIZE, 308, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(usbPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(cancelButton)
                    .addComponent(openButton)
                    .addComponent(unmountButton))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cancelButtonActionPerformed
        this.option = OPTION.CANCEL;
        this.setVisible(false);
    }//GEN-LAST:event_cancelButtonActionPerformed

    private void openButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openButtonActionPerformed
        this.option = OPTION.APPROVE;
        
        this.path = this.jFileChooser.getCurrentDirectory();
        
        this.setVisible(false);
    }//GEN-LAST:event_openButtonActionPerformed

    private void unmountButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_unmountButtonActionPerformed
        this.option = OPTION.APPROVE;
        
        this.path = this.jFileChooser.getCurrentDirectory();
        
        this.unmountSelected = true;
        
        this.setVisible(false);
    }//GEN-LAST:event_unmountButtonActionPerformed



    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton cancelButton;
    private javax.swing.JFileChooser jFileChooser;
    private javax.swing.JButton openButton;
    private javax.swing.JButton unmountButton;
    private naoscp.components.USBPanel usbPanel;
    // End of variables declaration//GEN-END:variables
}
