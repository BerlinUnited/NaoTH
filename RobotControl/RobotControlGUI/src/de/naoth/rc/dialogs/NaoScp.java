package de.naoth.rc.dialogs;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import java.awt.BorderLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.filechooser.FileNameExtensionFilter;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class NaoScp extends AbstractDialog
{
    private static final String CONFIG_KEY = "NaoSCP";
    private final MouseAdapter labelMouseListener;
    
    @RCDialog(category = RCDialog.Category.Tools, name = "NaoSCP")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<NaoScp> {
        @InjectPlugin
        public static RobotControl parent;
    }//end Plugin
    
    /**
     * Creates new form NaoScp
     */
    public NaoScp() {
        // reusable mouse listener
        labelMouseListener = new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                labelClicked(evt);
            }
        };
        
        initComponents();
        jarFileLabel.addMouseListener(labelMouseListener);
        
        String jarFile = Plugin.parent.getConfig().getProperty(CONFIG_KEY);
        if(jarFile != null) {
            // load/show naoscp after everything else was initialized and is displayed
            SwingUtilities.invokeLater(() -> {
                showNaoScp(new File(jarFile));
            });
        }
    }
    
    /**
     * Adds the NaoSCP panel from the given naoscp jar file.
     * 
     * @param the (compiled) naoscp jar file
     * @return true, if panel was successfully loaded, false otherwise
     */
    private boolean showNaoScp(File file) {
        try {
            // subclassed classloader
            URLClassLoader loader = new URLClassLoader (new URL[] {file.toURI().toURL()}, this.getClass().getClassLoader());
            // load the panel class
            Class panelClass = Class.forName ("naoscp.NaoSCP", true, loader);
            JPanel panel = (JPanel) panelClass.getConstructor().newInstance();
            // set the parent frame to the RC frame
            Method setParentFrame = panelClass.getDeclaredMethod ("setParentFrame", java.awt.Frame.class);
            setParentFrame.invoke(panel, Plugin.parent);
            // retrieve and add naoscp panel
            replaceCenterComponent(panel);
            // successfully loaded naoscp panel
            return true;
        } catch (java.lang.ClassNotFoundException ex) {
            Logger.getLogger(NaoScp.class.getName()).log(Level.SEVERE, null, ex);
            errorLabel.setText("<html><h2>Unable to load class '"+ex.getMessage()+"'!</h2></html>");
            errorLabel.addMouseListener(labelMouseListener);
            replaceCenterComponent(errorLabel);
        } catch (Exception ex) {
            Logger.getLogger(NaoScp.class.getName()).log(Level.SEVERE, null, ex);
            errorLabel.setText("<html><h2>An error occurred!</h2><br>"+ex.getMessage()+"<br>Was NaoSCP compiled with 'panel'-support?!</html>");
            errorLabel.removeMouseListener(labelMouseListener);
            replaceCenterComponent(errorLabel);
        }
        return false;
    }
    
    /**
     * Checks whether or not the naoscp panel could be loaded from the given jar file.
     * 
     * @param f the naoscp jar file, which should be tested
     * @return true, if it seems to be the correct jar file; false otherwise
     */
    private boolean checkNaoScpJarFile(File f) {
        if(f.isFile() && f.exists() && f.getName().endsWith(".jar")) {
            try {
                // subclassed classloader
                URLClassLoader loader = new URLClassLoader (new URL[] {f.toURI().toURL()}, this.getClass().getClassLoader());
                // use the loader to check for existing classes
                // an exception would be thrown, if the class isn't available
                Class cls = Class.forName ("naoscp.NaoSCP", false, loader);
                // check if the NaoSCP class is a subclass of JPanel
                return !JPanel.class.isAssignableFrom(cls);
            } catch (Exception e) {
                /* ignore exceptions */
            }
        }
        return false;
    }
    
    /**
     * Replaces the center comoponent with the given one.
     * 
     * @param c the new center component for this dialog
     */
    private void replaceCenterComponent(java.awt.Component c) {
        remove(((BorderLayout)getLayout()).getLayoutComponent(BorderLayout.CENTER));
        add(c, BorderLayout.CENTER);
        revalidate();
    }
    
    /**
     * Mouselistener callback for labels.
     * @param ev 
     */
    private void labelClicked(MouseEvent ev) {
        if(jarFileChooser.showOpenDialog(this) ==  javax.swing.JFileChooser.APPROVE_OPTION) {
            File jar = jarFileChooser.getSelectedFile();
            if(checkNaoScpJarFile(jar)) {
                if(showNaoScp(jar)) {
                    Plugin.parent.getConfig().setProperty(CONFIG_KEY, jar.getAbsolutePath());
                } else {
                    replaceCenterComponent(jarFileLabel);
                }
            } else {
                JOptionPane.showMessageDialog(this, "The selected jar file isn't a valid NaoScp jar file!", "Invalid jar file", JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    /**
     * This method is called from within the constructor to initialize the form. WARNING: Do NOT
     * modify this code. The content of this method is always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jarFileChooser = new javax.swing.JFileChooser();
        errorLabel = new javax.swing.JLabel();
        jarFileLabel = new javax.swing.JLabel();

        jarFileChooser.setAcceptAllFileFilterUsed(false);
        jarFileChooser.setDialogTitle("Select NaoSCP jar file");
        jarFileChooser.setFileFilter(new FileNameExtensionFilter("JAR file", "jar"));

        errorLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        errorLabel.setText("An error occurred");

        setLayout(new java.awt.BorderLayout());

        jarFileLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jarFileLabel.setText("Select the NaoSCP jar file ...");
        add(jarFileLabel, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel errorLabel;
    private javax.swing.JFileChooser jarFileChooser;
    private javax.swing.JLabel jarFileLabel;
    // End of variables declaration//GEN-END:variables
}
