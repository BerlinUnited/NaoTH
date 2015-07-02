package de.naoth.rc.components;

import info.monitorenter.gui.chart.Chart2D;
import info.monitorenter.gui.chart.IAxis;
import info.monitorenter.gui.chart.IRangePolicy;
import info.monitorenter.gui.chart.ITrace2D;
import info.monitorenter.gui.chart.ITracePoint2D;
import info.monitorenter.gui.chart.LabeledValue;
import info.monitorenter.gui.chart.axis.AxisLinear;
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
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import javax.swing.JCheckBox;
import javax.swing.JOptionPane;

/**
 *
 * @author Heinrich Mellmann
 */
public class PlotPanel extends javax.swing.JPanel {

    private int numberOfValues = 10000;
    private boolean dontScrollOnStaticRegionData = false;
    
    private final Map<String, ITrace2D> plotTraces = new HashMap<String, ITrace2D>();
    private final Map<String, Double> plotTracesMaxX = new HashMap<String, Double>();
    
    private final SimpleColorIterator colorIterator = new SimpleColorIterator();
    //private final APointHighlighter pointHighlighter = new PointHighlighterConfigurable(new PointPainterCoords(), true);
    
    /**
     * Creates new form PlotPanel
     */
    public PlotPanel() {
        initComponents();
        
        chart.setUseAntialiasing(false);

        DragListener dragListener = new DragListener();

        chart.addMouseListener(dragListener);
        chart.addMouseMotionListener(dragListener);
        chart.addMouseWheelListener(new MouseWheelZoom());

        ScrollAxis axis = new ScrollAxis();
        chart.setAxisXBottom(axis);
        
        // NOTE: the axis needs to be registered at a chart before setting the policy
        axis.setRangePolicy(new DynamicRangePolicy(new Range(0, numberOfValues)));
        //IRangePolicy zoomPolicyX = new RangePolicyFixedViewport(new Range(0, 10));
        //axis.setRangePolicy(zoomPolicyX);
        
        //chart.getAxisY().setRangePolicy(new MinimalRangePolicy(chart.getAxisY().getRange()));
        
        chart.setGridColor(Color.lightGray);
        //chart.getAxisY().setPaintGrid(true);
        //chart.getAxisX().setPaintGrid(true);
    }
    
    public void clear() {
        this.plotTraces.clear();
        this.plotTracesMaxX.clear();
        this.chart.removeAllTraces();
        // its necessary, otherwise we get an null pointer exception
        this.chart.enablePointHighlighting(false);
        
        this.panelList.removeAll();
        this.panelList.repaint();
    }
    
    public void clearTracePoints() {
        for (String traceName : this.plotTraces.keySet()) {
            this.plotTraces.get(traceName).removeAllPoints();
            this.plotTracesMaxX.put(traceName, 0.0);
        }
    }
    
    public void setPaintGrid(boolean v) {
        chart.getAxisY().setPaintGrid(v);
        chart.getAxisX().setPaintGrid(v);
    }
    
    public void setDontScrollOnStaticRegionData(boolean v) {
        this.dontScrollOnStaticRegionData = v;
    }
    
    public void setNumberOfValues(int v) {
        this.numberOfValues = v;
    }
    
    public void fitToData() {
        this.chart.getAxisX().setRange(new Range(chart.getAxisX().getMinValue(), chart.getAxisX().getMaxValue()));
    }
    
    public void setAutoScroll(boolean v) {
        if(v) {
            chart.getAxisX().setRangePolicy(new DynamicRangePolicy(chart.getAxisX().getRange()));
        } else {
            chart.getAxisX().setRangePolicy(new RangePolicyFixedViewport(chart.getAxisX().getRange()));
        }
    }
    
    public Chart2D getChart() {
        return this.chart;
    }
    
