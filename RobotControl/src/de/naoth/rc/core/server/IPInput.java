/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.core.server;

import javax.swing.DefaultComboBoxModel;

/**
 *
 * @author Heinrich Mellmann
 */
public class IPInput extends javax.swing.JPanel {

    public IPInput() {
        initComponents();
    }

    synchronized public void addAddress(String address)  {
        DefaultComboBoxModel model = (DefaultComboBoxModel) cbHost.getModel();
        if(model.getIndexOf(address) == -1) {
            cbHost.addItem(address);
        }
    }
    
    public void setSelectedAddress(String address) {
        if(address != null) {
          cbHost.setSelectedItem(address);
        }
    }
    
    public void setPort(String port) {
        if(port != null) {
          txtPort.setText(port);
        }
    }
    
    public void setPort(int port) {
        txtPort.setText("" + port);
    }
    
    public String getAddress() {
        return (String) cbHost.getSelectedItem();
    }
    
    public int getPort() {
        return Integer.parseInt(txtPort.getText());
    }
    
    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        cbHost = new javax.swing.JComboBox();
        jLabel1 = new javax.swing.JLabel();
        txtPort = new javax.swing.JTextField();

        setLayout(new javax.swing.BoxLayout(this, javax.swing.BoxLayout.LINE_AXIS));

        cbHost.setEditable(true);
        cbHost.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "localhost" }));
        add(cbHost);

        jLabel1.setText(":");
        add(jLabel1);

        txtPort.setColumns(6);
        txtPort.setText("5401");
        txtPort.setMaximumSize(new java.awt.Dimension(60, 2147483647));
        txtPort.setMinimumSize(new java.awt.Dimension(60, 20));
        add(txtPort);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JComboBox cbHost;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JTextField txtPort;
    // End of variables declaration//GEN-END:variables
}
