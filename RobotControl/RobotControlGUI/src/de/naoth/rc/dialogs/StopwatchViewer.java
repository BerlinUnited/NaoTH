/*
 * StopwatchViewer.java
 *
 * Created on 18. April 2008, 20:25
 */
package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.SimpleProgressBar;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.messages.Messages;
import de.naoth.rc.core.server.Command;
import java.awt.Color;
import java.awt.Component;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTable;
import javax.swing.SortOrder;
import javax.swing.SwingUtilities;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableModel;
import javax.swing.table.TableRowSorter;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  thomas
 */
public class StopwatchViewer extends AbstractDialog
{

  @RCDialog(category = RCDialog.Category.Status, name = "Stopwatch")
  @PluginImplementation
  public static class Plugin extends DialogPlugin<StopwatchViewer>
  {
    @InjectPlugin
    static public RobotControl parent;
    @InjectPlugin
    public static GenericManagerFactory genericManagerFactory;
  }
  
  private final Color normalColor = new Color(100,200,255);
  private final Color warnColor = new Color(255,150,150);

  final StopWatchListener stopWatchListener = new StopWatchListener();
  GenericManager currentManager = null;
  float globalWarnLevel = 40.0f;
  
  public StopwatchViewer()
  {
    initComponents();
    
    //stopwatchTable.getTableHeader().setC
    stopwatchTable.setModel(new StopwatchTableModel());
    stopwatchTable.getTableHeader().setReorderingAllowed(false);
    
    for (int i = 1; i < stopwatchTable.getColumnModel().getColumnCount(); i++) {
        stopwatchTable.getColumnModel().getColumn(i).setCellRenderer(new StopwatchCellRenderer());
    }
    
    // sort all columns in the deccending order except for the first column
    TableRowSorter<TableModel> sorter = new TableRowSorter<TableModel>(stopwatchTable.getModel()){
      @Override
      public void toggleSortOrder(int column) { 
          if(column == 0) {
              super.toggleSortOrder(column);
              return;
          }
              
        List<SortKey> newKeys = new ArrayList<SortKey>();
        newKeys.add(new SortKey(column, SortOrder.DESCENDING));
        setSortKeys(newKeys);
      } 
    };
    stopwatchTable.setRowSorter(sorter);
    // HACK: column 2 is the average
    sorter.toggleSortOrder(2);
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jScrollPane = new javax.swing.JScrollPane();
        jScrollPane1 = new javax.swing.JScrollPane();
        stopwatchTable = new javax.swing.JTable();
        jToolBar1 = new javax.swing.JToolBar();
        btShowStopwatch = new javax.swing.JToggleButton();
        cbProcess = new javax.swing.JComboBox();
        btReset = new javax.swing.JButton();
        cbAutoSort = new javax.swing.JCheckBox();
        cbModulesOnly = new javax.swing.JCheckBox();
        jLabel1 = new javax.swing.JLabel();
        txtWarn = new javax.swing.JFormattedTextField();
        jLabel2 = new javax.swing.JLabel();

        panelStopwatches.setBackground(new java.awt.Color(255, 255, 255));
        panelStopwatches.setLayout(new java.awt.BorderLayout());

        stopwatchTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        jScrollPane1.setViewportView(stopwatchTable);

        panelStopwatches.add(jScrollPane1, java.awt.BorderLayout.CENTER);

        jScrollPane.setViewportView(panelStopwatches);

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        btShowStopwatch.setMnemonic('s');
        btShowStopwatch.setText("Show stopwatches");
        btShowStopwatch.setFocusable(false);
        btShowStopwatch.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btShowStopwatch.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btShowStopwatch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btShowStopwatchActionPerformed(evt);
            }
        });
        jToolBar1.add(btShowStopwatch);

        cbProcess.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Cognition", "Motion" }));
        cbProcess.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbProcessActionPerformed(evt);
            }
        });
        jToolBar1.add(cbProcess);

        btReset.setMnemonic('r');
        btReset.setText("Reset");
        btReset.setFocusable(false);
        btReset.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReset.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btResetActionPerformed(evt);
            }
        });
        jToolBar1.add(btReset);

        cbAutoSort.setText("Coninuous Sort");
        cbAutoSort.setFocusable(false);
        cbAutoSort.setHorizontalTextPosition(javax.swing.SwingConstants.LEADING);
        cbAutoSort.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToolBar1.add(cbAutoSort);

        cbModulesOnly.setText("modules only");
        cbModulesOnly.setFocusable(false);
        cbModulesOnly.setHorizontalTextPosition(javax.swing.SwingConstants.LEADING);
        cbModulesOnly.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        cbModulesOnly.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbModulesOnlyActionPerformed(evt);
            }
        });
        jToolBar1.add(cbModulesOnly);

        jLabel1.setText("warn:");
        jToolBar1.add(jLabel1);

        txtWarn.setColumns(2);
        txtWarn.setFormatterFactory(new javax.swing.text.DefaultFormatterFactory(new javax.swing.text.NumberFormatter(new java.text.DecimalFormat("#0"))));
        txtWarn.setText("40");
        txtWarn.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                txtWarnActionPerformed(evt);
            }
        });
        jToolBar1.add(txtWarn);

        jLabel2.setText(" ms");
        jToolBar1.add(jLabel2);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jScrollPane)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 269, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

