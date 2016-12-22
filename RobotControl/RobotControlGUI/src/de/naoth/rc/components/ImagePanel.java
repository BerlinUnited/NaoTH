/*
 * ImagePanel.java
 *
 * Created on 07.03.2013, 19:27:12
 */
package de.naoth.rc.components;

import de.naoth.rc.drawings.Drawable;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.ArrayList;
import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

/**
 *
 * @author Heinrich
 */
public class ImagePanel extends javax.swing.JPanel
{
    private Image backgroundImage;
    private boolean keepAspectRatio;
    private boolean stretchImage;
    private boolean showDrawings;
    
    private final ArrayList<Drawable> drawigs = new ArrayList<Drawable>();
    
    public ImagePanel()
    {
        initComponents();
        
        this.keepAspectRatio = true;
        this.stretchImage = true;
        this.showDrawings = false;
        
        this.fileChooser.setFileFilter(new PNGImageFileFilter());
    }
    
    
    public void setImage(Image image)
    {
      this.backgroundImage = image;
      //this.setSize(imagePanel.getSize());
      this.repaint();
    }

    public void setKeepAspectRatio(boolean value)
    {
      if(this.keepAspectRatio != value) {
        this.keepAspectRatio = value;
        this.repaint();
      }
    }

    public void setStretchImage(boolean value)
    {
      if(this.stretchImage != value) {
        this.stretchImage = value;
        this.repaint();
      }
    }
    
    public void setShowDrawings(boolean value) {
      if(this.showDrawings != value) {
        this.showDrawings = value;
        this.repaint();
      }
    }
    
    public boolean isKeepAspectRatio() {
      return this.keepAspectRatio;
    }
    
    public boolean isStretchImage() {
      return this.stretchImage;
    }
    
    public boolean isShowDrawings() {
      return this.showDrawings;
    }

    public Image getImage()
    {
      return this.backgroundImage;
    }

    public ArrayList<Drawable> getDrawingList()
    {
      return this.drawigs;
    }

    @Override
    public void paint(Graphics g)
    {
        Graphics2D g2d = (Graphics2D) g;

        if (backgroundImage == null) {
            return;
        }
      
        double hPanel = (double) getHeight() - 2;
        double wPanel = (double) getWidth() - 2;

        double hImg = (double) backgroundImage.getHeight(this);
        double wImg = (double) backgroundImage.getWidth(this);

        double posX = 0.0;
        double posY = 0.0;

        double ratioH = hPanel / hImg;
        double ratioW = wPanel / wImg;

        if (!this.stretchImage)
        {
          posX = Math.round((wPanel - wImg) * 0.5);
          posY = Math.round((hPanel - hImg) * 0.5);
          ratioH = 1.0;
          ratioW = 1.0;
        }
        else
        {
          if (this.keepAspectRatio)
          {
            double scale = Math.min(hPanel / hImg, wPanel / wImg);
            posX = Math.round((wPanel - scale * wImg) * 0.5);
            posY = Math.round((hPanel - scale * hImg) * 0.5);
            ratioH = scale;
            ratioW = scale;
          }
        }

        g2d.translate((posX + 1), (posY + 1));
        g2d.scale(ratioW, ratioH);

        g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
        g2d.drawImage(backgroundImage, 0, 0, (int) wImg, (int) hImg, this);

        if(this.showDrawings) {
          for (Drawable d : drawigs) {
            if(d != null) {
              d.draw(g2d);
            }
          }
        }

        // transform the drawing-pane back (nessesary to draw the other components correct)
        g2d.scale(1.0/ratioW, 1.0/ratioH);
        g2d.translate(-(posX + 1), -(posY + 1));
    }//end paint
    
    public void saveImage(File file)
    {
        try
        {
          if (this.backgroundImage == null)
          {
            throw new Exception("There is no Image to save");
          }
          int height = this.backgroundImage.getHeight(this);
          int width = this.backgroundImage.getWidth(this);

          BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);

          Graphics2D g2d = bi.createGraphics();
          g2d.drawImage(this.backgroundImage, 0, 0, width, height, this);

          if(this.showDrawings) {
            for (Drawable d : drawigs) {
              if(d != null) {
                d.draw(g2d);
              }
            }
          }
          
          ImageIO.write(bi, "PNG", file);

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
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPopupMenuImagePanel = new javax.swing.JPopupMenu();
        jMenuItemSaveAs = new javax.swing.JMenuItem();
        fileChooser = new de.naoth.rc.components.ExtendedFileChooser();

        jMenuItemSaveAs.setText("Save As...");
        jMenuItemSaveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemSaveAsActionPerformed(evt);
            }
        });
        jPopupMenuImagePanel.add(jMenuItemSaveAs);

        setComponentPopupMenu(jPopupMenuImagePanel);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jMenuItemSaveAsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemSaveAsActionPerformed
        fileChooser.setSelectedFile(new File("image.png"));
        fileChooser.showSaveDialog(this);
        File selectedFile = fileChooser.getSelectedFile();
        if (selectedFile != null)
        {
            this.saveImage(selectedFile);
        }
    }//GEN-LAST:event_jMenuItemSaveAsActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private de.naoth.rc.components.ExtendedFileChooser fileChooser;
    private javax.swing.JMenuItem jMenuItemSaveAs;
    private javax.swing.JPopupMenu jPopupMenuImagePanel;
    // End of variables declaration//GEN-END:variables
}
