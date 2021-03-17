/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * RepresentationInspector.java
 *
 * Created on 09.04.2009, 14:33:07
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.core.messages.FrameworkRepresentations;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.messages.Representations;
import de.naoth.rc.logmanager.BlackBoard;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.logmanager.LogFrameListener;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.core.server.Command;
import javax.swing.DefaultListModel;
import javax.swing.JPanel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

import java.awt.Color;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.lang.reflect.Method;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.SwingUtilities;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultHighlighter;
import javax.swing.text.Highlighter;

/**
 *
 * @author Heinrich Mellmann
 */
public class RepresentationInspector extends AbstractDialog {

    @RCDialog(category = RCDialog.Category.Status, name = "Representations")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<RepresentationInspector> {

        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static GenericManagerFactory genericManagerFactory;
        @InjectPlugin
        public static SwingCommandExecutor commandExecutor;
        @InjectPlugin
        public static LogFileEventManager logFileEventManager;
    }

    private String representationOwner;

    private String getRepresentationList() {
        return representationOwner + ":representation:list";
    }

    private String getRepresentationPrint() {
        return representationOwner + ":representation:print";
    }

    private String getRepresentationBinary() {
        return representationOwner + ":representation:get";
    }

    private ObjectListener<byte[]> currentHandler;
    private GenericManager currentManager;

    private final LogRepresentationListener logHandler = new LogRepresentationListener();
    private final Highlighter.HighlightPainter searchHighlighter = new DefaultHighlighter.DefaultHighlightPainter(Color.LIGHT_GRAY);
    private final Highlighter.HighlightPainter searchHighlighterActive = new DefaultHighlighter.DefaultHighlightPainter(Color.ORANGE);

    public RepresentationInspector() {
        initComponents();
        searchField.setVisible(false);
        this.representationOwner = this.cbProcess.getSelectedItem().toString();
    }

    @Override
    public void init() {
        this.representationList.addListSelectionListener(new ListSelectionListener() {
            @Override
            public void valueChanged(ListSelectionEvent e) {
                if (!e.getValueIsAdjusting()) {
                    Object o = representationList.getSelectedValue();
                    if (currentManager != null) {
                        currentManager.removeListener(currentHandler);
                    }

                    if (o != null) {
                        String prefix = null;

                        if (jToggleButtonLog.isSelected()) {
                            logHandler.showFrame((String) o);
                        } else {
                            if (jToggleButtonBinary.isSelected()) {
                                prefix = getRepresentationBinary();
                                currentHandler = new DataHandlerBinary((String) o);
                            } else {
                                prefix = getRepresentationPrint();
                                currentHandler = new DataHandlerPrint();
                            }

                            Command cmd = new Command(prefix).addArg((String) o);
                            currentManager = Plugin.genericManagerFactory.getManager(cmd);
                            currentManager.addListener(currentHandler);
                        }
                    }
                }//end if
            }//end valueChanged
        });
    }//end init