    public void addValue(final String name, double x, double y) {
        if (Double.isInfinite(x) || Double.isInfinite(y)) {
            JOptionPane.showMessageDialog(this, name + " has an infinite value", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        if (Double.isNaN(x) || Double.isNaN(y)) {
            JOptionPane.showMessageDialog(this, name + " has a NaN-Value", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        // it is necessary to prevent deadlock while adding a new point
        chart.enablePointHighlighting(false);

        // add a new plot
        if (!plotTraces.containsKey(name)) {
            final Trace2DLtd trace = new Trace2DLtd(numberOfValues, name);
            trace.setVisible(false);
            //trace.setPointHighlighter(pointHighlighter);
            plotTraces.put(name, trace);
            plotTracesMaxX.put(name, 0.0);
            chart.addTrace(trace);

            // create a checkbox for this trace
            final JCheckBox checkBoxItem = new JCheckBox(name, false);
            checkBoxItem.setBackground(Color.white);
            checkBoxItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    if (checkBoxItem.isSelected()) {
                        Color c = getNextColor();
                        //ITrace2D trace = (ITrace2D) plotTraces.get(name);
                        trace.setColor(c);
                        trace.setVisible(true);
                        checkBoxItem.setBackground(c);
                    } else {
                        //ITrace2D trace = (ITrace2D) plotTraces.get(name);
                        trace.setVisible(false);
                        checkBoxItem.setBackground(Color.white);
                    }
                }
            });

            panelList.add(checkBoxItem);
        }//end if

        ITrace2D trace = (ITrace2D) plotTraces.get(name);
        
        // allow only monotonic growth
        if (dontScrollOnStaticRegionData && x <= plotTracesMaxX.get(name)) {
            trace.removeAllPoints();
        }
        
        plotTracesMaxX.put(name, x);
        plotTraces.get(name).addPoint(x, y);
    }//end addValue
    
    
    private Color getNextColor() {
         return colorIterator.next();
    }
    
    class SimpleColorIterator {
        private int currentColorIndex = 0;
        private final int colorDarkness = 200;
        
        private final Color colorArray[] = new Color[]{
            new Color(0, 0, colorDarkness),
            new Color(0, colorDarkness, 0),
            new Color(colorDarkness, 0, 0),
            new Color(colorDarkness, 0, colorDarkness),
            new Color(colorDarkness, colorDarkness, 0),
            new Color(0, colorDarkness, colorDarkness)
        };
            
        public Color next() {
            currentColorIndex = (currentColorIndex+1)%colorArray.length;
            return colorArray[currentColorIndex];
        }
    }
    
    public void exportCSV() {
        for (String name : plotTraces.keySet()) {
            ITrace2D trace = plotTraces.get(name);
            Iterator<ITracePoint2D> pi = trace.iterator();

            try {
                String fileName = name.replace(':', '_') + ".txt";
                PrintWriter writer = new PrintWriter(fileName, "UTF-8");

                while (pi.hasNext()) {
                    ITracePoint2D p = pi.next();
                    writer.print(p.getX());
                    writer.print(',');
                    writer.println(p.getY());
                }
                
                writer.close();
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }
    
    class DragListener implements MouseListener, MouseMotionListener {

        private int oldX = 0;
        //private int oldY = 0;
        private IRangePolicy oldRangePolicy = null;

        @Override
        public void mousePressed(MouseEvent e) {
            this.oldX = e.getX();
            //this.oldY = e.getY();
            this.oldRangePolicy = chart.getAxisX().getRangePolicy();
        }

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
        public void mouseReleased(MouseEvent e) {
            this.oldRangePolicy.setRange(chart.getAxisX().getRange());
            chart.getAxisX().setRangePolicy(this.oldRangePolicy);
        }
        
        @Override
        public void mouseClicked(MouseEvent e) {}
        @Override
        public void mouseMoved(MouseEvent e) {}
        @Override
        public void mouseEntered(MouseEvent e) {}
        @Override
        public void mouseExited(MouseEvent e) {}
    }//end DragListener

    
    class MouseWheelZoom implements MouseWheelListener 
    {
        double scale = 1.0;

        @Override
        public void mouseWheelMoved(MouseWheelEvent e) {

            IAxis axisX = chart.getAxisX();

            double newScale = scale + scale * e.getWheelRotation() * 0.1;
            double oldOffset = axisX.translatePxToValue(e.getX());

            double xmin = axisX.getRange().getMin();
            double xmax = axisX.getRange().getMax();

            xmin = (xmin - oldOffset) * newScale + oldOffset;
            xmax = (xmax - oldOffset) * newScale + oldOffset;

            chart.getAxisX().setRange(new Range(xmin * scale, xmax * scale));
        }
    }
    
    class ScrollAxis extends AxisLinear {

        @Override
        protected List<LabeledValue> getLabels(final double resolution) {
            List<LabeledValue> collect = new LinkedList<LabeledValue>();
            if (resolution <= 0) {
                return collect;
            }

            Range domain = this.getRange();
            double min = domain.getMin();
            double max = domain.getMax();
            double range = max - min;

            // k = log_2(range/10)
            // todo: what is the dea behind that?!
            double k = (Math.log(range / 10)) / Math.log(2);
            double y = Math.pow(2.0, Math.ceil(k));

            double minorTickSpacing = y; //this.m_minorTickSpacing;
            double majorTickSpacing = 5.0 * minorTickSpacing;//this.m_majorTickSpacing;

            double minorTickOffset = Math.ceil(min / minorTickSpacing);
            double majorTickOffset = Math.ceil(min / majorTickSpacing);

            double minorValue = minorTickOffset * minorTickSpacing;
            double majorValue = majorTickOffset * majorTickSpacing;
            double value = Math.min(minorValue, majorValue);
            
            double loopStop = 0;
            
            while (value <= max && loopStop < 100) {
                LabeledValue label = new LabeledValue();

                label.setLabel(this.getFormatter().format(value));
                label.setValue((value - min) / range);

                label.setMajorTick(minorValue >= majorValue);

                if (minorValue <= value) {
                    minorValue += minorTickSpacing;
                }

                if (majorValue <= value) {
                    majorValue += majorTickSpacing;
                }

                value = Math.min(minorValue, majorValue);

                collect.add(label);
                loopStop++;
            }//end while

            return collect;
        }//end getLabels
    }//end class ScrollAxis
    
    class DynamicRangePolicy extends ARangePolicy {

        public DynamicRangePolicy(Range range) {
            super(range);
        }

        @Override
        public double getMax(double chartMin, double chartMax) {
            if (chartMax < this.getRange().getMin()) {
                return chartMax + this.getRange().getExtent();
            }

            return Math.max(this.getRange().getMax(), chartMax);
        }

        @Override
        public double getMin(double chartMin, double chartMax) {
            return getMax(chartMin, chartMax) - this.getRange().getExtent();
        }
    }//end class DynamicRangePolicy
    
    public class PointPainterCoords extends APointPainter 
    {
        int radius = 5;

        @Override
        public void paintPoint(final int absoluteX, final int absoluteY, final int nextX,
            final int nextY, final Graphics g, final ITracePoint2D point) {
            g.drawOval(absoluteX - this.radius, absoluteY - this.radius, 2 * this.radius, 2 * this.radius);
            g.drawString(point.getX() + " : " + point.getY(), absoluteX, absoluteY - 2 * this.radius);
        }
    }
    
    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jSplitPane = new javax.swing.JSplitPane();
        scrollList = new javax.swing.JScrollPane();
        panelList = new javax.swing.JPanel();
        chart = new info.monitorenter.gui.chart.Chart2D();

        setLayout(new java.awt.BorderLayout());

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
            .addGap(0, 416, Short.MAX_VALUE)
        );
        chartLayout.setVerticalGroup(
            chartLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );

        jSplitPane.setRightComponent(chart);

        add(jSplitPane, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private info.monitorenter.gui.chart.Chart2D chart;
    private javax.swing.JSplitPane jSplitPane;
    private javax.swing.JPanel panelList;
    private javax.swing.JScrollPane scrollList;
    // End of variables declaration//GEN-END:variables
}
