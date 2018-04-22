package de.naoth.bdr;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.Dialog;
import de.naoth.rc.server.MessageServer;
import java.awt.Image;
import java.awt.Toolkit;
import java.io.File;
import java.net.URISyntaxException;
import java.util.Arrays;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.SimpleObjectProperty;
import javax.swing.JFrame;
import javax.swing.JPanel;
import net.xeoh.plugins.base.PluginManager;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.impl.PluginManagerFactory;
import net.xeoh.plugins.base.util.JSPFProperties;
import net.xeoh.plugins.base.util.uri.ClassURI;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class RobotControlBdrMonitorImpl extends javax.swing.JFrame implements RobotControlBdrMonitor
{
    public static String panelName = "";
    public static String panelTitle = "";
    public static SimpleObjectProperty<JPanel> healthPanel = new SimpleObjectProperty<JPanel>();

    /**
     * Creates new form RobotControlBdrImpl
     */
    public RobotControlBdrMonitorImpl() {
//        splashScreenMessage("Welcome to RobotControl");

        // load the configuration
        //    readConfigFromFile();
//        try {
            //UIManager.setLookAndFeel(new PlasticXPLookAndFeel());
//            UIManager.setLookAndFeel(new CustomNimbusLookAndFeel(RobotControlBdrImpl.this));
            // set explicitely the Nimbus colors to be used
//      DockUI.getDefaultDockUI().registerColors("de.naoth.rc.CustomNimbusLookAndFeel", new Nimbus6u10());
//        } catch (UnsupportedLookAndFeelException ex) {
//        }

        // icon
        Image icon = Toolkit.getDefaultToolkit().getImage(RobotControl.class.getResource("res/RobotControlLogo128.png"));
        setIconImage(icon);
        initComponents();

        setExtendedState(JFrame.MAXIMIZED_BOTH);
    }
    
  @PluginLoaded
  public void registerDialog(final Dialog dialog)
  {
//    splashScreenMessage("Loading dialog " + dialog.getDisplayName() + "...");
//    this.dialogRegistry.registerDialog(dialog);
      System.out.println("loaded: " + dialog.getDisplayName());
      if(dialog.getDisplayName().equals(panelName)) {
          getContentPane().add(dialog.getPanel(), java.awt.BorderLayout.CENTER);
      } else if(dialog.getDisplayName().equals("RobotHealth")) {
          healthPanel.set(dialog.getPanel());
      }
      // for debugging:
      /*
      if(dialog.getDisplayName().equals("Simspark")) {
          Simspark s = (Simspark) dialog.getPanel();
          s.connect();
      }
      */
      
  }

    /**
     * This method is called from within the constructor to initialize the form. WARNING: Do NOT
     * modify this code. The content of this method is always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        jLayeredPane1 = new javax.swing.JLayeredPane();
        jLabel1 = new javax.swing.JLabel();
        jLayeredPane2 = new javax.swing.JLayeredPane();
        jButton1 = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("BDR - Monitor");
        setUndecorated(true);

        jPanel1.setLayout(new javax.swing.OverlayLayout(jPanel1));

        jLayeredPane1.setLayout(new javax.swing.BoxLayout(jLayeredPane1, javax.swing.BoxLayout.X_AXIS));

        jLabel1.setFont(new java.awt.Font("Dialog", 1, 48)); // NOI18N
        jLabel1.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel1.setText(panelTitle);
        jLabel1.setToolTipText("");
        jLabel1.setBorder(javax.swing.BorderFactory.createEmptyBorder(20, 1, 20, 1));
        jLayeredPane1.add(jLabel1);

        jPanel1.add(jLayeredPane1);

        jLayeredPane2.setLayout(new java.awt.BorderLayout());

        jButton1.setText("X");
        jButton1.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 10, 1, 10));
        jButton1.setBorderPainted(false);
        jButton1.setContentAreaFilled(false);
        jButton1.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });
        jLayeredPane2.add(jButton1, java.awt.BorderLayout.EAST);

        jPanel1.add(jLayeredPane2);

        getContentPane().add(jPanel1, java.awt.BorderLayout.NORTH);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        System.exit(0);
    }//GEN-LAST:event_jButton1ActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(RobotControlBdrMonitorImpl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(RobotControlBdrMonitorImpl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(RobotControlBdrMonitorImpl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(RobotControlBdrMonitorImpl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>
        //</editor-fold>

        // sets the panel, which should be loaded
        panelName = Arrays.asList(args).contains("-m") ? "BDRMonitor" : "BDRControl";
        panelTitle = Arrays.asList(args).contains("-m") ? "Monitoring" : "Remote Maintenance"; //Fernwartung
        
        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                
                final JSPFProperties props = new JSPFProperties();
                props.setProperty(PluginManager.class, "cache.enabled", "false");
                PluginManager pluginManager = PluginManagerFactory.createPluginManager(props);
                // make sure the main frame if loaded first
                pluginManager.addPluginsFrom(new ClassURI(RobotControlBdrMonitorImpl.class).toURI());

                try {
                    File selfFile = new File(RobotControlBdrMonitorImpl.class.getProtectionDomain().getCodeSource().getLocation().toURI());
                    // load all plugins from 
                    pluginManager.addPluginsFrom(selfFile.toURI());
                } catch (URISyntaxException ex) {
                    Logger.getLogger(RobotControlBdrMonitorImpl.class.getName()).log(Level.SEVERE, null, ex);
                }

                // load the robot control itself
                pluginManager.getPlugin(RobotControlBdrMonitor.class).setVisible(true);
            }
        });
    }

    @Override
    public boolean checkConnected() {
        // TODO:
        return false;
    }

    @Override
    public MessageServer getMessageServer() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public Properties getConfig() {
        return null;
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLayeredPane jLayeredPane1;
    private javax.swing.JLayeredPane jLayeredPane2;
    private javax.swing.JPanel jPanel1;
    // End of variables declaration//GEN-END:variables
}
