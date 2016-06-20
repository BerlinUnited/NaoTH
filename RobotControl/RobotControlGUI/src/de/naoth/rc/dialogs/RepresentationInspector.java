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

import com.google.protobuf.Descriptors.Descriptor;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.logmanager.BlackBoard;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.logmanager.LogFrameListener;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.server.Command;
import javax.swing.DefaultListModel;
import javax.swing.JPanel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

import de.naoth.rc.messages.FrameworkRepresentations;
import de.naoth.rc.messages.Messages;
import de.naoth.rc.messages.Representations;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.swing.ListModel;

/**
 *
 * @author Heinrich Mellmann
 */
public class RepresentationInspector extends AbstractDialog {

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

    private String getRepresentationBase() {
        return representationOwner + ":representation:get";
    }

    private String getRepresentationBinary() {
        return representationOwner + ":representation:getbinary";
    }

    private ObjectListener<byte[]> currentHandler;
    private GenericManager currentManager;

    private final LogRepresentationListener logHandler = new LogRepresentationListener();

    public RepresentationInspector() {
        initComponents();

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
                                prefix = getRepresentationBase();
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
        jScrollPane2 = new javax.swing.JScrollPane();
        textArea = new javax.swing.JTextArea();

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

        jScrollPane2.setBorder(null);

        textArea.setColumns(20);
        textArea.setFont(new java.awt.Font("Monospaced", 0, 14)); // NOI18N
        textArea.setRows(5);
        textArea.setBorder(null);
        jScrollPane2.setViewportView(textArea);

        jSplitPane1.setRightComponent(jScrollPane2);

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
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 275, Short.MAX_VALUE))
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
  
    class DataHandlerBinary implements ObjectListener<byte[]> {

        private final Parser parser;
        private final List<Descriptor> allDescriptors = getAllProtobufDescriptors();

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
            String[] representations = new String(object).split("\n");

            DefaultListModel model = new DefaultListModel();
            for (String representation : representations) {
                model.addElement(representation);
            }
            representationList.setModel(model);
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
            Set<String> names = b.getNames();
            for (String n : names) {
                LogDataFrame frame = b.get(n);
                
                ListModel model = representationList.getModel();
                if(model instanceof DefaultListModel) {
                    if (!((DefaultListModel) model).contains(frame.getName())) {
                        ((DefaultListModel) model).addElement(frame.getName());
                    }
                }
                dataByName.put(frame.getName(), frame.getData());
                
                Object selected = representationList.getSelectedValue();
                if(selected instanceof String) {
                    showFrame((String) selected);
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
                    textArea.setText(result.toString());
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

    private static List<Descriptor> getAllProtobufDescriptors() {
        List<Descriptor> result = new ArrayList<Descriptor>();
        result.addAll(FrameworkRepresentations.getDescriptor().getMessageTypes());
        result.addAll(Representations.getDescriptor().getMessageTypes());
        result.addAll(Messages.getDescriptor().getMessageTypes());
        return result;
    }

    private static List<Class<?>> getAllProtobufClasses() {
        List<Class<?>> result = new ArrayList<Class<?>>();
        Class<?> protoClasses[] = {
            FrameworkRepresentations.class,
            Representations.class,
            Messages.class};

        for (Class<?> c : protoClasses) {
            result.addAll(Arrays.asList(c.getClasses()));
        }
        return result;
    }

    private static Class<?> getProtobufClass(String name) {
        
        // EVIL HACKS
        if (name.endsWith("Top")) {
            name = name.substring(0, name.length() - 3);
        } else if("TeamMessage".equals(name)) {
            name = "TeamMessageCollection";
        }
        
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

        return null;
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
            } catch (NoSuchMethodException e) {
            } catch (IllegalAccessException ex) {
            } catch (InvocationTargetException ebx) {
            }

            return null;
        }//end parse
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JComboBox cbProcess;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JToggleButton jToggleButtonBinary;
    private javax.swing.JToggleButton jToggleButtonLog;
    private javax.swing.JToggleButton jToggleButtonRefresh;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JList representationList;
    private javax.swing.JTextArea textArea;
    // End of variables declaration//GEN-END:variables

}
