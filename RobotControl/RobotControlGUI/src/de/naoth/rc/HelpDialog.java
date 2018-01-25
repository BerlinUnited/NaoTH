/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * HelpDialog.java
 *
 * Created on 21.09.2009, 14:51:01
 */

package de.naoth.rc;

import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.concurrent.Worker;
import javafx.embed.swing.JFXPanel;
import javafx.scene.Scene;
import javafx.scene.text.FontSmoothingType;
import javafx.scene.web.WebView;
import javax.swing.JComponent;
import javax.swing.KeyStroke;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.events.Event;
import org.w3c.dom.events.EventListener;
import org.w3c.dom.events.EventTarget;
import org.w3c.dom.html.HTMLAnchorElement;

/**
 *
 * @author admin
 */
public class HelpDialog extends javax.swing.JDialog {

    private Frame parent = null;
    
    /** Creates new form HelpDialog */
    public HelpDialog(java.awt.Frame parent, boolean modal, String helpText) {
        super(parent, modal);
        initComponents();
        
        this.parent = parent;
        
        this.helpPanel.setText(helpText);

        this.helpPanel.setCaretPosition(0);
        
        ActionListener actionListener = new ActionListener() {
          public void actionPerformed(ActionEvent actionEvent) {
            dispose();
          }
        };
    
        KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
        this.getRootPane().registerKeyboardAction(actionListener, stroke, JComponent.WHEN_IN_FOCUSED_WINDOW);
    }

    HelpDialog(Frame parent, boolean b, URL res) {
        Dimension defaultSize = new Dimension(400,300);
        this.setPreferredSize(defaultSize);
        this.setSize(defaultSize);
        this.parent = parent;
        
        JFXPanel jfxPanel = new JFXPanel();
        this.add(jfxPanel);

        // Creation of scene and future interactions with JFXPanel
        // should take place on the JavaFX Application Thread
        Platform.runLater(() -> {
            WebView webView = new WebView();
            webView.setFontSmoothingType(FontSmoothingType.LCD);
            webView.setContextMenuEnabled(false);
            jfxPanel.setScene(new Scene(webView));
            if(res == null) {
                webView.getEngine().loadContent("For this dialog is no help avaliable.");
            } else {
                webView.getEngine().load(res.toExternalForm());
                // default event handler ("click") for all "a" tags
                EventListener listener = (Event ev) -> {
                    String href = ((Element)ev.getTarget()).getAttribute("href");
                    // open url in system browser
                    if(Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.BROWSE)) {
                        // NOTE: on linux there's java bug, which leads to a crash/freeze of RC in Java 8!
                        // bug: https://bugs.openjdk.java.net/browse/JDK-8184155
                        // solution: https://stackoverflow.com/questions/23176624/javafx-freeze-on-desktop-openfile-desktop-browseuri
                        new Thread(() -> {
                               try {
                                   Desktop.getDesktop().browse(new URI(href));
                               } catch (IOException | URISyntaxException ex) {
                                   Logger.getLogger(HelpDialog.class.getName()).log(Level.SEVERE, null, ex);
                               }
                           }).start();
                    }
                    // stop executing "link click"
                    ev.preventDefault();
                };
                // listen to document events
                webView.getEngine().getLoadWorker().stateProperty().addListener((ObservableValue<? extends Worker.State> ov, Worker.State t, Worker.State t1) -> {
                    // register listener to all "a" tags
                    Document doc = webView.getEngine().getDocument();
                    if(doc != null) {
                        NodeList l = doc.getElementsByTagName("a");
                        for (int i = 0; i < l.getLength(); i++) {
                            ((EventTarget) l.item(i)).addEventListener("click", listener, false);
                        }
                    }
                });
            }
        });
        
        ActionListener actionListener = new ActionListener() {
          public void actionPerformed(ActionEvent actionEvent) {
              setVisible(false);
          }
        };
    
        KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
        this.getRootPane().registerKeyboardAction(actionListener, stroke, JComponent.WHEN_IN_FOCUSED_WINDOW);
    }

    public void showHelp()
    {
      if(!this.isVisible())
      {
        Point location = this.parent.getLocation();
        location.translate(100, 100);
        this.setLocation(location);
      }//end if

      this.setVisible(true);
      this.requestFocus();
    }//end show

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jScrollPane1 = new javax.swing.JScrollPane();
    helpPanel = new javax.swing.JEditorPane();

    setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

    helpPanel.setContentType("text/html");
    helpPanel.setEditable(false);
    jScrollPane1.setViewportView(helpPanel);

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 400, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 300, Short.MAX_VALUE)
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                HelpDialog dialog = new HelpDialog(new javax.swing.JFrame(), true, "");
                dialog.addWindowListener(new java.awt.event.WindowAdapter() {
                    public void windowClosing(java.awt.event.WindowEvent e) {
                        System.exit(0);
                    }
                });
                dialog.setVisible(true);
            }
        });
    }

    


  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JEditorPane helpPanel;
  private javax.swing.JScrollPane jScrollPane1;
  // End of variables declaration//GEN-END:variables

}
