 /*
 * LogfileInspector.java
 *
 * Created on 06.08.2010, 17:01:29
 */

package de.naoth.rc.dialogs;


import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.logplayer.LogSimulator;
import de.naoth.rc.logplayer.LogSimulator.LogSimulatorManager;
import de.naoth.rc.logplayer.LogfileAutoPlayer;
import de.naoth.rc.manager.GenericManagerFactory;
import java.awt.Color;
import java.awt.DefaultKeyboardFocusManager;
import java.awt.KeyEventPostProcessor;
import java.awt.event.KeyEvent;
import java.io.File;
import java.io.IOException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public class LogfilePlayer extends AbstractDialog
{
  @RCDialog(category = RCDialog.Category.Log, name = "Player")
  @PluginImplementation
  public static class Plugin extends DialogPlugin<LogfilePlayer> {
    @InjectPlugin
    public static RobotControl parent;
    @InjectPlugin
    public static GenericManagerFactory genericManagerFactory;
  }
   
  private final LogPerceptListener logPerceptListener = new LogPerceptListener();
  
  LogSimulatorManager logSimulator = null;
  LogfileAutoPlayer logFileAutoPlayer = null;
  private boolean autoSliderChange = false;
  private int minFrame;
  private int maxFrame;
  //Hack um FrameNumber zu bekommen
 // private static int currentFrame = -1;
  private static String fileName = "";
  
  private static final String LAST_FILE_PATH_PROPERTY = "logfileplayer.last.file.path";
  
  public LogfilePlayer() {
      initComponents();
      LogSimulator.LogSimulatorManager.getInstance().addListener(logPerceptListener);
      this.fileChooser.setFileFilter(new LogFileFilter());
      String path = Plugin.parent.getConfig().getProperty(LAST_FILE_PATH_PROPERTY);
      if(path != null) {
          File f = new File(path);
          if(f.exists() && f.isDirectory()) {
            this.fileChooser.setCurrentDirectory(f);
          }
      }
      
      try {
        this.logSimulator = LogSimulatorManager.getInstance();//new LogSimulator();
      } catch(UnsatisfiedLinkError err) {
            JOptionPane.showMessageDialog(null,
              err.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
      }
      
      DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager().
            addKeyEventPostProcessor(new LogfilePlayerKeyController());
  }  
    
  public static String getFileName () {
      return LogfilePlayer.fileName;
  }
    
  class LogfilePlayerKeyController implements KeyEventPostProcessor 
    {
        @Override
        public boolean postProcessKeyEvent(KeyEvent e) {
            
            if(e.getID() != KeyEvent.KEY_PRESSED || !LogfilePlayer.this.isEnabled()) {
                return false;
            }
            
           
            
            // stop the player first if necessary
            if(("awsdrlp").indexOf(e.getKeyChar()) >= 0)
            {
                if (logFileAutoPlayer != null) 
                {
                    waitTillThreadsDeath();
                    
                    if(e.getKeyChar() == 'p' || e.getKeyChar() == 'l') {
                        return false;
                    }
                }
            }
            
            switch(e.getKeyChar()) {
                case 'd':
                    LogfilePlayer.this.logSimulator.stepForward();
                    return true;
                case 'a':
                    LogfilePlayer.this.logSimulator.stepBack();
                    return true;
                case 'w':
                    LogfilePlayer.this.logSimulator.jumpToBegin();
                    return true;
                case 's':
                    LogfilePlayer.this.logSimulator.jumpToEnd();
                    return true;
                case 'r':
                    LogfilePlayer.this.logSimulator.jumpTo(logSimulator.getCurrentFrame());
                    return true;
                case 'p':                    
                    logFileAutoPlayer = new LogfileAutoPlayer();
                    logFileAutoPlayer.start();
                    return true;
                case 'l':
                    logFileAutoPlayer = new LogfileAutoPlayer(true);
                    logFileAutoPlayer.start();
                    return true;
            }
            
            return false;
        }
  }
  
  @Override
  public void dispose() 
  {
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        fileChooser = new javax.swing.JFileChooser();
        fileNameLabel = new javax.swing.JLabel();
        jTextField2 = new javax.swing.JTextField();
        jButton2 = new javax.swing.JButton();
        jLabel1 = new javax.swing.JLabel();
        jButton3 = new javax.swing.JButton();
        jButton4 = new javax.swing.JButton();
        jTextField1 = new javax.swing.JTextField();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        logSlider = new javax.swing.JSlider();
        openButton = new javax.swing.JButton();

        addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        fileNameLabel.setText("no file selected");
        fileNameLabel.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        jTextField2.setText("33");
        jTextField2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jTextField2ActionPerformed(evt);
            }
        });
        jTextField2.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                jTextField2KeyTyped(evt);
            }
        });

        jButton2.setLabel("||>");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton2ActionPerformed(evt);
            }
        });
        jButton2.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        jLabel1.setText("ms");
        jLabel1.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        jButton3.setText("|<<");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton3ActionPerformed(evt);
            }
        });
        jButton3.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        jButton4.setText("goto");
        jButton4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton4ActionPerformed(evt);
            }
        });

        jTextField1.setText("0");
        jTextField1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jTextField1ActionPerformed(evt);
            }
        });

        jLabel2.setText("min:");
        jLabel2.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        jLabel3.setText("cur: ");
        jLabel3.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        jLabel4.setText("max:");
        jLabel4.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        logSlider.setCursor(new java.awt.Cursor(java.awt.Cursor.DEFAULT_CURSOR));
        logSlider.setEnabled(false);
        logSlider.setPreferredSize(new java.awt.Dimension(480, 23));
        logSlider.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                logSliderStateChanged(evt);
            }
        });
        logSlider.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        openButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Open24.gif"))); // NOI18N
        openButton.setToolTipText("Open");
        openButton.setFocusable(false);
        openButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        openButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        openButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openButtonActionPerformed(evt);
            }
        });
        openButton.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                formKeyTyped(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(fileNameLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jButton3)
                    .addComponent(openButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jButton2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jTextField2, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jButton4)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jTextField1, javax.swing.GroupLayout.PREFERRED_SIZE, 58, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jLabel3, javax.swing.GroupLayout.PREFERRED_SIZE, 70, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 70, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel4, javax.swing.GroupLayout.PREFERRED_SIZE, 76, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(0, 0, Short.MAX_VALUE))
                    .addComponent(logSlider, javax.swing.GroupLayout.DEFAULT_SIZE, 560, Short.MAX_VALUE)))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(openButton, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(logSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(8, 8, 8)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jTextField2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jButton2)
                    .addComponent(jLabel1)
                    .addComponent(jButton3)
                    .addComponent(jButton4)
                    .addComponent(jTextField1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2)
                    .addComponent(jLabel4)
                    .addComponent(jLabel3))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(fileNameLabel))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void openButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openButtonActionPerformed
      if(fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION)
      {
        this.logSlider.setEnabled(false);        
        File openedFile = fileChooser.getSelectedFile();
        if(openedFile != null && openedFile.exists()) 
        {
            try {
                Plugin.parent.getConfig().put(LAST_FILE_PATH_PROPERTY, openedFile.getParentFile().getCanonicalPath());
            } catch (IOException ex) {
                // bah
            }
            
            this.fileNameLabel.setText(openedFile.getAbsolutePath());
            this.logSimulator.openLogFile(openedFile.getAbsolutePath());
            this.minFrame = logSimulator.getMinFrame();
            this.maxFrame = logSimulator.getMaxFrame();           
            this.logSlider.setMinimum(minFrame);
            this.logSlider.setMaximum(maxFrame);
            this.logSlider.setValue(minFrame);
            this.logSimulator.jumpToBegin();
            this.jTextField1.setText("" +minFrame);
            this.jLabel2.setText("min: " +minFrame);
            this.jLabel3.setText("cur: " +minFrame);
            this.jLabel4.setText("max: " +maxFrame);
            this.logSlider.setEnabled(true);
            //this.openButton.setEnabled(false);
            LogfilePlayer.fileName = openedFile.getAbsolutePath();
            connectToSimulator();
        }
      }
    }//GEN-LAST:event_openButtonActionPerformed

    private void logSliderStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_logSliderStateChanged
        Object o = evt.getSource();        
        if (autoSliderChange){
            autoSliderChange = false;
        } else {            
            if (logFileAutoPlayer!=null) {
                 waitTillThreadsDeath();
            }
            this.logSimulator.jumpTo(this.logSlider.getValue());                  
        }        
    }//GEN-LAST:event_logSliderStateChanged

    private void formKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_formKeyTyped
         
    }//GEN-LAST:event_formKeyTyped

    private void jTextField2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jTextField2ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jTextField2ActionPerformed

    private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton2ActionPerformed
        if (verifyStringInt(jTextField2.getText())) {
            jTextField2.setBackground(Color.white);
            if (logFileAutoPlayer == null) {            
                logFileAutoPlayer = new LogfileAutoPlayer(Integer.parseInt(jTextField2.getText()));
                logFileAutoPlayer.start();
            }
            else {
                waitTillThreadsDeath();
            }        
        } else {
            jTextField2.setBackground(Color.red);
        }
        
        
    }//GEN-LAST:event_jButton2ActionPerformed

    private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton3ActionPerformed
         if (logFileAutoPlayer!=null) {
             waitTillThreadsDeath();
         }
         logSimulator.jumpTo(logSimulator.getMinFrame());
    }//GEN-LAST:event_jButton3ActionPerformed

    private void jTextField2KeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_jTextField2KeyTyped
        // TODO add your handling code here:
    }//GEN-LAST:event_jTextField2KeyTyped

    private void jButton4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton4ActionPerformed
        if (verifyStringInt(jTextField1.getText())) {
            int frame = Integer.parseInt(jTextField1.getText());
            if (frame >= minFrame && frame <= maxFrame) { 
                jTextField1.setBackground(Color.white);
                if (logFileAutoPlayer!=null) {
                    waitTillThreadsDeath();
                }
                logSimulator.jumpTo(frame);
                return;
            }            
        } 
        jTextField1.setBackground(Color.red);
    }//GEN-LAST:event_jButton4ActionPerformed

    private void jTextField1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jTextField1ActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jTextField1ActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JFileChooser fileChooser;
    private javax.swing.JLabel fileNameLabel;
    private javax.swing.JButton jButton2;
    private javax.swing.JButton jButton3;
    private javax.swing.JButton jButton4;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JTextField jTextField1;
    private javax.swing.JTextField jTextField2;
    private javax.swing.JSlider logSlider;
    private javax.swing.JButton openButton;
    // End of variables declaration//GEN-END:variables

    
  
 
  
  
    public boolean verifyStringInt(String text) {         
          try {
              int value = Integer.parseInt(text);
              return value>=0;
          } catch (NumberFormatException e) {
              return false;
          }
    }
    
    private void waitTillThreadsDeath() {
      try {
          logFileAutoPlayer.stopPlay();
          logFileAutoPlayer.join();
      } catch (InterruptedException ex) {
          Logger.getLogger(LogfilePlayer.class.getName()).log(Level.SEVERE, null, ex);
          logFileAutoPlayer =  null;
      } finally {
        logFileAutoPlayer =  null;
      }
    }
 
  private void connectToSimulator () {
    if(!Plugin.parent.getMessageServer().isConnected()) {
        Plugin.parent.getMessageServer().connect("localhost", 5401);
    }
  }
  

  private class LogFileFilter extends javax.swing.filechooser.FileFilter
  {
    @Override
    public boolean accept(File file)
    {
      if (file.isDirectory())
      {
        return true;
      }
      String filename = file.getName();
      return filename.toLowerCase().endsWith(".log");
    }

    @Override
    public String getDescription()
    {
      return "Logfile (*.log)";
    }

    @Override
    public String toString()
    {
      return ".log";
    }
  }//end class LogFileFilter
  
    class LogPerceptListener implements LogSimulator.LogSimulatorActionListener 
    {
        @Override
        public void frameChanged(LogSimulator.BlackBoard b, int frameNumber) {
            autoSliderChange = true;
            LogfilePlayer.this.logSlider.setValue(frameNumber);
            LogfilePlayer.this.jLabel3.setText("cur: " +frameNumber);
        }
        
        @Override
        public void logfileOpened(LogSimulator.BlackBoard b, String path) {
        
        }
        
        public void lockInterface() {
            LogfilePlayer.this.setEnabled(false);
            LogfilePlayer.this.setButtons(false);
            
            
        }
        
        public void unLockInterface() {
            LogfilePlayer.this.setEnabled(true);
            LogfilePlayer.this.setButtons(true);
        }
    }
    
    void setButtons (boolean enabled) {
        this.logSlider.setEnabled(enabled);
        this.openButton.setEnabled(enabled);
        this.jButton2.setEnabled(enabled);
        this.jButton3.setEnabled(enabled);
        this.jButton4.setEnabled(enabled);
        this.jTextField1.setEnabled(enabled);
        this.jTextField2.setEnabled(enabled);
    }
}//end class LogfileInspector
