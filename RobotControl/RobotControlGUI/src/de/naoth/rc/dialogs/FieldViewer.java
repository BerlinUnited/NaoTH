/*
 * FieldViewer.java
 *
 * Created on 1. Mai 2008, 00:02
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.PNGExportFileType;
import de.naoth.rc.components.PlainPDFExportFileType;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.drawings.Drawable;
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.DrawingOnField;
import de.naoth.rc.drawings.DrawingsContainer;
import de.naoth.rc.drawings.FieldDrawingBDR;
import de.naoth.rc.drawings.FieldDrawingS3D2011;
import de.naoth.rc.drawings.FieldDrawingSPL2012;
import de.naoth.rc.drawings.FieldDrawingSPL2013;
import de.naoth.rc.drawings.FieldDrawingSPL2013BlackWhite;
import de.naoth.rc.drawings.LocalFieldDrawing;
import de.naoth.rc.drawings.RadarDrawing;
import de.naoth.rc.drawings.StrokePlot;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.manager.DebugDrawingManager;
import de.naoth.rc.manager.ImageManagerBottom;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.drawings.FieldDrawingSPL3x4;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.manager.DebugDrawingManagerMotion;
import de.naoth.rc.manager.PlotDataManager;
import de.naoth.rc.math.Vector2D;
import de.naoth.rc.messages.Messages.PlotItem;
import de.naoth.rc.messages.Messages.Plots;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;
import javax.imageio.ImageIO;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;
import org.freehep.graphicsio.emf.EMFExportFileType;
import org.freehep.graphicsio.java.JAVAExportFileType;
import org.freehep.graphicsio.ps.EPSExportFileType;
import org.freehep.graphicsio.svg.SVGExportFileType;
import org.freehep.util.export.ExportDialog;

/**
 *
 * @author  Heinrich Mellmann
 */
public class FieldViewer extends AbstractDialog implements ActionListener
{

    @RCDialog(category = RCDialog.Category.View, name = "Field")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<FieldViewer> {

        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static DebugDrawingManager debugDrawingManager;
        @InjectPlugin
        public static DebugDrawingManagerMotion debugDrawingManagerMotion;
        @InjectPlugin
        public static PlotDataManager plotDataManager;
        @InjectPlugin
        public static ImageManagerBottom imageManager;
        @InjectPlugin
        public static DrawingEventManager drawingEventManager;
        @InjectPlugin
        public static LogFileEventManager logFileEventManager;
    }//end Plugin

  private final Timer drawingTimer;
  // drawing buffer for concurrent access; every "source" gets its own "buffer"
  private ConcurrentHashMap<Class<?>, Drawable> drawingBuffers = new ConcurrentHashMap<>();

  private final PlotDataListener plotDataListener;
  private final StrokePlot strokePlot;

  // create new classes, so the received drawings are stored into different buffers
  private class DrawingsListenerMotion extends DrawingsListener{}
  private class DrawingsListenerCognition extends DrawingsListener{}
  private final DrawingsListenerMotion drawingsListenerMotion = new DrawingsListenerMotion();
  private final DrawingsListenerCognition drawingsListenerCognition = new DrawingsListenerCognition();
  
  // this is ued as an exportbuffer when the field issaved as image
  private BufferedImage exportBuffer = null;
  
  // TODO: this is a hack
  private static de.naoth.rc.components.DynamicCanvasPanel canvasExport = null;
  public static de.naoth.rc.components.DynamicCanvasPanel getCanvas() {
      return canvasExport;
  }
  
