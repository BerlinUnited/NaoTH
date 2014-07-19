/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * SimpleValuePlotter.java
 *
 * Created on 15.02.2011, 00:38:59
 */

package de.naoth.rc.dialogs;

import de.naoth.rc.AbstractDialog;
import de.naoth.rc.RobotControl;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.manager.PlotDataManager;
import de.naoth.rc.messages.CommonTypes.DoubleVector2;
import de.naoth.rc.messages.Messages.PlotStroke2D;
import de.naoth.rc.messages.Messages.Plots;
import info.monitorenter.gui.chart.IAxis;
import info.monitorenter.gui.chart.IRangePolicy;
import info.monitorenter.gui.chart.ITrace2D;
import info.monitorenter.gui.chart.ITracePoint2D;
import info.monitorenter.gui.chart.LabeledValue;
import info.monitorenter.gui.chart.axis.AxisLinear;
import info.monitorenter.gui.chart.pointhighlighters.APointHighlighter;
import info.monitorenter.gui.chart.pointhighlighters.PointHighlighterConfigurable;
import info.monitorenter.gui.chart.pointpainters.APointPainter;
import info.monitorenter.gui.chart.rangepolicies.ARangePolicy;
import info.monitorenter.gui.chart.rangepolicies.RangePolicyFixedViewport;
import info.monitorenter.gui.chart.traces.Trace2DLtd;
import info.monitorenter.util.Range;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import javax.swing.JCheckBox;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class Plott2D  extends AbstractDialog
  implements ObjectListener<Plots>
{

  @InjectPlugin
  public RobotControl parent;
  @InjectPlugin
  public PlotDataManager plotDataManager;

  
  private final int maxNumberOfValues = 5000;
  private final int numberOfValues = maxNumberOfValues;
  
  //private final ColorIterator colorIterator = new ColorIterator();
  private final APointHighlighter pointHighlighter = new PointHighlighterConfigurable(new PointPainterCoords(), true);

  private final Map<String, ITrace2D> plotTraces = new HashMap<String, ITrace2D>();
  private final Map<String, Double> plotTracesMaxX = new HashMap<String, Double>();

    /** Creates new form SimpleValuePlotter */
    public Plott2D() {
        initComponents();

        chart.setUseAntialiasing(false);

        DragListener dragListener = new DragListener();
        
        chart.addMouseListener(dragListener);
        chart.addMouseMotionListener(dragListener);
        chart.addMouseWheelListener(new MouseWheelListener() {
          double scale = 1.0;
          @Override
          public void mouseWheelMoved(MouseWheelEvent e) {

            IAxis axisX = chart.getAxisX();
            
            double newScale = scale + scale * e.getWheelRotation() * 0.1;
            double oldOffset = axisX.translatePxToValue(e.getX());

            double xmin = axisX.getRange().getMin();
            double xmax = axisX.getRange().getMax();
            
            xmin = (xmin-oldOffset) * newScale + oldOffset;
            xmax = (xmax-oldOffset) * newScale + oldOffset;
            
            //axisX.setMajorTickSpacing(Math.ceil(axisX.getMajorTickSpacing()*newScale*10.0)/10.0);
            //axisX.setMinorTickSpacing(Math.ceil(axisX.getMinorTickSpacing()*newScale*10.0)/10.0);

            chart.getAxisX().setRange(new Range(xmin*scale, xmax*scale));//new RangePolicyFixedViewport(new Range(xmin*scale, xmax*scale));
            //axisX.setRangePolicy(zoomPolicyX);
          }
        });

        ScrollAxis axis = new ScrollAxis();
        //IRangePolicy zoomPolicyX = new RangePolicyFixedViewport(new Range(0, 10));
        chart.setAxisXBottom(axis);
        axis.setRangePolicy(new DynamicRangePolicy(new Range(0, numberOfValues)));
        //axis.setRangePolicy(zoomPolicyX);

        //chart.getAxisY().setRangePolicy(new MinimalRangePolicy(chart.getAxisY().getRange()));
        chart.setGridColor(Color.lightGray);

        //chart.getAxisY().setPaintGrid(true);
        //chart.getAxisX().setPaintGrid(true);
    }

    class ScrollAxis extends AxisLinear
    {
      @Override
      protected List<LabeledValue> getLabels(final double resolution)
      {
        List<LabeledValue> collect = new LinkedList<LabeledValue>();
        if (resolution <= 0)
          return collect;

        Range domain = this.getRange();
        double min = domain.getMin();
        double max = domain.getMax();
        double range = max - min;
        
        double k = (Math.log(range/10))/Math.log(2);
        k = Math.ceil(k);

        double minorTickSpacing = Math.pow(2.0,k); //this.m_minorTickSpacing;
        double majorTickSpacing = 5.0*minorTickSpacing;//this.m_majorTickSpacing;

        double minorTickOffset = Math.ceil(min / minorTickSpacing);
        double majorTickOffset = Math.ceil(min / majorTickSpacing);

        //double minorDistance = Math.abs(value - minorRound);
        //double majorDistance = Math.abs(value - majorRound);

        double minorValue = minorTickOffset*minorTickSpacing;
        double majorValue = majorTickOffset*majorTickSpacing;

        double value = Math.min(minorValue, majorValue);
        
        double loopStop = 0;
        while(value <= max && loopStop < 100)
        {
          LabeledValue label = new LabeledValue();
          
          label.setLabel(this.getFormatter().format(value));
          label.setValue((value - min)/range);

          label.setMajorTick(minorValue >= majorValue);

          if(minorValue <= value)
            minorValue += minorTickSpacing;

          if(majorValue <= value)
            majorValue += majorTickSpacing;

          value = Math.min(minorValue, majorValue);

          collect.add(label);
          loopStop++;
        }//end while
        
        return collect;
      }//end getLabels
    }//end class ScrollAxis

    
    class DragListener implements MouseListener, MouseMotionListener
    {
      private int oldX = 0;
      private int oldY = 0;
      private IRangePolicy oldRangePolicy = null;

      @Override
      public void mousePressed(MouseEvent e) {
        this.oldX = e.getX();
        this.oldY = e.getY();
        this.oldRangePolicy = chart.getAxisX().getRangePolicy();
      }//end mousePressed

      @Override
      public void mouseDragged(MouseEvent e) {

        IAxis axisX = chart.getAxisX();

        double xAxisOld = axisX.translatePxToValue(this.oldX);
        double xAxisNew = axisX.translatePxToValue(e.getX());
        this.oldX = e.getX();

        double delta = xAxisNew - xAxisOld;

        double xmin = axisX.getRange().getMin() - delta;
        double xmax = axisX.getRange().getMax() - delta;

        IRangePolicy zoomPolicyX = new RangePolicyFixedViewport(new Range(xmin, xmax));
        axisX.setRangePolicy(zoomPolicyX);

        axisX.setStartMajorTick(false);
        //System.out.println(axisX.getMajorTickSpacing());
        chart.setRequestedRepaint(true);
      }//end mouseDragged

      @Override
      public void mouseClicked(MouseEvent e) {}
      @Override
      public void mouseMoved(MouseEvent e) {}
      @Override
      public void mouseReleased(MouseEvent e)
      {
        this.oldRangePolicy.setRange(chart.getAxisX().getRange());
        chart.getAxisX().setRangePolicy(this.oldRangePolicy);
      }
      @Override
      public void mouseEntered(MouseEvent e) {}
      @Override
      public void mouseExited(MouseEvent e) {}
    }//end DragListener


    class DynamicRangePolicy extends ARangePolicy
    {

      public DynamicRangePolicy(Range range)
      {
        super(range);
      }

      @Override
      public double getMax(double chartMin, double chartMax) {
        if(chartMax < this.getRange().getMin())
          return chartMax + this.getRange().getExtent();

        return Math.max(this.getRange().getMax(),chartMax);
      }

      @Override
      public double getMin(double chartMin, double chartMax) {
        return getMax(chartMin, chartMax) - this.getRange().getExtent();
      }
    }//end class DynamicRangePolicy

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jToolBar1 = new javax.swing.JToolBar();
        btReceiveData = new javax.swing.JToggleButton();
        btClear = new javax.swing.JButton();
        btClearData = new javax.swing.JButton();
        cbDontScrollOnStaticRegionData = new javax.swing.JCheckBox();
        jButton1 = new javax.swing.JButton();
        cbShowGrid = new javax.swing.JCheckBox();
        jSplitPane = new javax.swing.JSplitPane();
        scrollList = new javax.swing.JScrollPane();
        panelList = new javax.swing.JPanel();
        chart = new info.monitorenter.gui.chart.Chart2D();

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        btReceiveData.setText("Receive Data");
        btReceiveData.setFocusable(false);
        btReceiveData.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReceiveData.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReceiveData.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveDataActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceiveData);

        btClear.setText("Clear");
        btClear.setFocusable(false);
        btClear.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btClear.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btClear.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btClearActionPerformed(evt);
            }
        });
        jToolBar1.add(btClear);

        btClearData.setText("Clear Data");
        btClearData.setFocusable(false);
        btClearData.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btClearData.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btClearData.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btClearDataActionPerformed(evt);
            }
        });
        jToolBar1.add(btClearData);

        cbDontScrollOnStaticRegionData.setSelected(true);
        cbDontScrollOnStaticRegionData.setText("dont scroll in x-axis if on static data regions");
        cbDontScrollOnStaticRegionData.setFocusable(false);
        cbDontScrollOnStaticRegionData.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        cbDontScrollOnStaticRegionData.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToolBar1.add(cbDontScrollOnStaticRegionData);

        jButton1.setText("Fit to Data");
        jButton1.setFocusable(false);
        jButton1.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jButton1.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });
        jToolBar1.add(jButton1);

        cbShowGrid.setText("Show Grid");
        cbShowGrid.setFocusable(false);
        cbShowGrid.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        cbShowGrid.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        cbShowGrid.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbShowGridActionPerformed(evt);
            }
        });
        jToolBar1.add(cbShowGrid);

        jSplitPane.setBorder(null);
        jSplitPane.setDividerLocation(150);

        scrollList.setBorder(null);

        panelList.setBackground(new java.awt.Color(255, 255, 255));
        panelList.setLayout(new javax.swing.BoxLayout(panelList, javax.swing.BoxLayout.PAGE_AXIS));
        scrollList.setViewportView(panelList);

        jSplitPane.setLeftComponent(scrollList);

        javax.swing.GroupLayout chartLayout = new javax.swing.GroupLayout(chart);
        chart.setLayout(chartLayout);
        chartLayout.setHorizontalGroup(
            chartLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 369, Short.MAX_VALUE)
        );
        chartLayout.setVerticalGroup(
            chartLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 356, Short.MAX_VALUE)
        );

        jSplitPane.setRightComponent(chart);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 524, Short.MAX_VALUE)
            .addComponent(jSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 524, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(jSplitPane))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btReceiveDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveDataActionPerformed

      if (btReceiveData.isSelected()) {
        if (parent.checkConnected()) {
          plotDataManager.addListener(this);
          chart.getAxisX().setRangePolicy(new DynamicRangePolicy(chart.getAxisX().getRange()));
        } else {
          btReceiveData.setSelected(false);
        }
      } else {
        plotDataManager.removeListener(this);
        chart.getAxisX().setRangePolicy(new RangePolicyFixedViewport(chart.getAxisX().getRange()));
        //chart.enablePointHighlighting(true);
      }
}//GEN-LAST:event_btReceiveDataActionPerformed

    private void btClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btClearActionPerformed

      synchronized(this)
      {
        this.plotTraces.clear();
        this.plotTracesMaxX.clear();
        this.panelList.removeAll();
        this.chart.removeAllTraces();

        // its necessary, otherwise we get an null pointer exception
        this.chart.enablePointHighlighting(false);
        
        this.panelList.removeAll();
        this.panelList.repaint();
      }//end synchronized
    }//GEN-LAST:event_btClearActionPerformed

  private void btClearDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btClearDataActionPerformed
      synchronized(this)
      {
        clearTracePoints();
      }
  }//GEN-LAST:event_btClearDataActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        chart.getAxisX().setRange(new Range(chart.getAxisX().getMinValue(), chart.getAxisX().getMaxValue()));
    }//GEN-LAST:event_jButton1ActionPerformed

