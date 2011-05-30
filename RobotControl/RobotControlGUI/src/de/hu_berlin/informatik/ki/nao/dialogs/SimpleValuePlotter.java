/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * SimpleValuePlotter.java
 *
 * Created on 15.02.2011, 00:38:59
 */

package de.hu_berlin.informatik.ki.nao.dialogs;

import de.hu_berlin.informatik.ki.nao.AbstractDialog;
import de.hu_berlin.informatik.ki.nao.RobotControl;
import de.hu_berlin.informatik.ki.nao.manager.ObjectListener;
import de.hu_berlin.informatik.ki.nao.manager.PlotDataManager;
import de.hu_berlin.informatik.ki.nao.messages.Messages.PlotItem;
import de.hu_berlin.informatik.ki.nao.messages.Messages.Plots;
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
import info.monitorenter.gui.util.ColorIterator;
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
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class SimpleValuePlotter  extends AbstractDialog
  implements ObjectListener<Plots>
{

  @InjectPlugin
  public RobotControl parent;
  @InjectPlugin
  public PlotDataManager plotDataManager;

  @Init
  public void init()
  {
  }//end init

  private int numberOfValues = 5000;
  private ColorIterator colorIterator = new ColorIterator();
  private APointHighlighter pointHighlighter = new PointHighlighterConfigurable(new PointPainterCoords(), true);
    
  private Map<String, ITrace2D> plotTraces = new HashMap<String, ITrace2D>();

    /** Creates new form SimpleValuePlotter */
    public SimpleValuePlotter() {
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
      .addGap(0, 370, Short.MAX_VALUE)
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
        .addComponent(jSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 356, Short.MAX_VALUE))
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
        chart.enablePointHighlighting(true);
      }
}//GEN-LAST:event_btReceiveDataActionPerformed

    private void btClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btClearActionPerformed

      synchronized(this)
      {
        this.plotTraces.clear();
        this.panelList.removeAll();
        this.chart.removeAllTraces();

        // its necessary, otherwise we get an null pointer exception
        this.chart.enablePointHighlighting(false);
      }//end synchronized
    }//GEN-LAST:event_btClearActionPerformed

  

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton btClear;
  private javax.swing.JToggleButton btReceiveData;
  private info.monitorenter.gui.chart.Chart2D chart;
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
        for(PlotItem item : data.getPlotsList())
        {
          if(item.getType() == PlotItem.PlotType.Default && item.hasX() && item.hasY())
          {
            addValue(item.getName(), item.getX(), item.getY());
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
    
    plotTraces.get(name).addPoint(x, y);
  }//end addValue

  @Override
  public void dispose()
  {
    plotDataManager.removeListener(this);
  }

  @Override
  public JPanel getPanel() {
    return this;
  }


  private int currentColorIndex = 0;
  private Color colorArray[] = new Color[]
  {
    Color.blue,
    Color.green,
    Color.red,
    Color.yellow,
    Color.orange,
    Color.pink,
    Color.gray,
    Color.magenta,
    Color.cyan
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
