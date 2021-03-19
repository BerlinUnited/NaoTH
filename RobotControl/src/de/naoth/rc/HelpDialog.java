package de.naoth.rc;

import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
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
import org.w3c.dom.NodeList;
import org.w3c.dom.events.Event;
import org.w3c.dom.events.EventListener;
import org.w3c.dom.events.EventTarget;
import org.w3c.dom.html.HTMLFrameElement;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class HelpDialog extends javax.swing.JDialog
{
    private WebView webView;
    private final ArrayList<String> availableHelp = new ArrayList<>();
    private final EventListener linkListener;
    
    /** Creates new form HelpDialog */
    HelpDialog(Frame parent) {
        super(parent);
        
        // set some window attributes
        Dimension defaultSize = new Dimension(800,400);
        this.setPreferredSize(defaultSize);
        this.setSize(defaultSize);
        this.setDefaultCloseOperation(javax.swing.WindowConstants.HIDE_ON_CLOSE);
        this.setTitle("Help");
        this.setModal(false);
        this.setAlwaysOnTop(true);
        this.setVisible(false);
        this.setLocationRelativeTo(parent);
        this.getRootPane().registerKeyboardAction(
            (e) -> { setVisible(false); },
            KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
            JComponent.WHEN_IN_FOCUSED_WINDOW
        );

        // default event handler ("click") for all "a" tags
        linkListener = (Event ev) -> {
            String href = ((Element)ev.getTarget()).getAttribute("href");
            // open url in system browser
            if(Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.BROWSE)) {
                // NOTE: on linux there's java bug, which leads to a crash/freeze of RC in Java 8!
                // bug: https://bugs.openjdk.java.net/browse/JDK-8184155
                // solution: https://stackoverflow.com/questions/23176624/javafx-freeze-on-desktop-openfile-desktop-browseuri
                new Thread(() -> {
                       try {
                           Desktop.getDesktop().browse(new URI(href));
                       } catch (URISyntaxException | IOException ex) {
                           Logger.getLogger(HelpDialog.class.getName()).log(Level.SEVERE, null, ex);
                       }
                   }).start();
            }
            // stop executing "link click"
            ev.preventDefault();
        };
        
        JFXPanel jfxPanel = new JFXPanel();
        this.add(jfxPanel);

        // switch to the JavaFX Application Thread
        Platform.runLater(() -> {
            webView = new WebView();
            webView.setFontSmoothingType(FontSmoothingType.LCD);
            webView.setContextMenuEnabled(false);
            webView.getEngine().load(getClass().getResource("/de/naoth/rc/dialogs/help/index.html").toExternalForm());

            // wait for the index page to be fully loaded
            webView.getEngine().getLoadWorker().stateProperty().addListener((ObservableValue<? extends Worker.State> ov, Worker.State t, Worker.State t1) -> {
                Document doc = webView.getEngine().getDocument();
                if(doc != null) {
                    // generate the table of contents in the nav frame
                    Document navDoc = ((HTMLFrameElement) doc.getElementById("navigation")).getContentDocument();
                    createHelpToc(navDoc);
                    
                    // registers the link listener to all "a" tags of the newly loaded content frame
                    ((EventTarget) doc.getElementById("content")).addEventListener("load", (Event evt) -> {
                        registerLinkListener(((HTMLFrameElement) evt.getTarget()).getContentDocument());
                    }, false);
                }
            });

            jfxPanel.setScene(new Scene(webView));
        });
    }
    
    /**
     * Generates the table of content based on the available help files.
     * Filters out files starting with "index" and only use ".html" files.
     * 
     * @param doc the document, where the TOC should be appended to
     */
    private void createHelpToc(Document doc)
    {
        Helper.getFiles("/de/naoth/rc/dialogs/help/").forEach((t) -> {
            String fileName = t.getFileName().toString();
            if (!fileName.startsWith("index") && fileName.endsWith(".html")) {
                createHelpTocEntry(doc, fileName);
                availableHelp.add(fileName.substring(0, fileName.length() - 5)); // remove ".html"
            }
        });
    }
    
    /**
     * Creates a TOC entry for the given help file.
     * <li><a href="..." target="content">...</a></li>
     * 
     * @param doc   the document, where the TOC entry should be appended to
     * @param file  the name of the HTML help file
     */
    private void createHelpTocEntry(Document doc, String file) {
        Element a = doc.createElement("a");
        a.setAttribute("href", file);
        a.setAttribute("target", "content");
        a.setTextContent(file.substring(0, file.length() - 5)); // remove ".html"
        
        Element li = doc.createElement("li");
        li.insertBefore(a, null);
        
        doc.getElementById("toc").appendChild(li);
    }
    
    /**
     * Registers the link listener to all "a" tags of a given document.
     * 
     * @param doc the document, where the link listener should be registered
     */
    private void registerLinkListener(Document doc) {
        NodeList l = doc.getElementsByTagName("a");
        for (int i = 0; i < l.getLength(); i++) {
            ((EventTarget) l.item(i)).addEventListener("click", linkListener, false);
        }
    }

    /**
     * Shows the help window for the given help/dialog name.
     * 
     * @param name name of the dialog respectively help file
     */
    public void showHelp(String name)
    {
        // switch to the fx thread
        Platform.runLater(() -> {
            Document doc = webView.getEngine().getDocument();
            HTMLFrameElement frame = (HTMLFrameElement) doc.getElementById("content");
            if (name == null) {
                frame.setSrc("index_rc.html");
            } else if (!availableHelp.contains(name)) {
                frame.setSrc("index_404.html");
            } else {
                frame.setSrc(name + ".html");
            }
            this.setVisible(true);
            this.requestFocus();
        });
    }//end show

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
}
