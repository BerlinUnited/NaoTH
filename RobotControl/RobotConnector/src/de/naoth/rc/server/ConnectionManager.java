
package de.naoth.rc.server;

import java.awt.Frame;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Properties;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.swing.JDialog;
import javax.swing.JOptionPane;

/**
 *
 * @author Heinrich Mellmann
 */
public class ConnectionManager
{
  private final MessageServer messageServer;
  private final Properties properties;
  private final ExecutorService executor = Executors.newCachedThreadPool();
  
  Frame parent;  
  final IPInput ipInput = new IPInput();
  final JOptionPane optionPane;
  final JDialog dialog;
  
  public ConnectionManager(Frame parent, MessageServer messageServer)
  {
      this(parent, messageServer, new Properties());
  }
          
  public ConnectionManager(Frame parent, MessageServer messageServer, Properties properties)
  {
    this.messageServer = messageServer;
    this.properties = properties;
    this.parent = parent;
    
    Object[] options = { "Connect", "Cancel" };
    this.optionPane = new JOptionPane(ipInput, JOptionPane.PLAIN_MESSAGE, JOptionPane.OK_CANCEL_OPTION, null, options, options[0]){
        @Override
        public int getMaxCharactersPerLineCount() {
            return 60;
        }
    };
    
    PropertyChangeListener optionPropertyListener = new PropertyChangeListener() 
    {
        @Override
        public void propertyChange(PropertyChangeEvent e) {
            
            Object value = e.getNewValue();
            if(value == null || value.equals(JOptionPane.UNINITIALIZED_VALUE) || optionPane.getParent() == null) {
                return;
            }
            
            String prop = e.getPropertyName();
            
            if (optionPane.getParent().isVisible() 
             && (e.getSource() == optionPane)
             && (prop.equals(JOptionPane.VALUE_PROPERTY))) 
            {
                if(value instanceof Integer && ((Integer)value) == JOptionPane.CLOSED_OPTION) {
                    dialog.setVisible(false);
                }
                    
                if(value.equals("Cancel")) {
                    dialog.setVisible(false);
                }

                if(value.equals("Connect")) {
                    // try to connect
                    messageServer.connect(ipInput.getAddress(), ipInput.getPort());
                }
                
                optionPane.setValue(JOptionPane.UNINITIALIZED_VALUE);
            }
        }
    };
    
    this.optionPane.addPropertyChangeListener(optionPropertyListener);
    this.dialog = optionPane.createDialog(parent, "Connect");
    
    // HACK: remove the last change listener which is responsible for closing the dialog
    PropertyChangeListener[] p = optionPane.getPropertyChangeListeners();
    this.optionPane.removePropertyChangeListener(p[p.length-1]);
    
    this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() 
    {
        @Override
        public void connected(ConnectionStatusEvent event) {
            ipInput.addAddress(event.getAddress().getHostString());

            properties.put("hostname", event.getAddress().getHostString());
            properties.put("port", "" + event.getAddress().getPort());
            dialog.setVisible(false);
        }

        @Override
        public void disconnected(ConnectionStatusEvent event) {
            setMessage(event.getMessage());
        }
    });
  }
  
  private void setMessage(String message) {
      if(message == null) {
          optionPane.setMessage(ipInput);
          optionPane.setMessageType(JOptionPane.PLAIN_MESSAGE);
      } else {
          optionPane.setMessage(new Object[]{String.format("<html><body>%s",message), ipInput});
          optionPane.setMessageType(JOptionPane.WARNING_MESSAGE);
      }
      dialog.pack();
  }
  
  public void showConnectionDialog(String message) {
      setMessage(message);
      dialog.setVisible(true);
  }
  
  public void showConnectionDialog() {
      setMessage(null);
      dialog.setVisible(true);
  }
  
}
