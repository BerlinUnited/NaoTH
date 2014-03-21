/*
 * StopwatchViewer.java
 *
 * Created on 18. April 2008, 20:25
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.AbstractDialog;
import de.naoth.rc.manager.ModuleStopwatchManager;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.manager.StopwatchManager;
import java.awt.Color;
import java.awt.GridLayout;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JProgressBar;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  thomas
 */
@PluginImplementation
public class StopwatchViewer extends AbstractDialog
  implements ObjectListener<HashMap<String, Integer>>
{

  @InjectPlugin
  public StopwatchManager stopwatchManager;
  @InjectPlugin
  public ModuleStopwatchManager moduleStopwatchManager;

  private HashMap<String, JProgressBar> progressBars;
  private HashMap<String, StopwatchEntry> stopwatchEntries;

  /** Creates new form StopwatchViewer */
  public StopwatchViewer()
  {
    initComponents();
  }

  @Init
  @Override
  public void init()
  {
    stopwatchEntries = new HashMap<String, StopwatchEntry>();
    progressBars = new HashMap<String, JProgressBar>();
  }


  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents()
  {

    jScrollPane1 = new javax.swing.JScrollPane();
    jToolBar1 = new javax.swing.JToolBar();
    btShowStopwatch = new javax.swing.JToggleButton();
    btReset = new javax.swing.JButton();
    jButton1 = new javax.swing.JButton();
    cbSortMethod = new javax.swing.JComboBox();
    cbModulesOnly = new javax.swing.JCheckBox();
    jLabel1 = new javax.swing.JLabel();
    jLabel2 = new javax.swing.JLabel();
    txtWarn = new javax.swing.JFormattedTextField();
    jLabel3 = new javax.swing.JLabel();
    lblSum = new javax.swing.JLabel();

    panelStopwatches.setLayout(new java.awt.GridLayout(0, 1));
    jScrollPane1.setViewportView(panelStopwatches);

    jToolBar1.setFloatable(false);
    jToolBar1.setRollover(true);

    btShowStopwatch.setMnemonic('s');
    btShowStopwatch.setText("Show stopwatches");
    btShowStopwatch.setFocusable(false);
    btShowStopwatch.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    btShowStopwatch.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    btShowStopwatch.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        btShowStopwatchActionPerformed(evt);
      }
    });
    jToolBar1.add(btShowStopwatch);

    btReset.setMnemonic('r');
    btReset.setText("Reset");
    btReset.setFocusable(false);
    btReset.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    btReset.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    btReset.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        btResetActionPerformed(evt);
      }
    });
    jToolBar1.add(btReset);

    jButton1.setText("Sort by");
    jButton1.setFocusable(false);
    jButton1.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    jButton1.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    jButton1.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jButton1ActionPerformed(evt);
      }
    });
    jToolBar1.add(jButton1);

    cbSortMethod.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Name", "Average", "Min", "Max" }));
    jToolBar1.add(cbSortMethod);

    cbModulesOnly.setText("modules only");
    cbModulesOnly.setFocusable(false);
    cbModulesOnly.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    cbModulesOnly.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        cbModulesOnlyActionPerformed(evt);
      }
    });
    jToolBar1.add(cbModulesOnly);

    jLabel1.setText("warn:");

    jLabel2.setText(" ms");

    txtWarn.setColumns(1);
    txtWarn.setFormatterFactory(new javax.swing.text.DefaultFormatterFactory(new javax.swing.text.NumberFormatter(new java.text.DecimalFormat("#0"))));
    txtWarn.setText("30");

    jLabel3.setText("sum:");

    lblSum.setText("----");

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 460, Short.MAX_VALUE)
      .addGroup(layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(layout.createSequentialGroup()
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 423, Short.MAX_VALUE)
            .addContainerGap())
          .addGroup(layout.createSequentialGroup()
            .addComponent(jLabel1)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(txtWarn, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(3, 3, 3)
            .addComponent(jLabel2)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jLabel3)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(lblSum)
            .addGap(11, 11, 11))))
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 243, Short.MAX_VALUE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel1)
          .addComponent(jLabel2)
          .addComponent(txtWarn, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel3)
          .addComponent(lblSum)))
    );
  }// </editor-fold>//GEN-END:initComponents

private void btShowStopwatchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btShowStopwatchActionPerformed

  registerListeners();
   
}//GEN-LAST:event_btShowStopwatchActionPerformed

private void registerListeners()
{
  stopwatchManager.removeListener(this);
  moduleStopwatchManager.removeListener(this);
  
  if(btShowStopwatch.isSelected()) 
  {
    resetAll();
    
    if(cbModulesOnly.isSelected())
    {
      moduleStopwatchManager.addListener(this);
    }
    else
    {
      stopwatchManager.addListener(this);
    }
  }
}

private void btResetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btResetActionPerformed

  resetAll();
}//GEN-LAST:event_btResetActionPerformed

private void resetAll()
{
  stopwatchEntries.clear();
  progressBars.clear();

  panelStopwatches.removeAll();
  GridLayout layout = (GridLayout) panelStopwatches.getLayout();
  layout.setRows(0);
}

