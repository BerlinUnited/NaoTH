package de.naoth.rc.dialogs;

import de.naoth.rc.AbstractDialog;
import de.naoth.rc.ExtendedFileChooser;
import de.naoth.rc.RobotControl;
import de.naoth.rc.dataformats.JanusImage;
import de.naoth.rc.dialogs.drawings.DrawingCollection;
import de.naoth.rc.dialogs.drawings.DrawingOnImage;
import de.naoth.rc.dialogs.drawings.DrawingsContainer;
import de.naoth.rc.manager.DebugDrawingManager;
import de.naoth.rc.manager.ImageManager;
import de.naoth.rc.manager.ObjectListener;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.File;
import javax.imageio.ImageIO;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  thomas
 */
@PluginImplementation
public class ImageViewer extends AbstractDialog
{

  @InjectPlugin
  public RobotControl parent;
  @InjectPlugin
  public ImageManager imageManager;
  @InjectPlugin
  public DebugDrawingManager debugDrawingManager;
  //private ImagePanel imageCanvas;
  private long timestampOfTheLastImage;
  private ExtendedFileChooser fileChooser;
  // listeners
  ImageListener imageListener;
  DrawingsListener drawingsListener;

  /** Creates new form ImageViewer */
  public ImageViewer()
  {
    initComponents();
  }

  @Init
  public void init()
  {
    //this.imagePanel.add(imageCanvas);

    this.timestampOfTheLastImage = 0;

    this.drawingsListener = new DrawingsListener();
    this.imageListener = new ImageListener();

    fileChooser = new ExtendedFileChooser();
    fileChooser.setFileFilter(new PNGImageFileFilter());
  }//end init

  @Override
  public JPanel getPanel()
  {
    return this;
  }//end getPanel

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPopupMenuImagePanel = new javax.swing.JPopupMenu();
        jMenuItemSaveAs = new javax.swing.JMenuItem();
        imagePanel = new javax.swing.JPanel();
        imageCanvas = new de.naoth.rc.dialogs.panels.ImagePanel();
        jToolBar1 = new javax.swing.JToolBar();
        btReceiveImages = new javax.swing.JToggleButton();
        btReceiveDrawings = new javax.swing.JToggleButton();
        cbStretch = new javax.swing.JCheckBox();
        cbPreserveAspectRatio = new javax.swing.JCheckBox();
        jLabelFPS = new javax.swing.JLabel();
        jLabelResolution = new javax.swing.JLabel();

