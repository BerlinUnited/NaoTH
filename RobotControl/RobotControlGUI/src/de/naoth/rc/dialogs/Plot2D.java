/*
 * Plot2D.java
 *
 * Created on 15.02.2011, 00:38:59
 */
package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.PNGExportFileType;
import de.naoth.rc.components.PlainPDFExportFileType;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.PlotDataManagerImpl;
import de.naoth.rc.core.messages.CommonTypes.DoubleVector2;
import de.naoth.rc.core.messages.Messages.PlotStroke2D;
import de.naoth.rc.core.messages.Messages.Plots;
import de.naoth.rc.core.server.Command;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;
import org.freehep.graphicsio.emf.EMFExportFileType;
import org.freehep.graphicsio.java.JAVAExportFileType;
import org.freehep.graphicsio.ps.EPSExportFileType;
import org.freehep.graphicsio.svg.SVGExportFileType;
import org.freehep.util.export.ExportDialog;

/**
 *
 * @author Heinrich Mellmann
 */
public class Plot2D extends AbstractDialog {

    @RCDialog(category = RCDialog.Category.View, name = "Plot 2D")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<Plot2D> {

        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static GenericManagerFactory genericManagerFactory;
    }

    private final Command commandGetPlotMotion = new Command("Motion:DebugPlot:get");
    private final Command commandGetPlotCognition = new Command("Cognition:DebugPlot:get");

    private final PlotDataHandler plotDataHandler = new PlotDataHandler();

    public Plot2D() {
        initComponents();
        
        this.plotPanel.setDontScrollOnStaticRegionData(cbDontScrollOnStaticRegionData.isSelected());
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
        btReceiveData = new javax.swing.JToggleButton();
        btClear = new javax.swing.JButton();
        btClearData = new javax.swing.JButton();
        cbDontScrollOnStaticRegionData = new javax.swing.JCheckBox();
        btFitToData = new javax.swing.JButton();
        cbShowGrid = new javax.swing.JCheckBox();
        btExportCSV = new javax.swing.JButton();
        btExportGraph = new javax.swing.JButton();
        plotPanel = new de.naoth.rc.components.PlotPanel();

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
        cbDontScrollOnStaticRegionData.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbDontScrollOnStaticRegionDataActionPerformed(evt);
            }
        });
        jToolBar1.add(cbDontScrollOnStaticRegionData);

        btFitToData.setText("Fit to Data");
        btFitToData.setFocusable(false);
        btFitToData.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btFitToData.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btFitToData.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btFitToDataActionPerformed(evt);
            }
        });
        jToolBar1.add(btFitToData);

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

        btExportCSV.setText("export csv");
        btExportCSV.setFocusable(false);
        btExportCSV.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btExportCSV.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btExportCSV.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btExportCSVActionPerformed(evt);
            }
        });
        jToolBar1.add(btExportCSV);

        btExportGraph.setText("export graph");
        btExportGraph.setFocusable(false);
        btExportGraph.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btExportGraph.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btExportGraph.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btExportGraphActionPerformed(evt);
            }
        });
        jToolBar1.add(btExportGraph);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(plotPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(plotPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 356, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btReceiveDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveDataActionPerformed

        if (btReceiveData.isSelected()) {
            if (Plugin.parent.checkConnected()) {
                Plugin.genericManagerFactory.getManager(commandGetPlotMotion).addListener(plotDataHandler);
                Plugin.genericManagerFactory.getManager(commandGetPlotCognition).addListener(plotDataHandler);
                this.plotPanel.setAutoScroll(true);
            } else {
                btReceiveData.setSelected(false);
            }
        } else {
            Plugin.genericManagerFactory.getManager(commandGetPlotMotion).removeListener(plotDataHandler);
            Plugin.genericManagerFactory.getManager(commandGetPlotCognition).removeListener(plotDataHandler);
            this.plotPanel.setAutoScroll(false);
            //chart.enablePointHighlighting(true);
        }
}//GEN-LAST:event_btReceiveDataActionPerformed

    private void btClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btClearActionPerformed
        this.plotPanel.clear();
    }//GEN-LAST:event_btClearActionPerformed

  private void btClearDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btClearDataActionPerformed
        this.plotPanel.clearTracePoints();
  }//GEN-LAST:event_btClearDataActionPerformed

    private void btFitToDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btFitToDataActionPerformed
        this.plotPanel.fitToData();
    }//GEN-LAST:event_btFitToDataActionPerformed

    private void cbShowGridActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbShowGridActionPerformed
        this.plotPanel.setPaintGrid(this.cbShowGrid.isSelected());
    }//GEN-LAST:event_cbShowGridActionPerformed

    private void btExportCSVActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btExportCSVActionPerformed
        this.plotPanel.exportCSV();
    }//GEN-LAST:event_btExportCSVActionPerformed

    private void btExportGraphActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btExportGraphActionPerformed
        ExportDialog export = new ExportDialog("FieldViewer", false);

        // add the image types for export
        export.addExportFileType(new SVGExportFileType());
        export.addExportFileType(new PlainPDFExportFileType());
        export.addExportFileType(new EPSExportFileType());
        export.addExportFileType(new EMFExportFileType());
        export.addExportFileType(new JAVAExportFileType());
        export.addExportFileType(new PNGExportFileType());

        export.showExportDialog(this, "Export view as ...", this.plotPanel.getChart(), "export");
    }//GEN-LAST:event_btExportGraphActionPerformed

    private void cbDontScrollOnStaticRegionDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbDontScrollOnStaticRegionDataActionPerformed
        this.plotPanel.setDontScrollOnStaticRegionData(cbDontScrollOnStaticRegionData.isSelected());
    }//GEN-LAST:event_cbDontScrollOnStaticRegionDataActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btClear;
    private javax.swing.JButton btClearData;
    private javax.swing.JButton btExportCSV;
    private javax.swing.JButton btExportGraph;
    private javax.swing.JButton btFitToData;
    private javax.swing.JToggleButton btReceiveData;
    private javax.swing.JCheckBox cbDontScrollOnStaticRegionData;
    private javax.swing.JCheckBox cbShowGrid;
    private javax.swing.JToolBar jToolBar1;
    private de.naoth.rc.components.PlotPanel plotPanel;
    // End of variables declaration//GEN-END:variables

    class PlotDataHandler implements ObjectListener<byte[]> {

        @Override
        public void newObjectReceived(byte[] object) {
            try {
                if (object != null) {
                    Plots plots = Plots.parseFrom(object);
                    for (PlotStroke2D stroke : plots.getPlotstrokesList()) {
                        for (DoubleVector2 point : stroke.getPointsList()) {
                            Plot2D.this.plotPanel.addValue(stroke.getName(), point.getX(), point.getY());
                        }
                    }
                }
            } catch (InvalidProtocolBufferException ex) {
                Logger.getLogger(PlotDataManagerImpl.class.getName()).log(Level.SEVERE, new String(object), ex);
            }
        }

        @Override
        public void errorOccured(String cause) {
            Logger.getLogger(PlotDataManagerImpl.class.getName()).log(Level.SEVERE, cause);
            dispose();
        }
    }//end class DataHandlerPrint

    @Override
    public void dispose() {
        btReceiveData.setSelected(false);
        Plugin.genericManagerFactory.getManager(commandGetPlotMotion).removeListener(plotDataHandler);
        Plugin.genericManagerFactory.getManager(commandGetPlotCognition).removeListener(plotDataHandler);
    }

}//end class Plot2D