    @Override
    public JPanel getPanel() {
        return this;
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jToolBar1 = new javax.swing.JToolBar();
        jToggleButtonRefresh = new javax.swing.JToggleButton();
        jToggleButtonBinary = new javax.swing.JToggleButton();
        jToggleButtonLog = new javax.swing.JToggleButton();
        cbProcess = new javax.swing.JComboBox();
        jSplitPane1 = new javax.swing.JSplitPane();
        jScrollPane1 = new javax.swing.JScrollPane();
        representationList = new javax.swing.JList();
        jPanel1 = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        textArea = new javax.swing.JTextArea();
        searchField = new javax.swing.JTextField();

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

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

        jToggleButtonBinary.setText("Binary");
        jToggleButtonBinary.setFocusable(false);
        jToggleButtonBinary.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jToggleButtonBinary.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToggleButtonBinary.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButtonBinaryActionPerformed(evt);
            }
        });
        jToolBar1.add(jToggleButtonBinary);

        jToggleButtonLog.setText("Log");
        jToggleButtonLog.setFocusable(false);
        jToggleButtonLog.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jToggleButtonLog.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToggleButtonLog.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButtonLogActionPerformed(evt);
            }
        });
        jToolBar1.add(jToggleButtonLog);

        cbProcess.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Cognition", "Motion" }));
        cbProcess.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbProcessActionPerformed(evt);
            }
        });
        jToolBar1.add(cbProcess);

        jSplitPane1.setBorder(null);

        jScrollPane1.setBorder(null);

        representationList.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        jScrollPane1.setViewportView(representationList);

        jSplitPane1.setLeftComponent(jScrollPane1);

        jPanel1.setLayout(new java.awt.BorderLayout());

        jScrollPane2.setBorder(null);

        textArea.setColumns(20);
        textArea.setFont(new java.awt.Font("Monospaced", 0, 14)); // NOI18N
        textArea.setRows(5);
        textArea.setBorder(null);
        textArea.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                textAreaKeyPressed(evt);
            }
        });
        jScrollPane2.setViewportView(textArea);

        jPanel1.add(jScrollPane2, java.awt.BorderLayout.CENTER);

        searchField.setOpaque(false);
        searchField.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                searchFieldKeyPressed(evt);
            }
        });
        jPanel1.add(searchField, java.awt.BorderLayout.SOUTH);

        jSplitPane1.setRightComponent(jPanel1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 400, Short.MAX_VALUE)
            .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(jSplitPane1))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jToggleButtonRefreshActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButtonRefreshActionPerformed
        if (jToggleButtonRefresh.isSelected()) {
            if (Plugin.parent.checkConnected()) {
                Plugin.commandExecutor.executeCommand(new RepresentationListUpdater(), new Command(getRepresentationList()));
            } else {
                jToggleButtonRefresh.setSelected(false);
            }
        } else {
            dispose();
        }
}//GEN-LAST:event_jToggleButtonRefreshActionPerformed

    private void jToggleButtonBinaryActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jToggleButtonBinaryActionPerformed
    {//GEN-HEADEREND:event_jToggleButtonBinaryActionPerformed
        if (currentManager != null) {
            currentManager.removeListener(currentHandler);
        }
        int i = representationList.getSelectedIndex();
        representationList.clearSelection();
        representationList.setSelectedIndex(i);
    }//GEN-LAST:event_jToggleButtonBinaryActionPerformed

    private void cbProcessActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbProcessActionPerformed
    {//GEN-HEADEREND:event_cbProcessActionPerformed
        dispose();
        representationOwner = this.cbProcess.getSelectedItem().toString();
    }//GEN-LAST:event_cbProcessActionPerformed

  private void jToggleButtonLogActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jToggleButtonLogActionPerformed
  {//GEN-HEADEREND:event_jToggleButtonLogActionPerformed

      // delete old model
      DefaultListModel model = new DefaultListModel();
      representationList.setModel(model);
      if (jToggleButtonLog.isEnabled()) {
          Plugin.logFileEventManager.addListener(logHandler);
      } else {
          Plugin.logFileEventManager.removeListener(logHandler);
      }
  }//GEN-LAST:event_jToggleButtonLogActionPerformed

    private void textAreaKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_textAreaKeyPressed
        // Ctrl+F -> search
        if ((evt.getKeyCode() == KeyEvent.VK_F) && ((evt.getModifiers() & KeyEvent.CTRL_MASK) != 0)) {
            if(this.searchField.isVisible()) {
                this.searchField.selectAll();
            } else {
                this.searchField.setVisible(true);
                this.revalidate();
            }
            this.searchField.requestFocus();
        }
    }//GEN-LAST:event_textAreaKeyPressed

    private void searchFieldKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_searchFieldKeyPressed
        if(evt.getKeyCode() == KeyEvent.VK_ESCAPE) {
            // hide & reset textarea
            this.searchField.setBackground(Color.white);
            textArea.getHighlighter().removeAllHighlights();
            this.searchField.setVisible(false);
            this.textArea.requestFocus();
            this.revalidate();
        } else if(evt.getKeyCode() == KeyEvent.VK_ENTER) {
            // search (next) occurrence of search string
            String search = this.searchField.getText().trim();
            if(!search.isEmpty()) {
                int offset = textArea.getText().indexOf(search);
                int offset_last = textArea.getText().lastIndexOf(search);
                int offset_cursor = textArea.getCaretPosition() > offset_last ? 0 : textArea.getCaretPosition();
                
                // indicate not found
                if(offset >= 0) {
                    this.searchField.setBackground(Color.white);
                } else {
                    this.searchField.setBackground(Color.red);
                }
                
                // clear previous search highlights
                textArea.getHighlighter().removeAllHighlights();
                // set default highlighter
                Highlighter.HighlightPainter highlighter = searchHighlighter;
                // highlight every occurrence
                while (offset != -1) {                    
                    try {
                        // highlight the current search result differently
                        if((offset_cursor >= 0 && offset >= offset_cursor) && highlighter == searchHighlighter) {
                            highlighter = searchHighlighterActive;
                            textArea.setCaretPosition(offset + search.length());
                            offset_cursor = -1; // make sure there's only one 'current' highlight
                        } else if(highlighter == searchHighlighterActive) {
                            highlighter = searchHighlighter;
                        }
                        // highlight & next
                        textArea.getHighlighter().addHighlight(offset, offset + search.length() , highlighter);
                        offset = textArea.getText().indexOf(search, offset+search.length());
                    } catch (BadLocationException ex) {}
                }
            }
        }
    }//GEN-LAST:event_searchFieldKeyPressed
  
    class DataHandlerBinary implements ObjectListener<byte[]> {

        private final Parser parser;
        //private final List<Descriptor> allDescriptors = getAllProtobufDescriptors();

        public DataHandlerBinary(String name) {
            
            Class<?> parserClass = getProtobufClass(name);
            if (parserClass != null) {
                parser = new ProtobufParser(parserClass);
            } else {
                parser = new EmptyParser();
            }
        }

        @Override
        public void newObjectReceived(byte[] data) {

            Object result = parser.parse(data);
            if (result != null) {
                textArea.setText(result.toString());
            } else {
                textArea.setText("Error while parsing.\n");
                //textArea.append(new String(data));
            }

        }//end newObjectReceived

        @Override
        public void errorOccured(String cause) {
            handleError(cause);
        }
    }//end class DataHandlerPrint

    class DataHandlerPrint implements ObjectListener<byte[]> {

        @Override
        public void newObjectReceived(byte[] object) {
            textArea.setText(new String(object));
        }

        @Override
        public void errorOccured(String cause) {
            handleError(cause);
        }
    }//end class DataHandlerPrint

    class RepresentationListUpdater implements ObjectListener<byte[]> {

        @Override
        public void newObjectReceived(byte[] object) {
            //System.out.println(new String(result));
            String lastSelection = (String) representationList.getSelectedValue();
            String[] representations = new String(object).split("\n");

            DefaultListModel model = new DefaultListModel();
            for (String representation : representations) {
                model.addElement(representation);
            }
            representationList.setModel(model);
            representationList.setSelectedValue(lastSelection, true);
        }

        @Override
        public void errorOccured(String cause) {
            jToggleButtonRefresh.setSelected(false);
            dispose();
        }
    }//end RepresentationListUpdater

    private class LogRepresentationListener implements LogFrameListener {

        private final Map<String, byte[]> dataByName = Collections.synchronizedMap(new HashMap<String, byte[]>());
        
        @Override
        public void newFrame(BlackBoard b) {
            dataByName.clear();
            DefaultListModel model = representationList.getModel() instanceof DefaultListModel ? (DefaultListModel)representationList.getModel() : new DefaultListModel();
            String selected = representationList.getSelectedValue() instanceof String ? (String)representationList.getSelectedValue() : null;
            
            for (String name : b.getNames()) {
                if (!model.contains(name)) {
                    model.addElement(name);
                }
                dataByName.put(name, b.get(name).getData());
                
                if(selected != null && selected.equals(name)) {
                    showFrame(selected);
                }
            }
        }
        
        public void showFrame(String representation) {
            Class<?> parserClass = getProtobufClass(representation);
            Parser parser;
            if (parserClass != null) {
                parser = new ProtobufParser(parserClass);
            } else {
                parser = new EmptyParser();
            }
            
            byte[] data = dataByName.get(representation);
            if (data != null) {
                Object result = parser.parse(data);
                if (result != null) {
                    // save last scrollbar position (viewport)
                    final Point scrollPosition = jScrollPane2.getViewport().getViewPosition();
                    // set "new" representation string
                    textArea.setText(result.toString());
                    // restore last scrollbar position (viewport)
                    SwingUtilities.invokeLater(() -> {
                        jScrollPane2.getViewport().setViewPosition(scrollPosition);
                    });
                } else {
                    textArea.setText("Error while parsing.\n");
                }
            } else {
                textArea.setText("No data available");
            }
        }
    }

    public void handleError(String cause) {
        jToggleButtonRefresh.setSelected(false);
        if (currentManager != null) {
            currentManager.removeListener(currentHandler);
        }
    }

    @Override
    public void dispose() {
        if (currentManager != null) {
            currentManager.removeListener(currentHandler);
        }
        this.jToggleButtonRefresh.setSelected(false);
    }

    /*
    private static List<Descriptor> getAllProtobufDescriptors() {
        List<Descriptor> result = new ArrayList<Descriptor>();
        result.addAll(FrameworkRepresentations.getDescriptor().getMessageTypes());
        result.addAll(Representations.getDescriptor().getMessageTypes());
        result.addAll(TeamMessageOuterClass.getDescriptor().getMessageTypes());
        result.addAll(Messages.getDescriptor().getMessageTypes());
        result.addAll(AudioDataOuterClass.getDescriptor().getMessageTypes());
        return result;
    }

    private static List<Class<?>> getAllProtobufClasses() {
        List<Class<?>> result = new ArrayList<Class<?>>();
        Class<?> protoClasses[] = {
            FrameworkRepresentations.class,
            Representations.class,
            TeamMessageOuterClass.class,
            Messages.class};

        for (Class<?> c : protoClasses) {
            result.addAll(Arrays.asList(c.getClasses()));
        }
        return result;
    }
	*/
	
    private static Class<?> getProtobufClass(String name) {
        
        // EVIL HACK: remove suffix "Top"
        if (name.endsWith("Top")) {
            name = name.substring(0, name.length() - 3);
        }
        
        // look in the general message classes first
        Class<?> protoClasses[] = {
            FrameworkRepresentations.class,
            Representations.class,
            Messages.class};

        for (Class<?> pc : protoClasses) {
            for (Class<?> c : pc.getClasses()) {
                if (c.getSimpleName().equals(name)) {
                    return c;
                }
            }
        }
        
        // if nothing found, then look in the individual message classes
        String protoPackage = Representations.class.getPackage().getName();
        try {
            // the protobuf message classes are inside an outer class, 
            // so we cave to use the operator $ to access the inner class
            // e.g.: RobotPoseOuterClass$RobotPose
            return Class.forName(protoPackage+"."+name+"OuterClass$"+name);
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(RepresentationInspector.class.getName()).log(Level.WARNING, "Could not find a protobuf class for " + name, ex);
            return null;
        }
    }

    interface Parser {
        public Object parse(byte[] object);
    }

    class EmptyParser implements Parser {

        @Override
        public Object parse(byte[] object) {
            return "No binary serialization avaliable.";
        }
    }

    class ProtobufParser implements Parser {

        //private Descriptor descriptor;

        private final Class<?> parserClass;

        public ProtobufParser(Class<?> parserClass) {
            this.parserClass = parserClass;
        }
        /*
         @Override
         public Object test(byte[] data)
         {
         try {
         DynamicMessage msg = DynamicMessage.parseFrom(descriptor, data);
         return msg;
         }
         catch(InvalidProtocolBufferException ex) {}
            
         return null;
         }//end parse
         */

        @Override
        public Object parse(byte[] data) {
            try {
                //DynamicMessage msg = DynamicMessage.parseFrom(descriptor, data);
                Method m = parserClass.getMethod("parseFrom", data.getClass());
                Object msg = m.invoke(null, data);
                return msg;
            } catch (Exception e) {
                getLogger().log(Level.SEVERE, "Error while trying to parse a message", e);
            } 

            return null;
        }//end parse
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JComboBox cbProcess;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JToggleButton jToggleButtonBinary;
    private javax.swing.JToggleButton jToggleButtonLog;
    private javax.swing.JToggleButton jToggleButtonRefresh;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JList representationList;
    private javax.swing.JTextField searchField;
    private javax.swing.JTextArea textArea;
    // End of variables declaration//GEN-END:variables

}