        jMenuItemSaveAs.setText("Save As...");
        jMenuItemSaveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemSaveAsActionPerformed(evt);
            }
        });
        jPopupMenuImagePanel.add(jMenuItemSaveAs);

        imagePanel.setBackground(java.awt.Color.gray);
        imagePanel.setBorder(javax.swing.BorderFactory.createLineBorder(java.awt.Color.darkGray));
        imagePanel.setPreferredSize(new java.awt.Dimension(320, 240));
        imagePanel.setLayout(new java.awt.BorderLayout());

        imageCanvas.setBackground(java.awt.Color.gray);
        imageCanvas.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        imageCanvas.setComponentPopupMenu(jPopupMenuImagePanel);
        imageCanvas.setDoubleBuffered(true);
        imageCanvas.setOpaque(false);

        javax.swing.GroupLayout imageCanvasLayout = new javax.swing.GroupLayout(imageCanvas);
        imageCanvas.setLayout(imageCanvasLayout);
        imageCanvasLayout.setHorizontalGroup(
            imageCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 476, Short.MAX_VALUE)
        );
        imageCanvasLayout.setVerticalGroup(
            imageCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 230, Short.MAX_VALUE)
        );

        imagePanel.add(imageCanvas, java.awt.BorderLayout.CENTER);

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        btReceiveImages.setText("Receive Images");
        btReceiveImages.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveImagesActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceiveImages);

        btReceiveDrawings.setText("Receive Drawings");
        btReceiveDrawings.setFocusable(false);
        btReceiveDrawings.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReceiveDrawings.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReceiveDrawings.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveDrawingsActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceiveDrawings);

        cbStretch.setSelected(true);
        cbStretch.setText("stretch image");
        cbStretch.setFocusable(false);
        cbStretch.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        cbStretch.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        cbStretch.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbStretchActionPerformed(evt);
            }
        });
        jToolBar1.add(cbStretch);

        cbPreserveAspectRatio.setSelected(true);
        cbPreserveAspectRatio.setText("keep aspect ratio");
        cbPreserveAspectRatio.setFocusable(false);
        cbPreserveAspectRatio.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);
        cbPreserveAspectRatio.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        cbPreserveAspectRatio.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbPreserveAspectRatioActionPerformed(evt);
            }
        });
        jToolBar1.add(cbPreserveAspectRatio);

        jLabelFPS.setFont(new java.awt.Font("Monospaced", 0, 11));
        jLabelFPS.setText("FPS");

        jLabelResolution.setFont(new java.awt.Font("Monospaced", 0, 11));
        jLabelResolution.setText("- x - ");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 500, Short.MAX_VALUE)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap(349, Short.MAX_VALUE)
                .addComponent(jLabelResolution, javax.swing.GroupLayout.PREFERRED_SIZE, 65, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabelFPS, javax.swing.GroupLayout.PREFERRED_SIZE, 70, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(imagePanel, javax.swing.GroupLayout.DEFAULT_SIZE, 480, Short.MAX_VALUE)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(imagePanel, javax.swing.GroupLayout.DEFAULT_SIZE, 234, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabelFPS)
                    .addComponent(jLabelResolution)))
        );
    }// </editor-fold>//GEN-END:initComponents
  private void btReceiveImagesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveImagesActionPerformed

    if (btReceiveImages.isSelected())
    {
      if (true || parent.checkConnected())
      {
        imageManager.addListener(this.imageListener);
      }
      else
      {
        btReceiveImages.setSelected(false);
      }
    }
    else
    {
      imageManager.removeListener(this.imageListener);
    }

  }//GEN-LAST:event_btReceiveImagesActionPerformed

  private void jMenuItemSaveAsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemSaveAsActionPerformed
    fileChooser.setSelectedFile(new File(this.timestampOfTheLastImage + ".png"));
    fileChooser.showSaveDialog(this);
    File selectedFile = fileChooser.getSelectedFile();
    if (selectedFile == null)
    {
      return;
    }
    try
    {
      Image image = imageCanvas.getImage();
      if (image == null)
      {
        throw new Exception("There is no Image to save");
      }
      int height = image.getHeight(this);
      int width = image.getWidth(this);

      BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);

      Graphics2D g2d = bi.createGraphics();
      g2d.drawImage(image, 0, 0, width, height, this);

      ImageIO.write(bi, "PNG", selectedFile);

      /*
      ImageIO.write(bi, "JPEG", selectedFile);
      ImageIO.write(bi, "gif", selectedFile);
      ImageIO.write(bi, "BMP", selectedFile);
       */
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog(this,
        e.toString(), "The image could not be written.", JOptionPane.ERROR_MESSAGE);
    }//end catch
  }//GEN-LAST:event_jMenuItemSaveAsActionPerformed

  private void cbPreserveAspectRatioActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbPreserveAspectRatioActionPerformed
    this.imageCanvas.setKeepAspectRatio(this.cbPreserveAspectRatio.isSelected());
  }//GEN-LAST:event_cbPreserveAspectRatioActionPerformed

  private void cbStretchActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbStretchActionPerformed
    this.imageCanvas.setStretchImage(this.cbStretch.isSelected());
  }//GEN-LAST:event_cbStretchActionPerformed

  private void btReceiveDrawingsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveDrawingsActionPerformed
    if (btReceiveDrawings.isSelected())
    {
      if (parent.checkConnected())
      {
        debugDrawingManager.addListener(this.drawingsListener);
      }
      else
      {
        btReceiveImages.setSelected(false);
      }
    }
    else
    {
      debugDrawingManager.removeListener(this.drawingsListener);
	  this.imageCanvas.getDrawingList().clear();
    }
  }//GEN-LAST:event_btReceiveDrawingsActionPerformed
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btReceiveDrawings;
    private javax.swing.JToggleButton btReceiveImages;
    private javax.swing.JCheckBox cbPreserveAspectRatio;
    private javax.swing.JCheckBox cbStretch;
    private de.naoth.rc.dialogs.panels.ImagePanel imageCanvas;
    private javax.swing.JPanel imagePanel;
    private javax.swing.JLabel jLabelFPS;
    private javax.swing.JLabel jLabelResolution;
    private javax.swing.JMenuItem jMenuItemSaveAs;
    private javax.swing.JPopupMenu jPopupMenuImagePanel;
    private javax.swing.JToolBar jToolBar1;
    // End of variables declaration//GEN-END:variables


  private void updateResolution(int x, int y)
  {
    this.jLabelResolution.setText(x + "x" + y);
  }//end updateResolution

  private void updateFPS()
  {
    long currentTime = System.currentTimeMillis();
    double fps = 1000.0 / ((double) (currentTime - this.timestampOfTheLastImage));
    String info = String.format("%4.1f fps", fps);
    this.jLabelFPS.setText(info);
    this.timestampOfTheLastImage = currentTime;
  }//end updateFPS

  class DrawingsListener implements ObjectListener<DrawingsContainer>
  {

    @Override
    public void errorOccured(String cause)
    {
      //btReceiveDrawings.setSelected(false);
      debugDrawingManager.removeListener(this);
    }//end errorOccured

    @Override
    public void newObjectReceived(DrawingsContainer objectList)
    {
      if (objectList != null)
      {
        imageCanvas.getDrawingList().clear();
        DrawingCollection drawingCollection = objectList.get(DrawingOnImage.class);
        if (drawingCollection != null)
        {
          imageCanvas.getDrawingList().add(drawingCollection);
        }
        repaint();

      }//end if
    }//end newObjectReceived
  }//end class DrawingsListener

  class ImageListener implements ObjectListener<JanusImage>
  {

    @Override
    public void newObjectReceived(JanusImage object)
    {
      if(object == null) return;
      
      imageCanvas.setImage(object.getRgb());
      imageCanvas.repaint();

      updateResolution(object.getRgb().getWidth(), object.getRgb().getHeight());
      updateFPS();
    }//end newObjectReceived

    @Override
    public void errorOccured(String cause)
    {
      btReceiveImages.setSelected(false);
      imageManager.removeListener(this);
    }//end errorOccured
  }//end ImageListener

  private class PNGImageFileFilter extends javax.swing.filechooser.FileFilter
  {

    final private String fileExtension = "png";
    final private String description = "Portable Network Graphics (*.png)";

    @Override
    public boolean accept(File file)
    {
	  if(file.isDirectory()) return true;
      String filename = file.getName();
      return filename.toLowerCase().endsWith("." + this.fileExtension);
    }

    @Override
    public String getDescription()
    {
      return this.description;
    }

    @Override
    public String toString()
    {
      return fileExtension;
    }
  }//end class PNGImageFileFilter

  @Override
  public void dispose()
  {
    imageManager.removeListener(this.imageListener);
    debugDrawingManager.removeListener(this.drawingsListener);
  }//end dispose
}//end class ImageViewer