private void btShowStopwatchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btShowStopwatchActionPerformed
    registerListeners();
}//GEN-LAST:event_btShowStopwatchActionPerformed

private void btResetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btResetActionPerformed
  resetAll();
}//GEN-LAST:event_btResetActionPerformed


  private void cbModulesOnlyActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbModulesOnlyActionPerformed
  {//GEN-HEADEREND:event_cbModulesOnlyActionPerformed
    registerListeners();
  }//GEN-LAST:event_cbModulesOnlyActionPerformed

    private void cbProcessActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbProcessActionPerformed
        registerListeners();
    }//GEN-LAST:event_cbProcessActionPerformed

    private void txtWarnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_txtWarnActionPerformed
        try {
          globalWarnLevel = Math.max(Float.parseFloat(txtWarn.getText()), 1.0f);
          stopwatchTable.repaint();
        } catch(NumberFormatException ex) {
          ex.printStackTrace(System.err);
        }
    }//GEN-LAST:event_txtWarnActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btReset;
    private javax.swing.JToggleButton btShowStopwatch;
    private javax.swing.JCheckBox cbAutoSort;
    private javax.swing.JCheckBox cbModulesOnly;
    private javax.swing.JComboBox cbProcess;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JScrollPane jScrollPane;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JToolBar jToolBar1;
    private final javax.swing.JPanel panelStopwatches = new javax.swing.JPanel();
    private javax.swing.JTable stopwatchTable;
    private javax.swing.JFormattedTextField txtWarn;
    // End of variables declaration//GEN-END:variables
  
    
    private void resetAll()
    {
      ((StopwatchTableModel)stopwatchTable.getModel()).reset();
    }
    
    private void registerListeners()
    {
      if (!Plugin.parent.checkConnected()) {
          return;
      }

      if (currentManager != null) {
          currentManager.removeListener(this.stopWatchListener);
      }

      if(btShowStopwatch.isSelected()) 
      {
        resetAll();

        String process = (String) cbProcess.getSelectedItem();

        Command command;
        if (cbModulesOnly.isSelected()) {
            command = new Command(process + ":modules:stopwatch");
        } else {
            command = new Command(process + ":representation:get").addArg("StopwatchManager");
        }

        currentManager = Plugin.genericManagerFactory.getManager(command);
        currentManager.addListener(this.stopWatchListener);
      }
    }
    
    // a named wrapped for a comperator to be stored in a drop down list
    class ComperatorElement implements Comparator<StopwatchEntry> 
    {
      private final String name;
      private final Comparator<StopwatchEntry> comp;

      public ComperatorElement(String name, Comparator<StopwatchEntry> comp) {
          this.name = name;
          this.comp = comp;
      }

      @Override
      public int compare(StopwatchEntry e1, StopwatchEntry e2) {
          return comp.compare(e1, e2);
      }

      @Override
      public String toString() {
          return name;
      }
  }
  
    
  private boolean allAscii(byte[] object) {
    for (byte b: object) {
        if(b > 127) {
            return false;
        }
    }
    return true;
  }

  class StopWatchListener implements ObjectListener<byte[]>
  {
        @Override
        public void newObjectReceived(byte[] object) {
            SwingUtilities.invokeLater(()->{
                try
                {

                  Messages.Stopwatches stopwatches = Messages.Stopwatches.parseFrom(object);
                  // collect all times into a map
                  Map<String, Integer> map = stopwatches.getStopwatchesList().stream().collect(
                      Collectors.toMap(i->i.getName(), i->i.getTime())
                  );

                  // update the table
                  ((StopwatchTableModel)stopwatchTable.getModel()).update(map);
                  if (cbAutoSort.isSelected()) {
                    ((TableRowSorter<TableModel>)stopwatchTable.getRowSorter()).sort();
                  }
                  stopwatchTable.repaint();
                }
                catch(InvalidProtocolBufferException ex)
                {
                  Logger.getLogger(StopwatchViewer.class.getName()).log(Level.SEVERE, null, ex);

                  if (allAscii(object)) {
                    errorOccured(new String(object));
                  } else {
                    errorOccured("Unknown binary data was received");
                  }
                }
            });
        }

        @Override
        public void errorOccured(String cause) {
            dispose();
            Logger.getAnonymousLogger().log(Level.SEVERE, cause);

            JOptionPane.showMessageDialog(StopwatchViewer.this, cause, "Error", JOptionPane.ERROR_MESSAGE);
        }
  }
  
  static class StopwatchTableModel extends DefaultTableModel 
  {
    interface StopwatchValue {
      Object get(StopwatchEntry entry);
    }
    
    static class Column {
        String name;
        StopwatchValue value;
        Column(String name, StopwatchValue value) {
            this.name = name;
            this.value = value;
        }
    }
      
    static private final Column[] columns = new Column[] {
        new Column("Name", stopwatch -> stopwatch.name),
        new Column("Value", stopwatch -> stopwatch.getValueMs()),
        new Column("Average", stopwatch -> stopwatch.getAverageMs()),
        new Column("Min", stopwatch -> stopwatch.getMinMs()),
        new Column("Max", stopwatch -> stopwatch.getMaxMs())};
    
    static private final Map<String, StopwatchEntry> stopwatchEntries = new HashMap<String, StopwatchEntry>();
    
    public StopwatchTableModel() {
        
    }
    
    synchronized public void update(Map<String, Integer> stopwatches) {
        boolean rowsAdded = false;
        for(Map.Entry<String, Integer> e : stopwatches.entrySet())
        {
          StopwatchEntry entry = stopwatchEntries.get(e.getKey());
          if(entry == null) {
            entry = new StopwatchEntry(e.getKey());
            stopwatchEntries.put(e.getKey(), entry);
            rowsAdded = true;
          }
          entry.addValue(e.getValue());
        }
        
        if (rowsAdded) {
            fireTableDataChanged();
        } else if (!stopwatchEntries.isEmpty()){
            fireTableRowsUpdated(0, getRowCount()-1);
        }
    }
    
    synchronized public void reset()  {
        if (!stopwatchEntries.isEmpty()) {
            stopwatchEntries.clear();
            fireTableDataChanged();
        }
    }
    
    @Override
    public boolean isCellEditable(int row, int column){
        return false;
    }
    
    @Override
    public String getColumnName(int column) {
        return columns[column].name;
    }
      
    @Override
    public int getRowCount() {
        return stopwatchEntries.size();
    }

    @Override
    public int getColumnCount() {
        return columns.length;
    }

    // this dummy is used to implicetly retrieve the class of the columns
    private static final StopwatchEntry testStopwatch = new StopwatchEntry("dummy");
    @Override
    public Class<?> getColumnClass(int columnIndex) {
        return columns[columnIndex].value.get(testStopwatch).getClass();
    }
    
    @Override
    public Object getValueAt(int row, int column) {
        try {
            StopwatchEntry stopwatch = (StopwatchEntry)stopwatchEntries.values().stream().toArray()[row];
            return columns[column].value.get(stopwatch);
        } catch (Exception ex) {
            System.out.println("test");
        }
        return null;
    }
    }
  
  private Color interpolate(Color c1, Color c2, Color c3, float t) {
      if (t >= 1.0) {
          return c3;
      }
      
      t = Math.max(Math.min(t, 1.0f), 0.0f);
      float r = Math.max(Math.min((c1.getRed()*(1.0f-t) + c2.getRed()*t) / 255.0f, 1.0f), 0.0f);
      float g = Math.max(Math.min((c1.getGreen()*(1.0f-t) + c2.getGreen()*t) / 255.0f, 1.0f), 0.0f);
      float b = Math.max(Math.min((c1.getBlue()*(1.0f-t) + c2.getBlue()*t) / 255.0f, 1.0f), 0.0f);
      return new Color(r,g,b);
  }
  
  public class StopwatchCellRenderer extends DefaultTableCellRenderer {
        private Color defaultColorOdd = null;
        private Color defaultColorEven = null;
      
        @Override
        public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int col) {

          //Cells are by default rendered as a JLabel.
          JLabel l = (JLabel) super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, col);

          float t = 0.0f;
          if (value instanceof Double) {
              double v = (Double)value;
              t = (float)Math.min(1.0, v / globalWarnLevel);
          }
          
          if (row % 2 == 1) {
              if(defaultColorOdd == null) {
                defaultColorOdd = l.getBackground();
              } else if (!isSelected) {
                  l.setBackground(interpolate(defaultColorOdd, normalColor, warnColor,t));
              }
          } else {
              if(defaultColorEven == null) {
                defaultColorEven = l.getBackground();
              } else if (!isSelected) {
                  l.setBackground(interpolate(defaultColorEven, normalColor, warnColor, t));
              }
          }
          
        return l;
      }
  }
  
  static class StopwatchEntry extends SimpleProgressBar
  {
    final String name;
    
    private double sum;
    private double count;
    private double max;
    private double min;
    private double value;
    private double average;

    public StopwatchEntry(String name) {
      super(0, 100);
      this.name = name;
      reset();
    }

    public void addValue(int value)
    {
      super.setValue(value);
      super.setString(this.toString());
      
      this.value = ((double)value) / 1000.0;
      this.sum += this.value;

      if(this.count == 0) {
        this.min = this.value;
        this.max = this.value;
      } else {
        this.min = Math.min(this.min, this.value);
        this.max = Math.max(this.max, this.value);
      }
      
      this.count++;
      this.average = (((double) this.sum) / ((double) this.count));
    }//end addValue

    public void reset()
    {
      this.sum = 0;
      this.max = 0;
      this.min = 0;
      this.count = 0;
      this.value = 0;
      this.average = 0;
    }

    double getValueMs() {
        return ((double)((int)(this.value*100)))/100.0;
    }
    
    double getAverageMs() {
        return ((double)((int)(this.average*100)))/100.0;
    }
    
    double getMinMs() {
        return ((double)((int)(this.min*100)))/100.0;
    }
    
    double getMaxMs() {
        return ((double)((int)(this.max*100)))/100.0;
    }
    
    @Override
    public String toString() {
      return String.format("%s: %.2f ms (avg: %.2f max: %.2f min: %.2f)", name, value, average, max, min);
    }
  }//end class StopwatchEntry

  @Override
  public void dispose()
  {
    btShowStopwatch.setSelected(false);
    if (currentManager != null) {
      currentManager.removeListener(this.stopWatchListener);
    } 
  }
}