  public FieldViewer()
  {
    initComponents();
    
    this.cbBackground.setModel(
        new javax.swing.DefaultComboBoxModel(
        new Drawable[] 
        { 
            new FieldDrawingBDR(),
            new FieldDrawingSPL2013(),
            new FieldDrawingSPL2012(),
            new FieldDrawingS3D2011(),
            new FieldDrawingSPL3x4(),
            new LocalFieldDrawing(),
            new RadarDrawing(),
            new FieldDrawingSPL2013BlackWhite()
        }
    ));
    
    this.plotDataListener = new PlotDataListener();
    
    this.fieldCanvas.setBackgroundDrawing((Drawable)this.cbBackground.getSelectedItem());
    this.fieldCanvas.setToolTipText("");
    this.fieldCanvas.setFitToViewport(this.btFitToView.isSelected());
    canvasExport = this.fieldCanvas;
    
    Plugin.drawingEventManager.addListener((Drawable drawing, Object src) -> {
        // add drawing of the source to the drawing buffer
        drawingBuffers.put(src.getClass(), drawing);
    });
    
    // intialize the field
    resetView();
    this.fieldCanvas.setAntializing(btAntializing.isSelected());
    this.fieldCanvas.repaint();
    
    
    this.fieldCanvas.addMouseListener(new MouseAdapter() {
        @Override
        public void mouseClicked(MouseEvent e) {
            if (e.getClickCount() == 2 && !e.isConsumed()) {
                e.consume();
                fieldCanvas.fitToViewport();
            }
        }
    });

    this.strokePlot = new StrokePlot(300);
    // schedules canvas drawing at a fixed rate, should prevent "flickering"
    this.drawingTimer = new Timer(300, this);
    this.drawingTimer.start();
  }


  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPopupMenu = new javax.swing.JPopupMenu();
        jMenuItemExport = new javax.swing.JMenuItem();
        coordsPopup = new javax.swing.JDialog();
        jToolBar1 = new javax.swing.JToolBar();
        btReceiveDrawings = new javax.swing.JToggleButton();
        btClean = new javax.swing.JButton();
        cbBackground = new javax.swing.JComboBox();
        btRotate = new javax.swing.JButton();
        btFitToView = new javax.swing.JToggleButton();
        btAntializing = new javax.swing.JCheckBox();
        btCollectDrawings = new javax.swing.JCheckBox();
        cbExportOnDrawing = new javax.swing.JCheckBox();
        btTrace = new javax.swing.JCheckBox();
        jSlider1 = new javax.swing.JSlider();
        drawingPanel = new javax.swing.JPanel();
        fieldCanvas = new de.naoth.rc.components.DynamicCanvasPanel();

        jMenuItemExport.setText("Export");
        jMenuItemExport.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemExportActionPerformed(evt);
            }
        });
        jPopupMenu.add(jMenuItemExport);

        javax.swing.GroupLayout coordsPopupLayout = new javax.swing.GroupLayout(coordsPopup.getContentPane());
        coordsPopup.getContentPane().setLayout(coordsPopupLayout);
        coordsPopupLayout.setHorizontalGroup(
            coordsPopupLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 114, Short.MAX_VALUE)
        );
        coordsPopupLayout.setVerticalGroup(
            coordsPopupLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 38, Short.MAX_VALUE)
        );

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        btReceiveDrawings.setText("Receive");
        btReceiveDrawings.setFocusable(false);
        btReceiveDrawings.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReceiveDrawings.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReceiveDrawings.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveDrawingsActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceiveDrawings);

        btClean.setText("Clean");
        btClean.setFocusable(false);
        btClean.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btClean.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btClean.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btCleanActionPerformed(evt);
            }
        });
        jToolBar1.add(btClean);

        cbBackground.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "SPL2013", "SPL2012", "BDR", "S3D2011", "RADAR", "LOCAL" }));
        cbBackground.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbBackgroundActionPerformed(evt);
            }
        });
        jToolBar1.add(cbBackground);

        btRotate.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/rotate_ccw.png"))); // NOI18N
        btRotate.setToolTipText("Rotate the coordinates by 90°");
        btRotate.setFocusable(false);
        btRotate.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btRotate.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btRotate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btRotateActionPerformed(evt);
            }
        });
        jToolBar1.add(btRotate);

        btFitToView.setIcon(new javax.swing.ImageIcon(getClass().getResource("/org/freehep/swing/images/0_MoveCursor.gif"))); // NOI18N
        btFitToView.setSelected(true);
        btFitToView.setToolTipText("auto-zoom canvas on resizing and rotation");
        btFitToView.setFocusable(false);
        btFitToView.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btFitToView.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btFitToView.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btFitToViewActionPerformed(evt);
            }
        });
        jToolBar1.add(btFitToView);

        btAntializing.setText("Antialiazing");
        btAntializing.setFocusable(false);
        btAntializing.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        btAntializing.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btAntializing.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btAntializingActionPerformed(evt);
            }
        });
        jToolBar1.add(btAntializing);

        btCollectDrawings.setText("Collect");
        btCollectDrawings.setFocusable(false);
        btCollectDrawings.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        btCollectDrawings.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btCollectDrawings.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btCollectDrawingsActionPerformed(evt);
            }
        });
        jToolBar1.add(btCollectDrawings);

        cbExportOnDrawing.setText("ExportOnDrawing");
        cbExportOnDrawing.setFocusable(false);
        jToolBar1.add(cbExportOnDrawing);

        btTrace.setText("Trace");
        btTrace.setFocusable(false);
        btTrace.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        btTrace.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        jToolBar1.add(btTrace);

        jSlider1.setMaximum(255);
        jSlider1.setValue(247);
        jSlider1.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSlider1StateChanged(evt);
            }
        });
        jToolBar1.add(jSlider1);

        drawingPanel.setBackground(new java.awt.Color(247, 247, 247));
        drawingPanel.setLayout(new java.awt.BorderLayout());

        fieldCanvas.setBackground(new java.awt.Color(247, 247, 247));
        fieldCanvas.setComponentPopupMenu(jPopupMenu);
        fieldCanvas.setOffsetX(350.0);
        fieldCanvas.setOffsetY(200.0);
        fieldCanvas.setScale(0.07);

        javax.swing.GroupLayout fieldCanvasLayout = new javax.swing.GroupLayout(fieldCanvas);
        fieldCanvas.setLayout(fieldCanvasLayout);
        fieldCanvasLayout.setHorizontalGroup(
            fieldCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 674, Short.MAX_VALUE)
        );
        fieldCanvasLayout.setVerticalGroup(
            fieldCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 363, Short.MAX_VALUE)
        );

        drawingPanel.add(fieldCanvas, java.awt.BorderLayout.CENTER);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(drawingPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(drawingPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btReceiveDrawingsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveDrawingsActionPerformed
      if(btReceiveDrawings.isSelected())
      {
        if(Plugin.parent.checkConnected())
        {
          Plugin.debugDrawingManager.addListener(drawingsListenerCognition);
          Plugin.debugDrawingManagerMotion.addListener(drawingsListenerMotion);
          Plugin.plotDataManager.addListener(plotDataListener);
        }
        else
        {
          btReceiveDrawings.setSelected(false);
        }
      }
      else
      {
        Plugin.debugDrawingManager.removeListener(drawingsListenerCognition);
        Plugin.debugDrawingManagerMotion.removeListener(drawingsListenerMotion);
        Plugin.plotDataManager.removeListener(plotDataListener);
      }
    }//GEN-LAST:event_btReceiveDrawingsActionPerformed

private void jMenuItemExportActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemExportActionPerformed
  
  ExportDialog export = new ExportDialog("FieldViewer", false);
  
  // add the image types for export
  export.addExportFileType(new SVGExportFileType());
  export.addExportFileType(new PlainPDFExportFileType());
  export.addExportFileType(new EPSExportFileType());
  export.addExportFileType(new EMFExportFileType());
  export.addExportFileType(new JAVAExportFileType());
  export.addExportFileType(new PNGExportFileType());
  
  export.showExportDialog(this, "Export view as ...", this.fieldCanvas, "export");
}//GEN-LAST:event_jMenuItemExportActionPerformed