private void clearTracePoints()
{
  for(String traceName : this.plotTraces.keySet())
  {
    this.plotTraces.get(traceName).removeAllPoints();
    this.plotTracesMaxX.put(traceName, 0.0);
  }
}
    private void cbShowGridActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbShowGridActionPerformed
        if(this.cbShowGrid.isSelected())
        {
            chart.getAxisY().setPaintGrid(true);
            chart.getAxisX().setPaintGrid(true);
        }
        else
        {
            chart.getAxisY().setPaintGrid(false);
            chart.getAxisX().setPaintGrid(false);
        }
    }//GEN-LAST:event_cbShowGridActionPerformed

  

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btClear;
    private javax.swing.JButton btClearData;
    private javax.swing.JToggleButton btReceiveData;
    private javax.swing.JCheckBox cbDontScrollOnStaticRegionData;
    private javax.swing.JCheckBox cbShowGrid;
    private info.monitorenter.gui.chart.Chart2D chart;
    private javax.swing.JButton jButton1;
    private javax.swing.JSplitPane jSplitPane;
    private javax.swing.JToolBar jToolBar1;
    private javax.swing.JPanel panelList;
    private javax.swing.JScrollPane scrollList;
    // End of variables declaration//GEN-END:variables

  @Override
  public void errorOccured(String cause) {
    JOptionPane.showMessageDialog(this, cause, "Error", JOptionPane.ERROR_MESSAGE);
    plotDataManager.removeListener(this);
  }

  @Override
  public void newObjectReceived(Plots data)
  {
    if (data != null)
    {
      synchronized(this)
      {
         /*
        for(PlotItem item : data.getPlotsList())
        {
          if(item.getType() == PlotItem.PlotType.Default && item.hasX() && item.hasY())
          {
            addValue(item.getName(), item.getX(), item.getY());
          }
        }//end for
           * */
        for(PlotStroke2D stroke : data.getPlotstrokesList())
        {
          for(DoubleVector2 point: stroke.getPointsList())
          {
            addValue(stroke.getName(), point.getX(), point.getY());
          }
        }//end for
      }//end synchronized
    }//end if
  }//end newObjectReceived


  public class PointPainterCoords extends APointPainter
  {
    int radius = 5;
    @Override
    public void paintPoint(final int absoluteX, final int absoluteY, final int nextX,
        final int nextY, final Graphics g, final ITracePoint2D point)
    {
      g.drawOval(absoluteX - this.radius, absoluteY - this.radius, 2*this.radius,2*this.radius);
      
      g.drawString(point.getX() + " : " + point.getY(), absoluteX, absoluteY- 2*this.radius);
    }
  }//end class PointPainterCoords
  
  private void addValue(final String name, double x, double y)
  {
    if(Double.isInfinite(x) || Double.isInfinite(y))
    {
      JOptionPane.showMessageDialog(this, name + " has an infinite value", "Error", JOptionPane.ERROR_MESSAGE);
      return;
    }

    if(Double.isNaN(x) || Double.isNaN(y))
    {
      JOptionPane.showMessageDialog(this, name + " has a NaN-Value", "Error", JOptionPane.ERROR_MESSAGE);
      return;
    }

    // it is necessary to prevent deadlock while adding a point
    chart.enablePointHighlighting(false);

    // add a new plot
    if (!plotTraces.containsKey(name))
    {
      Trace2DLtd trace = new Trace2DLtd(numberOfValues, name);
      trace.setVisible(false);
      trace.setPointHighlighter(pointHighlighter);
      plotTraces.put(name, trace);
      plotTracesMaxX.put(name, 0.0);
      chart.addTrace(trace);

      final JCheckBox checkBoxItem = new JCheckBox(name, false);
      checkBoxItem.setBackground(Color.white);
      checkBoxItem.addActionListener(new ActionListener()
      {
        @Override
        public void actionPerformed(ActionEvent e)
        {
          if(checkBoxItem.isSelected())
          {
            Color c = getNextColor();
            ITrace2D trace = (ITrace2D)plotTraces.get(name);
            trace.setColor(c);
            trace.setVisible(true);
            checkBoxItem.setBackground(c);
          }
          else
          {
            ITrace2D trace = (ITrace2D)plotTraces.get(name);
            trace.setVisible(false);
            checkBoxItem.setBackground(Color.white);
          }
        }
      });

      panelList.add(checkBoxItem);
    }//end if
    
    if(cbDontScrollOnStaticRegionData.isSelected())
    {
      if(x > plotTracesMaxX.get(name))
      {
        plotTracesMaxX.put(name, x);
        plotTraces.get(name).addPoint(x, y);
      }
      else
      {
        plotTracesMaxX.put(name, 0.0);
        plotTraces.get(name).removeAllPoints();
      }
    }
    else
    {
      plotTracesMaxX.put(name, x);
      plotTraces.get(name).addPoint(x, y);
    }
    
  }//end addValue

  @Override
  public void dispose()
  {
    btReceiveData.setSelected(false);
    plotDataManager.removeListener(this);
  }

  private int currentColorIndex = 0;
  private final int colorDarkness = 200;
  private final Color colorArray[] = new Color[]
  {
    new Color(0,0,colorDarkness),
    new Color(0,colorDarkness,0),
    new Color(colorDarkness,0,0),
    new Color(colorDarkness,0,colorDarkness),
    new Color(colorDarkness,colorDarkness,0),
    new Color(0,colorDarkness,colorDarkness),
    new Color(colorDarkness,colorDarkness,colorDarkness),
    //Color.blue,
    //Color.green,
    //Color.red,
    //Color.yellow,
    //Color.orange,
    //Color.pink,
    //Color.gray,
    //Color.magenta,
    //Color.cyan
    //new Color(128,0,128),
    //Color.black,
  };
  
  private Color getNextColor()
  {
    /*
    if(!colorIterator.hasNext())
       colorIterator.reset();

    return colorIterator.next();
     * */

    currentColorIndex = (currentColorIndex+1)%colorArray.length;
    return colorArray[currentColorIndex];
  }//end getNextColor
}//end class SimpleValuePlotter
