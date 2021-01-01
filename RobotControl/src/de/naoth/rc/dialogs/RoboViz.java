package de.naoth.rc.dialogs;

import de.naoth.rc.RobotControl;
import de.naoth.rc.components.FileDrop;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import java.awt.BorderLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFrame;
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
public class RoboViz extends AbstractDialog
{
    private static final String CONFIG_KEY = "RoboViz";
    private final MouseAdapter labelMouseListener;
    
    @RCDialog(category = RCDialog.Category.Tools, name = "RoboViz")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<RoboViz> {
        @InjectPlugin
        public static RobotControl parent;
    }//end Plugin

    /**
     * Creates new RoboViz panel
     */
    public RoboViz() {
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
            // load/show roboviz after everything else was initialized and is displayed
            // INFO: if RC/JavaVM crashes, we have to wait longer -> 'till the frame is shown!
            SwingUtilities.invokeLater(() -> {
                showRoboViz(new File(jarFile));
            });
        }
        // 'install' drop target on this dialog (only for setting RoboViz jar file)
        new FileDrop(this, (files) -> {
            File jar = null;
            for (File file : files) {
                if(checkRoboVizJarFile(file)) {
                    if(jar != null) {
                        JOptionPane.showMessageDialog(this, "Only one valid RoboViz jar file can be used!", "Multiple jar files", JOptionPane.ERROR_MESSAGE);
                        return;
                    }
                    jar = file;
                }
            }
            if(jar != null) {
                if(showRoboViz(jar)) {
                    Plugin.parent.getConfig().setProperty(CONFIG_KEY, jar.getAbsolutePath());
                } else {
                    replaceCenterComponent(jarFileLabel);
                }
            } else {
                JOptionPane.showMessageDialog(this, "No valid RoboViz jar file dropped!", "Invalid jar file(s)", JOptionPane.ERROR_MESSAGE);
            }
        });
    }
    
    /**
     * Adds the Roboviz panel from the given roboviz jar file.
     * 
     * @param the (compiled) roboviz jar file
     * @return true, if panel was successfully loaded, false otherwise
     */
    private boolean showRoboViz(File file) {
        try {
            // subclassed classloader
            URLClassLoader loader = new URLClassLoader (new URL[] {file.toURI().toURL()}, this.getClass().getClassLoader());
            // use the loader for the panel (adding lib paths)
            Class viewerLoader = Class.forName ("rv.ViewerPanelLoader", true, loader);
            Method add = viewerLoader.getDeclaredMethod ("load");
            Object v = add.invoke(null);
            // load the panel class
            Class viewerClass = Class.forName ("rv.ViewerPanel", true, loader);
            // set the parent frame to the RC frame
            Method setParentFrame = viewerClass.getDeclaredMethod ("setFrame", JFrame.class);
            setParentFrame.invoke(v, Plugin.parent);
            // show the menubar of the roboviz panel
            Method showMenuBar = viewerClass.getDeclaredMethod ("showMenuBar", boolean.class);
            showMenuBar.invoke(v, true);
            // retrieve and add roboviz panel
            Method getPanel = viewerClass.getDeclaredMethod ("getPanel");
            JPanel panel = (JPanel) getPanel.invoke(v);
            replaceCenterComponent(panel);
            // remove file drop - don't need it any more
            FileDrop.remove(this);
            // successfully loaded roboviz panel
            return true;
        } catch (java.lang.ClassNotFoundException ex) {
            Logger.getLogger(RoboViz.class.getName()).log(Level.SEVERE, null, ex);
            errorLabel.setText("<html><h2>Unable to load class '"+ex.getMessage()+"'!</h2><br>Was Roboviz compiled with 'panel'-support?!</html>");
            errorLabel.addMouseListener(labelMouseListener);
            replaceCenterComponent(errorLabel);
        } catch (Exception ex) {
            Logger.getLogger(RoboViz.class.getName()).log(Level.SEVERE, null, ex);
            errorLabel.setText("<html><h2>An error occurred!</h2><br>"+ex.getMessage()+"</html>");
            errorLabel.removeMouseListener(labelMouseListener);
            replaceCenterComponent(errorLabel);
        }
        return false;
    }
    
    /**
     * Checks whether or not the roboviz panel could be loaded from the given jar file.
     * 
     * @param f the roboviz jar file, which should be tested
     * @return true, if it seems to be the correct jar file; false otherwise
     */
    private boolean checkRoboVizJarFile(File f) {
        if(f.isFile() && f.exists() && f.getName().endsWith(".jar")) {
            try {
                // subclassed classloader
                URLClassLoader loader = new URLClassLoader (new URL[] {f.toURI().toURL()}, this.getClass().getClassLoader());
                // use the loader to check for existing classes
                Class.forName ("rv.ViewerPanelLoader", false, loader);
                // an exception would be thrown, if the class isn't available
                return true;
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
            if(checkRoboVizJarFile(jar)) {
                if(showRoboViz(jar)) {
                    Plugin.parent.getConfig().setProperty(CONFIG_KEY, jar.getAbsolutePath());
                } else {
                    replaceCenterComponent(jarFileLabel);
                }
            } else {
                JOptionPane.showMessageDialog(this, "The selected jar file isn't a valid RoboViz jar file!", "Invalid jar file", JOptionPane.ERROR_MESSAGE);
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
        jarFileChooser.setDialogTitle("Select RoboViz jar file");
        jarFileChooser.setFileFilter(new FileNameExtensionFilter("JAR file", "jar"));

        errorLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        errorLabel.setText("An error occurred");

        setLayout(new java.awt.BorderLayout());

        jarFileLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jarFileLabel.setText("Select the RoboViz jar file ...");
        add(jarFileLabel, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel errorLabel;
    private javax.swing.JFileChooser jarFileChooser;
    private javax.swing.JLabel jarFileLabel;
    // End of variables declaration//GEN-END:variables
}
