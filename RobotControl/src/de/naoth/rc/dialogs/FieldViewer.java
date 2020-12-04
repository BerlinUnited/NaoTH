/*
 * FieldViewer.java
 *
 * Created on 1. Mai 2008, 00:02
 */
package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
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
import de.naoth.rc.drawings.FieldDrawingS3D2011;
import de.naoth.rc.drawings.FieldDrawingSPL2012;
import de.naoth.rc.drawings.FieldDrawingSPL2013;
import de.naoth.rc.drawings.FieldDrawingNaoTHLabor;
import de.naoth.rc.drawings.FieldDrawingSPL2013BlackWhite;
import de.naoth.rc.drawings.LocalFieldDrawing;
import de.naoth.rc.drawings.RadarDrawing;
import de.naoth.rc.drawings.StrokePlot;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.manager.DebugDrawingManager;
import de.naoth.rc.manager.ImageManagerBottom;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.drawings.Circle;
import de.naoth.rc.drawings.FieldDrawingSPL2020;
import de.naoth.rc.drawings.FieldDrawingSPL3x4;
import de.naoth.rc.drawings.FieldDrawingSPLAspen;
import de.naoth.rc.drawings.FillOval;
import de.naoth.rc.drawings.Line;
import de.naoth.rc.drawings.Pen;
import de.naoth.rc.drawings.Robot;
import de.naoth.rc.logmanager.BlackBoard;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.logmanager.LogFrameListener;
import de.naoth.rc.manager.DebugDrawingManagerMotion;
import de.naoth.rc.manager.PlotDataManager;
import de.naoth.rc.math.Matrix3D;
import de.naoth.rc.math.Pose2D;
import de.naoth.rc.math.Pose3D;
import de.naoth.rc.math.Vector2D;
import de.naoth.rc.math.Vector3D;
import de.naoth.rc.core.messages.CommonTypes;
import de.naoth.rc.core.messages.CommonTypes.DoubleVector3;
import de.naoth.rc.core.messages.CommonTypes.IntVector2;
import de.naoth.rc.core.messages.CommonTypes.LineSegment;
import de.naoth.rc.core.messages.FrameworkRepresentations.RobotInfo;
import de.naoth.rc.core.messages.Messages.PlotItem;
import de.naoth.rc.core.messages.Messages.Plots;
import de.naoth.rc.core.messages.Representations;
import de.naoth.rc.core.messages.Representations.MultiBallPercept;
import de.naoth.rc.core.messages.Representations.RansacCirclePercept2018;
import de.naoth.rc.core.messages.Representations.RansacLinePercept;
import de.naoth.rc.core.messages.Representations.ShortLinePercept;
import de.naoth.rc.core.messages.RobotPoseOuterClass.RobotPose;
import de.naoth.rc.core.messages.TeamMessageOuterClass;
import de.naoth.rc.core.messages.TeamMessageOuterClass.TeamMessage;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.datatransfer.StringSelection;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.swing.SwingUtilities;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;
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
public class FieldViewer extends AbstractDialog
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

  // drawing buffer for concurrent access; every "source" gets its own "buffer"
  private final MultiSourceDrawingCollection drawings = new MultiSourceDrawingCollection();

  private final PlotDataListener plotDataListener;
  private final StrokePlot strokePlot;
  
  private final LogFrameListener logFrameListener;

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
            new FieldDrawingSPL2020(),
            new FieldDrawingSPL2013(),
            new FieldDrawingSPL2012(),
            new FieldDrawingS3D2011(),
            new FieldDrawingNaoTHLabor(),
            new FieldDrawingSPLAspen(),
            new FieldDrawingSPL3x4(),
            new LocalFieldDrawing(),
            new RadarDrawing(),
            new FieldDrawingSPL2013BlackWhite()
        }
    ));
    
    this.plotDataListener = new PlotDataListener();
    
    this.logFrameListener = new LogFrameDrawer();
    
    this.fieldCanvas.setBackgroundDrawing((Drawable)this.cbBackground.getSelectedItem());
    this.fieldCanvas.setToolTipText("");
    this.fieldCanvas.setFitToViewport(this.btFitToView.isSelected());
    this.fieldCanvas.addDrawing(drawings);
    
    canvasExport = this.fieldCanvas;
    
    Plugin.drawingEventManager.addListener((Drawable drawing, Object src) -> {
        // add drawing of the source to the drawing buffer
        drawings.add(src.getClass(), drawing);
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
    //this.drawingTimer = new Timer(300, this);
    //this.drawingTimer.start();
    // remember the location of the mouse
    jPopupMenu.addPopupMenuListener(new PopupMenuListener() {
        @Override
        public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
            Point2D.Double p = convertToFieldCoordinates(fieldCanvas.getMousePosition());
            jMenuItemCopyCoords.setToolTipText(String.format("%.1f; %.1f", p.x, p.y));
        }

        @Override
        public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {}

        @Override
        public void popupMenuCanceled(PopupMenuEvent e) {}
    });
    // middle click with the mouse on the field copies the field coordinates to the clipboard
    fieldCanvas.addMouseListener(new MouseAdapter() {
        @Override
        public void mousePressed(MouseEvent e) {
            if(e.getButton() == MouseEvent.BUTTON2) {
                // "save" Coordinates to clipboard
                Point2D.Double p = convertToFieldCoordinates(e.getPoint());
                StringSelection ss = new StringSelection(String.format("%.1f; %.1f", p.x, p.y));
                Toolkit.getDefaultToolkit().getSystemClipboard().setContents(ss, ss);
            }
        }
    });
  }
  
    /**
     * Convertes the given relative point of the field to "real" field coordinates.
     * 
     * @param p the relative ui point
     * @return "real" field coordinates.
     */
    private Point2D.Double convertToFieldCoordinates(Point p) {
        Point.Double o = new Point.Double(p.getX(), p.getY());
        Point2D.Double r = fieldCanvas.canvasCoordinatesToInternal(o);
        return r;
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
        jMenuItemCopyCoords = new javax.swing.JMenuItem();
        coordsPopup = new javax.swing.JDialog();
        jToolBar1 = new javax.swing.JToolBar();
        btReceiveDrawings = new javax.swing.JToggleButton();
        btLog = new javax.swing.JToggleButton();
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

        jMenuItemCopyCoords.setText("Copy Coordinates");
        jMenuItemCopyCoords.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemCopyCoordsActionPerformed(evt);
            }
        });
        jPopupMenu.add(jMenuItemCopyCoords);

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

        btLog.setText("Log");
        btLog.setFocusable(false);
        btLog.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btLog.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btLog.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btLogActionPerformed(evt);
            }
        });
        jToolBar1.add(btLog);

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

        cbBackground.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "SPL2013", "SPL2012", "S3D2011", "RADAR", "LOCAL" }));
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

    private void btLogActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btLogActionPerformed
        if(btLog.isSelected())
        {
            Plugin.logFileEventManager.addListener(logFrameListener);
        }
        else
        {
            Plugin.logFileEventManager.removeListener(logFrameListener);
        }
    }//GEN-LAST:event_btLogActionPerformed

    private void jMenuItemCopyCoordsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemCopyCoordsActionPerformed
        // "save" Coordinates to clipboard
        StringSelection ss = new StringSelection(jMenuItemCopyCoords.getToolTipText());
        Toolkit.getDefaultToolkit().getSystemClipboard().setContents(ss, ss);
    }//GEN-LAST:event_jMenuItemCopyCoordsActionPerformed

  final void resetView()
  {
    this.fieldCanvas.getDrawingList().clear();
    
    this.drawings.clear();
    this.fieldCanvas.addDrawing(drawings);
    
    if(btTrace.isSelected()) {
        this.fieldCanvas.getDrawingList().add(this.strokePlot);
    }
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
        drawings.add(this.getClass(), drawingCollection);

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
  
  class LogFrameDrawer implements LogFrameListener
  {
      
        Pose2D robotPose = null;
        Pose3D cmBottom = null;
        Pose3D cmTop = null;
        final double f = (0.5*640.0) / Math.tan(0.5 * 60.9/180.0*Math.PI);
        
        private void readCameraMatrix(BlackBoard b) 
        {
            cmBottom = null;
            cmTop = null;
            
            try {
                Representations.CameraMatrix cm = Representations.CameraMatrix.parseFrom(b.get("CameraMatrix").getData());
                Representations.CameraMatrix cmt = Representations.CameraMatrix.parseFrom(b.get("CameraMatrixTop").getData());

                cmBottom = new Pose3D(
                    toVector(cm.getPose().getTranslation()),
                    new Matrix3D(
                        toVector(cm.getPose().getRotation(0)),
                        toVector(cm.getPose().getRotation(1)),
                        toVector(cm.getPose().getRotation(2)))
                );

                cmTop = new Pose3D(
                    toVector(cmt.getPose().getTranslation()),
                    new Matrix3D(
                        toVector(cmt.getPose().getRotation(0)),
                        toVector(cmt.getPose().getRotation(1)),
                        toVector(cmt.getPose().getRotation(2)))
                );
                
            } catch(InvalidProtocolBufferException ex) {
                ex.printStackTrace(System.err);
            }      
        }
        
        private void drawTeamMessages(BlackBoard b, DrawingCollection dc)
        {   
            Optional<String> ownBodyID = getOwnBodyID(b);
            
            LogDataFrame teamMessageFrame = b.get("TeamMessage");
            if(teamMessageFrame != null)
            {
                try {
                    TeamMessage teamMessage = TeamMessage.parseFrom(teamMessageFrame.getData());
                    for(int i=0; i < teamMessage.getDataCount(); i++) 
                    {
                       TeamMessage.Data robotMsg = teamMessage.getData(i);
                        
                       SPLMessage splMsg = SPLMessage.parseFrom(robotMsg);
                       
                       splMsg.ballAge /= 1000.0; // the ballAge in the log files is in milliseconds
                       
                       if((splMsg.user.hasIsPenalized() && !splMsg.user.getIsPenalized()) || (splMsg.user.hasRobotState() && splMsg.user.getRobotState() != TeamMessageOuterClass.RobotState.penalized)) {
                            boolean isOwnMsg = false;
                            if(ownBodyID.isPresent())
                            {
                                isOwnMsg = ownBodyID.get().equals(splMsg.user.getBodyID());
                            }
                            if(isOwnMsg) {
                                robotPose = new Pose2D(splMsg.pose_x, splMsg.pose_y, splMsg.pose_a);
                            }

                            splMsg.draw(dc, isOwnMsg ? Color.red : Color.black, false);
                       }
                        
                    }
                } catch (InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            
            drawScanEdgelPercept(robotPose, b.get("GoalPercept"), b.get("ScanLineEdgelPercept"), cmBottom, dc, Color.red);
            drawScanEdgelPercept(robotPose, b.get("GoalPerceptTop"), b.get("ScanLineEdgelPerceptTop"), cmTop, dc, Color.blue);
        }
        
        private void drawFieldPercept(BlackBoard b, DrawingCollection dc) 
        {
            Color green = new Color(217, 255, 25);
            if(b.has("FieldPercept")) { drawFieldPercept(robotPose, b.get("FieldPercept"), cmBottom, dc, green); }
            if(b.has("FieldPerceptTop")) { drawFieldPercept(robotPose, b.get("FieldPerceptTop"), cmTop, dc, green); }
        }
        
        private void drawFieldPercept(Pose2D robotPose, LogDataFrame fieldPercept, Pose3D cameraMatrix, DrawingCollection dc, Color c) {
            try {
                Representations.FieldPercept fp = Representations.FieldPercept.parseFrom(fieldPercept.getData());
                
                List<Vector2D> fieldPoly = new ArrayList<>(fp.getFieldPoly().getPointsCount());
                for(IntVector2 polyPoint : fp.getFieldPoly().getPointsList()) {
                    Vector2D point = new Vector2D(polyPoint.getX(), polyPoint.getY());
                    Vector2D fieldPoint = project(cameraMatrix, f, point);                    
                    if(robotPose != null) {
                        fieldPoint = robotPose.multiply(fieldPoint);
                    }
                    fieldPoly.add(fieldPoint);
                }
                
                dc.add(new Pen(10, c));

                Vector2D last = null;
                for(Vector2D point: fieldPoly) {
                    dc.add(new Circle((int)point.x, (int)point.y, 10));
                    if(last != null) {
                        dc.add(new Line((int)last.x, (int)last.y, (int)point.x, (int)point.y));                            
                    }
                    last = point;
                } 
                
            } catch (InvalidProtocolBufferException ex) {
                Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }        
        
        private void drawScanEdgelPercept(Pose2D robotPose, LogDataFrame goalPerceptFrame, LogDataFrame scanLineFrame, Pose3D cameraMatrix, DrawingCollection dc, Color c) 
        {
            if (scanLineFrame == null || cameraMatrix == null) {
                return;
            }
            
            try {
                Representations.ScanLineEdgelPercept data = Representations.ScanLineEdgelPercept.parseFrom(scanLineFrame.getData());
                Representations.GoalPercept gp = Representations.GoalPercept.parseFrom(goalPerceptFrame.getData());
                
                // draw prijected edgesl on the field
                dc.add(new Pen(10, c));
                for(Representations.Edgel e: data.getEdgelsList()) {
                    Vector2D p = new Vector2D(e.getPoint().getX(), e.getPoint().getY());
                    Vector2D p2 = p.add(new Vector2D(e.getDirection().getX(), e.getDirection().getY()));
                    
                    Vector2D q = project(cameraMatrix,f,p);
                    Vector2D q2 = project(cameraMatrix,f,p2);
                    q2 = q.add(q2.subtract(q).normalize().multiply(50));
                    
                    if(robotPose != null) {
                        q = robotPose.multiply(q);
                        q2 = robotPose.multiply(q2);
                    }
                    
                    dc.add(new Circle((int)q.x, (int)q.y, 10));
                    dc.add(new Line((int)q.x, (int)q.y, (int)q2.x, (int)q2.y));
                }
                
                Vector2D last_p = null;
                dc.add(new Pen(10, Color.black));
                for(Representations.ScanLineEndPoint e: data.getEndPointsList()) {
                    Vector2D p = new Vector2D(e.getPosOnField().getX(), e.getPosOnField().getY());
                    
                    if(robotPose != null) {
                        p = robotPose.multiply(p);
                    }
                    
                    if (last_p != null) {
                        dc.add(new Line((int)last_p.x, (int)last_p.y, (int)p.x, (int)p.y));
                    }
                    dc.add(new Circle((int)p.x, (int)p.y, 10));
                    
                    last_p = p;
                }
                
                // draw prijected goal posts on the field
                for(Representations.GoalPercept.GoalPost g: gp.getPostList()) {
                    Vector2D q = new Vector2D(g.getPosition().getX(), g.getPosition().getY());
                    //Vector2D q = project(R,t,f,p);
                    
                    if(robotPose != null) {
                        q = robotPose.multiply(q);
                    }
                    
                    dc.add(new Pen(20, c));
                    dc.add(new FillOval((int)q.x, (int)q.y, 100, 100));
                    dc.add(new Pen(10, Color.black));
                    dc.add(new Circle((int)q.x, (int)q.y, 100));
                }

            } catch (InvalidProtocolBufferException ex) {
                Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        
        private Vector2D project(Pose3D pose, double f, Vector2D p) {
            Vector3D v = new Vector3D(f, 320 - p.x, 240 - p.y);
            v = pose.rotation.multiply(v);

            Vector2D q = new Vector2D(
                pose.translation.x - v.x*(pose.translation.z/v.z),
                pose.translation.y - v.y*(pose.translation.z/v.z));
            
            return q;
        }
        
        private Vector3D toVector(DoubleVector3 v) {
            return new Vector3D(v.getX(), v.getY(), v.getZ());
        }
        
        private Optional<String> getOwnBodyID(BlackBoard b)
        {
            LogDataFrame robotInfoFrame = b.get("RobotInfo");
            if(robotInfoFrame != null)
            {
                try {
                    RobotInfo robotInfo = RobotInfo.parseFrom(robotInfoFrame.getData());
                    return Optional.of(robotInfo.getBodyID());
                    
                } catch (InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            return Optional.empty();
        }
        
        private void drawCircle(BlackBoard b, DrawingCollection dc) {
            // circle
            LogDataFrame ransacCirclePercept2018 = b.get("RansacCirclePercept2018");
            if(ransacCirclePercept2018 != null)
            {
                try {
                    RansacCirclePercept2018 circle = RansacCirclePercept2018.parseFrom(ransacCirclePercept2018.getData());
                    
                    if(robotPose != null && circle.getWasSeen() && circle.hasMiddleCircleCenter()) 
                    {
                        Vector2D q = robotPose.multiply(new Vector2D(circle.getMiddleCircleCenter().getX(), circle.getMiddleCircleCenter().getY()));
                        dc.add(new Pen(50.0f, Color.yellow));
                        dc.add(new Circle((int)q.x, (int)q.y, 750));
                    }
                } catch (InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        
        private void drawLines(BlackBoard b, DrawingCollection dc) {
            LogDataFrame linesFrame = b.get("RansacLinePercept");
            if(linesFrame != null)
            {
                try {
                    RansacLinePercept lines = RansacLinePercept.parseFrom(linesFrame.getData());
                    
                    if(robotPose != null) 
                    {
                        for(LineSegment l: lines.getFieldLineSegmentsList()) {
                            Vector2D begin = new Vector2D(l.getBase().getX(), l.getBase().getY());
                            Vector2D end = new Vector2D(begin.x + l.getDirection().getX()*l.getLength(), begin.y + l.getDirection().getY()*l.getLength());
                            
                            begin = robotPose.multiply(begin);
                            end = robotPose.multiply(end);
                            
                            dc.add(new Pen(50.0f, Color.yellow));
                            dc.add(new Line((int)begin.x, (int)begin.y, (int)end.x, (int)end.y));
                        }
                    }
                    
                } catch (InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        
        private void drawShortLines(BlackBoard b, DrawingCollection dc) {
            LogDataFrame linesFrame = b.get("ShortLinePercept");
            if(linesFrame != null)
            {
                try {
                    ShortLinePercept lines = ShortLinePercept.parseFrom(linesFrame.getData());
                    
                    if(robotPose != null) 
                    {
                        for(LineSegment l: lines.getFieldLineSegmentsList()) {
                            Vector2D begin = new Vector2D(l.getBase().getX(), l.getBase().getY());
                            Vector2D end = new Vector2D(begin.x + l.getDirection().getX()*l.getLength(), begin.y + l.getDirection().getY()*l.getLength());
                            
                            begin = robotPose.multiply(begin);
                            end = robotPose.multiply(end);
                            
                            dc.add(new Pen(50.0f, Color.cyan));
                            dc.add(new Line((int)begin.x, (int)begin.y, (int)end.x, (int)end.y));
                        }
                    }
                    
                } catch (InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        
        private void drawBallPercept(BlackBoard b, DrawingCollection dc) {
            LogDataFrame data = b.get("MultiBallPercept");
            
            if(data != null)
            {
                try {
                    MultiBallPercept ball = MultiBallPercept.parseFrom(data.getData());
                    
                    if(robotPose != null && cmTop != null && cmBottom != null) {
                        
                        for(MultiBallPercept.BallPercept p: ball.getPerceptsList()) {
                            
                            Vector2D pi = new Vector2D(p.getCenterInImage().getX(), p.getCenterInImage().getY());
                            
                            Vector2D q;
                            if(p.getCameraId() == CommonTypes.CameraID.bottom) {
                                q = project(cmBottom,f,pi);
                            } else {
                                q = project(cmTop,f,pi);
                            }
                            
                            q = robotPose.multiply(q);
                            
                            dc.add(new Pen(50.0f, Color.pink));
                            dc.add(new Circle((int)q.x, (int)q.y, 50));
                        }
                    }
                    
                } catch (InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        
        private void drawRobot(BlackBoard b, DrawingCollection dc) {
            // parse the explicitely avaliable RobotPose if avaliable
            LogDataFrame data = b.get("RobotPose");
            
            if(data != null) {
                try {
                    RobotPose pose = RobotPose.parseFrom(data.getData());
                    robotPose = new Pose2D(pose.getPose().getTranslation().getX(), pose.getPose().getTranslation().getY(), pose.getPose().getRotation());
                } catch(InvalidProtocolBufferException ex) {
                    Logger.getLogger(FieldViewer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            
            if(robotPose != null) {
                dc.add(new Pen(1.0f, Color.red));
                dc.add(new Robot(robotPose.translation.x, robotPose.translation.y, robotPose.rotation, cmBottom.rotation.getZAngle()));
            }
        }
        
        @Override
        public void newFrame(BlackBoard b) 
        {
            readCameraMatrix(b);
            
            DrawingCollection dc = new DrawingCollection();
            drawTeamMessages(b, dc);
            drawCircle(b, dc);
            drawLines(b, dc);
            drawShortLines(b, dc);
            drawBallPercept(b, dc);
            drawRobot(b, dc);
            drawFieldPercept(b, dc);
            drawings.add(this.getClass(), dc);
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
    
  @Override
  public void dispose()
  {
    // remove all the registered listeners
    Plugin.debugDrawingManager.removeListener(drawingsListenerCognition);
    Plugin.debugDrawingManagerMotion.removeListener(drawingsListenerMotion);
    Plugin.plotDataManager.removeListener(plotDataListener);
  }
  
  /*
  // NOTE: can we use this again?
  class DrawingCollectionLimited extends DrawingCollection
    {
        private final int maxNumberOfEnties;
        public DrawingCollectionLimited(int maxNumberOfEnties) {
            this.maxNumberOfEnties = maxNumberOfEnties;
        }

        @Override
        public void add(Drawable d) {
            if(maxNumberOfEnties > 0 && this.drawables.size() >= maxNumberOfEnties) {
                this.drawables.remove(0);
            }
            super.add(d);
        }
    }
  */
  
  class MultiSourceDrawingCollection implements Drawable
  {
      // drawing buffer for concurrent access; every "source" gets its own "buffer"
      private final ConcurrentHashMap<Class<?>, DrawingCollection> drawingBuffers = new ConcurrentHashMap<>();
      
      public void add(Class<?> owner, Drawable d) 
      {
          // get the collection for the owner
          DrawingCollection dc = drawingBuffers.get(owner);
          if(dc == null) {
              dc = new DrawingCollection();
              drawingBuffers.put(owner, dc);
          }
          
          if(!btCollectDrawings.isSelected()) {
              dc.clear();
          }
          
          dc.add(d);
          
          // re-draw field
          fieldCanvas.repaint();
      }
      
      public void clear() {
          drawingBuffers.clear();
      }
      
      @Override
      public void draw(Graphics2D g2d) {
          drawingBuffers.forEach((s,b)->{b.draw(g2d);});
      }
  }

  
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBox btAntializing;
    private javax.swing.JButton btClean;
    private javax.swing.JCheckBox btCollectDrawings;
    private javax.swing.JToggleButton btFitToView;
    private javax.swing.JToggleButton btLog;
    private javax.swing.JToggleButton btReceiveDrawings;
    private javax.swing.JButton btRotate;
    private javax.swing.JCheckBox btTrace;
    private javax.swing.JComboBox cbBackground;
    private javax.swing.JCheckBox cbExportOnDrawing;
    private javax.swing.JDialog coordsPopup;
    private javax.swing.JPanel drawingPanel;
    private de.naoth.rc.components.DynamicCanvasPanel fieldCanvas;
    private javax.swing.JMenuItem jMenuItemCopyCoords;
    private javax.swing.JMenuItem jMenuItemExport;
    private javax.swing.JPopupMenu jPopupMenu;
    private javax.swing.JSlider jSlider1;
    private javax.swing.JToolBar jToolBar1;
    // End of variables declaration//GEN-END:variables
}