private void btAntializingActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btAntializingActionPerformed
  this.fieldCanvas.setAntializing(btAntializing.isSelected());
  this.fieldCanvas.repaint();
}//GEN-LAST:event_btAntializingActionPerformed

private void btCollectDrawingsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btCollectDrawingsActionPerformed
  // TODO add your handling code here:
}//GEN-LAST:event_btCollectDrawingsActionPerformed

private void jSlider1StateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSlider1StateChanged
  int v = this.jSlider1.getValue();
  this.fieldCanvas.setBackground(new Color(v,v,v));
  this.drawingPanel.repaint();
}//GEN-LAST:event_jSlider1StateChanged

    private void btCleanActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btCleanActionPerformed
        this.strokePlot.clear();
        resetView();
        this.fieldCanvas.repaint();
    }//GEN-LAST:event_btCleanActionPerformed

    private void cbBackgroundActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbBackgroundActionPerformed
    {//GEN-HEADEREND:event_cbBackgroundActionPerformed
        this.fieldCanvas.setBackgroundDrawing((Drawable)this.cbBackground.getSelectedItem());
        this.fieldCanvas.repaint();
        // TODO: should this be inside the DynamicCanvasPanel?
        if(this.fieldCanvas.isFitToViewport()) {
            this.fieldCanvas.fitToViewport();
        }
    }//GEN-LAST:event_cbBackgroundActionPerformed

    private void btRotateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btRotateActionPerformed
        this.fieldCanvas.setRotation(this.fieldCanvas.getRotation() + Math.PI*0.5);
        // TODO: should this be inside the DynamicCanvasPanel?
        if(this.fieldCanvas.isFitToViewport()) {
            this.fieldCanvas.fitToViewport();
        }
        this.fieldCanvas.repaint();
    }//GEN-LAST:event_btRotateActionPerformed


    private void btFitToViewActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btFitToViewActionPerformed
        fieldCanvas.setFitToViewport(this.btFitToView.isSelected());
    }//GEN-LAST:event_btFitToViewActionPerformed

  
  final void resetView()
  {
    this.fieldCanvas.getDrawingList().clear();
    if(btTrace.isSelected()) {
        this.fieldCanvas.getDrawingList().add(this.strokePlot);
    }
    
    drawingBuffers.clear();
  }//end resetView

  private void exportCanvasToPNG() {
      
      long l = java.lang.System.currentTimeMillis();
      File file = new File("./fieldViewerExport-"+l+".png");
      
      if(exportBuffer == null || exportBuffer.getWidth() != this.fieldCanvas.getWidth() || exportBuffer.getHeight() != this.fieldCanvas.getHeight()) {
        exportBuffer = new BufferedImage(this.fieldCanvas.getWidth(), this.fieldCanvas.getHeight(), BufferedImage.TYPE_INT_ARGB);
      }
      Graphics2D g2d = exportBuffer.createGraphics();
      this.fieldCanvas.paintAll(g2d);
      
      try {
        ImageIO.write(exportBuffer, "PNG", file);
      } catch(IOException ex) {
          ex.printStackTrace(System.err);
      }
  }
  
  private class DrawingsListener implements ObjectListener<DrawingsContainer>
  {
    @Override
    public void newObjectReceived(DrawingsContainer objectList)
    {
      if(objectList != null)
      {
        DrawingCollection drawingCollection = objectList.get(DrawingOnField.class);
        if(drawingCollection == null || drawingCollection.isEmpty()) {
            return;
        }
        
        // add received drawings to buffer
        drawingBuffers.put(this.getClass(), drawingCollection);

        if(cbExportOnDrawing.isSelected()) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    exportCanvasToPNG();
                }
            });
        }
      }
    }

    @Override
    public void errorOccured(String cause)
    {
      btReceiveDrawings.setSelected(false);
      Plugin.debugDrawingManager.removeListener(this);
      Plugin.debugDrawingManagerMotion.removeListener(this);
    }
  }
  
  

  class PlotDataListener implements ObjectListener<Plots>
  {
    @Override
    public void errorOccured(String cause)
    {
      btReceiveDrawings.setSelected(false);
      Plugin.plotDataManager.removeListener(this);
    }

    @Override
    public void newObjectReceived(Plots data)
    {
      if (data == null) return;

      for(PlotItem item : data.getPlotsList())
      {
        if(item.getType() == PlotItem.PlotType.Plot2D
          && item.hasX() && item.hasY())
        {
          strokePlot.addStroke(item.getName(), Color.blue);
          strokePlot.setEnabled(item.getName(), true);
          strokePlot.plot(item.getName(), new Vector2D(item.getX(), item.getY()));
        }
        else if(item.getType() == PlotItem.PlotType.Origin2D
          && item.hasX() && item.hasY() && item.hasRotation())
        {
          strokePlot.setRotation(item.getRotation());
        }
      } //end for
    }//end newObjectReceived
  }//end class PlotDataListener
    
    class DrawingBuffer extends DrawingCollection
    {
        private int maxNumberOfEnties;
        public DrawingBuffer(int maxNumberOfEnties) {
            this.maxNumberOfEnties = maxNumberOfEnties;
        }
        
        @Override
        public void add(Drawable d) {
            if(maxNumberOfEnties > 0 && this.drawables.size() >= maxNumberOfEnties) {
                this.drawables.remove(0);
            }
            super.add(d);
        }
        
        public void clear() {
            this.drawables.clear();
        }
    }
    
  @Override
  public void dispose()
  {
    // remove all the registered listeners
    Plugin.debugDrawingManager.removeListener(drawingsListenerCognition);
    Plugin.debugDrawingManagerMotion.removeListener(drawingsListenerMotion);
    Plugin.plotDataManager.removeListener(plotDataListener);
  }
  
    @Override
    public void actionPerformed(ActionEvent e) {
        // clear old field drawings
        if(!btCollectDrawings.isSelected()) {
            fieldCanvas.getDrawingList().clear();
            // removed it! in order to continue drawing strokes, we have to "re-add" it
            if(btTrace.isSelected()) {
                this.fieldCanvas.getDrawingList().add(this.strokePlot);
            }
        }
        // add buffered drawings to field drawing list
        drawingBuffers.forEach((s,b)->{fieldCanvas.getDrawingList().add(b);});
        // re-draw field
        fieldCanvas.repaint();
    }

  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBox btAntializing;
    private javax.swing.JButton btClean;
    private javax.swing.JCheckBox btCollectDrawings;
    private javax.swing.JToggleButton btFitToView;
    private javax.swing.JToggleButton btReceiveDrawings;
    private javax.swing.JButton btRotate;
    private javax.swing.JCheckBox btTrace;
    private javax.swing.JComboBox cbBackground;
    private javax.swing.JCheckBox cbExportOnDrawing;
    private javax.swing.JDialog coordsPopup;
    private javax.swing.JPanel drawingPanel;
    private de.naoth.rc.components.DynamicCanvasPanel fieldCanvas;
    private javax.swing.JMenuItem jMenuItemExport;
    private javax.swing.JPopupMenu jPopupMenu;
    private javax.swing.JSlider jSlider1;
    private javax.swing.JToolBar jToolBar1;
    // End of variables declaration//GEN-END:variables
}