private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
  panelStopwatches.removeAll();

  // default comperator compares the names :)
  Comparator<StopwatchEntry> comparator = new Comparator<StopwatchEntry>()
  {
    @Override
    public int compare(StopwatchEntry entryOne, StopwatchEntry entryTwo)
    { 
      return entryOne.name.compareTo(entryTwo.name);
    }
  };
  
  switch(this.cbSortMethod.getSelectedIndex())
  {
    case 0: break;
    case 1:
      comparator = new Comparator<StopwatchEntry>()
      { 
      @Override
        public int compare(StopwatchEntry e1, StopwatchEntry e2) 
        { return (int)(e2.getAverage() - e1.getAverage()); }
      };
      break;
    case 2: 
      comparator = new Comparator<StopwatchEntry>()
      { 
      @Override
        public int compare(StopwatchEntry e1, StopwatchEntry e2) 
        { return e2.min - e1.min; }
      };
      break;
    case 3: 
      comparator = new Comparator<StopwatchEntry>()
      { 
      @Override
        public int compare(StopwatchEntry e1, StopwatchEntry e2) 
        { return e2.max - e1.max; }
      };
      break;
    default: break;
  }//end switch


  ArrayList<StopwatchEntry> stopwatchList = new ArrayList<StopwatchEntry>(stopwatchEntries.values());
  java.util.Collections.sort(stopwatchList, comparator);

  GridLayout layout = (GridLayout) panelStopwatches.getLayout();
  layout.setRows(stopwatchList.size());
  
  for(StopwatchEntry entry: stopwatchList)
  {
    panelStopwatches.add(progressBars.get(entry.name));
  }
  panelStopwatches.revalidate();
}//GEN-LAST:event_jButton1ActionPerformed

  private void cbModulesOnlyActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbModulesOnlyActionPerformed
  {//GEN-HEADEREND:event_cbModulesOnlyActionPerformed
    registerListeners();
  }//GEN-LAST:event_cbModulesOnlyActionPerformed

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton btReset;
  private javax.swing.JToggleButton btShowStopwatch;
  private javax.swing.JCheckBox cbModulesOnly;
  private javax.swing.JComboBox cbSortMethod;
  private javax.swing.JButton jButton1;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JToolBar jToolBar1;
  private javax.swing.JLabel lblSum;
  private final javax.swing.JPanel panelStopwatches = new javax.swing.JPanel();
  private javax.swing.JFormattedTextField txtWarn;
  // End of variables declaration//GEN-END:variables
  @Override
  public void newObjectReceived(HashMap<String, Integer> sw)
  {
    double warn = Double.MAX_VALUE;
    try {
      warn = Double.parseDouble(txtWarn.getText());
    } catch(NumberFormatException ex) {
      ex.printStackTrace();
    }

    // get maximum value
    int max = 0;
    int sum = 0;
    for(Map.Entry<String, Integer> e : sw.entrySet())
    {
      sum += e.getValue();
      max = Math.max(max, e.getValue());
      if(stopwatchEntries.containsKey(e.getKey()))
      {
        max = Math.max(max, stopwatchEntries.get(e.getKey()).max);
      }
    }//end for

    // add components to show result
    for(String key: sw.keySet())
    {
      int valInt = sw.get(key);

      if(!stopwatchEntries.containsKey(key))
      {
        stopwatchEntries.put(key, new StopwatchEntry(key));
      }//end if

      StopwatchEntry entry = stopwatchEntries.get(key);
      entry.addValue(valInt);

      updateStopwatchBar(entry, max, warn);
    }//end for

    panelStopwatches.revalidate();
    lblSum.setText(String.format("%4.2f ms", (double) sum / 1000.0));
  }//end newObjectReceived

  
  private void updateStopwatchBar(StopwatchEntry entry, int max, double warn)
  {
    // add new bar
    if(!progressBars.containsKey(entry.name))
    {
      GridLayout layout = (GridLayout) panelStopwatches.getLayout();
      layout.setRows(layout.getRows() + 1);
      JProgressBar bar = new JProgressBar();
      bar.setStringPainted(true);
      bar.setBorderPainted(true);
      progressBars.put(entry.name, bar);
      panelStopwatches.add(bar);
    }//end if

    JProgressBar bar = progressBars.get(entry.name);
    bar.setMaximum(max);
    bar.setMinimum(0);
    bar.setValue(entry.value);
    bar.setString(entry.toString());

    if(entry.value >= warn*1000.0) {
      bar.setForeground(Color.red);
    } else {
      bar.setForeground(Color.black);
    }
  }//end updateStopwatchEntry

  
  @Override
  public void errorOccured(String cause)
  {
    btShowStopwatch.setSelected(false);
    stopwatchManager.removeListener(this);

    Logger.getAnonymousLogger().log(Level.SEVERE, cause);
  }//end errorOccured
  
  
  class StopwatchEntry
  {
    final String name;
    int sum;
    int count;
    int max;
    int min;
    int value;

    public StopwatchEntry(String name)
    {
      this.name = name;
      reset();
    }

    public void addValue(int value)
    {
      this.value = value;
      this.sum += value;

      if(this.count == 0)
      {
        this.min = value;
        this.max = value;
      }else
      {
        this.min = Math.min(this.min, value);
        this.max = Math.max(this.max, value);
      }
      
      this.count++;
    }//end addValue

    public double getAverage()
    {
      if(this.count == 0) return 0;
      return (((double) this.sum) / ((double) this.count));
    }//end getAverage

    public void reset()
    {
      this.sum = 0;
      this.max = 0;
      this.min = 0;
      this.count = 0;
      this.value = 0;
    }//end reset

    @Override
    public String toString()
    {
      double val = ((double) this.value) / 1000.0;
      double avg = getAverage() / 1000.0;

      String outStr = String.format("%s: %.2f ms (avg: %.2f max: %.2f min: %.2f)",
        name, val, avg,
        ((double) this.max) / 1000.0,
        ((double) this.min) / 1000.0);
      return outStr;
    }//end toString
  }//end class StopwatchEntry

  @Override
  public void dispose()
  {
    stopwatchManager.removeListener(this);
  }
}
