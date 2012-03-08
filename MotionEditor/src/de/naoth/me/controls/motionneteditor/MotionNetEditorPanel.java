package de.naoth.me.controls.motionneteditor;


import de.naoth.me.core.MotionNet;
import de.naoth.me.core.MotionNetEvent;
import de.naoth.me.core.MotionNetListener;
import java.awt.Point;
import java.beans.PropertyChangeEvent;
import javax.swing.SwingUtilities;

import de.naoth.me.core.KeyFrame;

/*
 * KeyFrameEditorPanel.java
 *
 * Created on 12. März 2008, 18:57
 */
import de.naoth.me.core.Transition;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.beans.PropertyChangeListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import javax.swing.JOptionPane;



/**
 *
 * @author  Heinrich Mellmann
 */
public class MotionNetEditorPanel extends javax.swing.JLayeredPane implements MotionNetListener
{
    
    //private final int KEY_FRAME_LAYER = 1000;
    //private final int TRANSITION_PREVIEW_LAYER = 500;
    
    private MotionNet motionNet;
    private HashMap<KeyFrame, KeyFrameControl> keyFrameControlMap;
    private ArrayList<TransitionControl> transitionControlList;
    
    private boolean createNewTransition = false;
    private KeyFrameControl from = null;
    private TransitionPreview transitionControlPreview = null;

    public boolean firstKeyFrameRemoved;
    public PropertyChangeListener jointChangeListener;

    private KeyFrameControl selectedKeyFrame;
    private TransitionControl selectedTransition;

    private Marker marker;
    
    /** Creates new form KeyFrameEditorPanel */
    public MotionNetEditorPanel()
    {
        initComponents();
        this.keyFrameControlMap = new HashMap<KeyFrame, KeyFrameControl>();
        this.transitionControlList = new ArrayList<TransitionControl>();
        this.motionNet = null;
        firstKeyFrameRemoved = false;
        setPasteEnable(false);
        this.setBounds(0, 0, this.getWidth(), this.getHeight());
        this.jointChangeListener = new PropertyChangeListener()
          {
            public void propertyChange(PropertyChangeEvent evt)
            {
              if(evt.getPropertyName().compareTo("jointStateChanged") == 0)
              {
                setPasteEnable(false);
              }
            }
          };

    }

    public void setPasteEnable(boolean enable)
    {
      jMenuItemPaste.setVisible(enable);
    }


    public MotionNet getMotionNet()
    {
        return this.motionNet;
    }//end getMotionNet
    
    public void setMotionNet(MotionNet motionNet)
    {
      if(motionNet == null)
          return;

      if(this.motionNet != null)
          this.motionNet.removeMotionNetListener(this);

      this.removeAll();
      firstKeyFrameRemoved = true;
      for(KeyFrame keyFrame: motionNet.getKeyFrameMap().values())
      {
        if(keyFrame.getId() == 0)
        {
          firstKeyFrameRemoved = false;
        }
        KeyFrameControl keyFrameControl = new KeyFrameControl(this, keyFrame);
        this.add(keyFrameControl);
        this.keyFrameControlMap.put(keyFrame, keyFrameControl);
      }//end for

      // TODO: das hier ist eine schlechte Loesung...
      // TransitionControl hat kein Bezug zu transition..
      for(KeyFrame keyFrame: motionNet.getKeyFrameMap().values())
      {
          for(Transition transition: keyFrame.getTransitions())
          {
              TransitionControl t = new TransitionControl(this, transition, keyFrameControlMap.get(keyFrame), keyFrameControlMap.get(transition.getToKeyFrame()));
              t.setBounds(this.getBounds());
              this.add(t);
              transitionControlList.add(t);
          }
      }//end for

      this.motionNet = motionNet;
      this.motionNet.addMotionNetListener(this);
      this.repaint();
    }//end setMotionNet
    
    
    // <editor-fold defaultstate="collapsed" desc="Implementation of the MotionNetListener methods">
    public void keyFrameAdded(MotionNetEvent event)
    {
      KeyFrame newKeyFrame = (KeyFrame)event.getValue();
      if(newKeyFrame.getId() == 0)
      {
        firstKeyFrameRemoved = false;
      }
      this.add(new KeyFrameControl(this, newKeyFrame));
      this.repaint();
    }//end keyFrameAdded
    
    
    public void keyFrameRemoved(MotionNetEvent event)
    {
        KeyFrameControl control = this.keyFrameControlMap.get((KeyFrame)event.getValue());
        if(control == null) return; 
        if(control.isSelected()) control.setSelected(false);
        this.remove(control);
        this.repaint();
    }//end keyFrameRemoved
    // </editor-fold> 
    
    
    public void removeKeyFrameControl(KeyFrameControl control)
    {
      KeyFrame keyFrame = control.getKeyFrame();
      if(keyFrame.getId() == 0)
      {
        firstKeyFrameRemoved = true;
      }
      for(TransitionControl transitionControl: transitionControlList)
      {
        Transition transition = transitionControl.getTransion();
        if(transition.getFromKeyFrame().equals(keyFrame) || transition.getToKeyFrame().equals(keyFrame))
        {
          removeTransitionControl(transitionControl);
        }
      }
      this.motionNet.removeKeyFrame(control.getKeyFrame());
      if(control == null) return;
      if(control.isSelected()) control.setSelected(false);
      this.remove(control);
      this.repaint();
    }//end removeKeyFrameControl
    
    public void removeTransitionControl(TransitionControl control)
    {
        this.motionNet.removeTransition(control.getTransion());
        if(control.isSelected()) control.setSelected(false);
        this.remove(control);
        this.repaint();
    }//end removeTransition
     
    // <editor-fold defaultstate="collapsed" desc="Creating new Transition">
    public void startCreateNewTransition(KeyFrameControl control)
    {
        this.createNewTransition = true;
        this.from = control;
        this.transitionControlPreview = new TransitionPreview(this, from);
        this.addMouseMotionListener(transitionControlPreview);
        this.add(this.transitionControlPreview);

        transitionControlPreview.setBounds(this.getBounds());
    }//end startCreateNewTransition
    
    public void finishCreateNewTransition(KeyFrameControl control)
    {
        Transition transition = new Transition(1000);
        this.motionNet.addTransition(transition, from.getKeyFrame(), control.getKeyFrame());
        TransitionControl transitionControl = new TransitionControl(this, transition, from, control);
        this.add(transitionControl);
        this.transitionControlList.add(transitionControl);
        this.addMouseMotionListener(transitionControl);
        //transitionControl.addMouseMotionListener(transitionControl);
        cancelCreateNewTransition();
        
    }//end finishCreateNewTransition
    
    public void cancelCreateNewTransition()
    {
        // clean
        this.createNewTransition = false;
        if(this.transitionControlPreview != null)
        {
            this.removeMouseMotionListener(transitionControlPreview);
            this.remove(transitionControlPreview);
            this.transitionControlPreview = null;
            this.from = null;
        }//end if
        
        repaint();
    }//end cancelCreateNewTransition
    // </editor-fold>
    
    
    public void keyFrameControlSelected(KeyFrameControl control)
    {
        if(selectedTransition != null) transitionControlSelected(null);
        
        if(createNewTransition)
        {
            if(from == null)
            {
              startCreateNewTransition(control);
            }
            else
            {
              finishCreateNewTransition(control);
            }
            
            if(selectedKeyFrame != control)
            {
              control.setSelected(false);
            }
            return;
        }//end if
        
        KeyFrame oldValue = null;
        
        if(selectedKeyFrame != null)
        {
            oldValue = selectedKeyFrame.getKeyFrame();
            selectedKeyFrame.setSelected(false);
        }//end if
        selectedKeyFrame = control;
        
        this.firePropertyChange("selectedKeyFrame",oldValue, 
                    (selectedKeyFrame!=null)?selectedKeyFrame.getKeyFrame():null);
    }//end keyFrameControlSelected
    
    
    public void transitionControlSelected(TransitionControl control)
    {        
        if(selectedKeyFrame != null) keyFrameControlSelected(null);
        
        Transition oldValue = null;
        
        if(selectedTransition != null)
        {
            oldValue = selectedTransition.getTransion();
            selectedTransition.setSelected(false);
        }//end if
        selectedTransition = control;
        
        this.firePropertyChange("selectedTransition",oldValue, 
                    (selectedTransition!=null)?selectedTransition.getTransion():null);
    }//end keyFrameControlSelected
    
    public void keyFrameControlFocused(KeyFrameControl control)
    {
        this.setComponentZOrder(control, 0); 
        control.repaint();
    }//end keyFrameControlSelected
    
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jPopupMenu = new javax.swing.JPopupMenu();
    jMenuItemNewKeyFrame = new javax.swing.JMenuItem();
    jMenuItemNewTransition = new javax.swing.JMenuItem();
    jMenuItemPaste = new javax.swing.JMenuItem();

    jMenuItemNewKeyFrame.setText("New KeyFrame");
    jMenuItemNewKeyFrame.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseReleased(java.awt.event.MouseEvent evt) {
        jMenuItemNewKeyFrameMouseReleased(evt);
      }
    });
    jPopupMenu.add(jMenuItemNewKeyFrame);

    jMenuItemNewTransition.setText("New Transition");
    jMenuItemNewTransition.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseReleased(java.awt.event.MouseEvent evt) {
        jMenuItemNewTransitionMouseReleased(evt);
      }
    });
    jPopupMenu.add(jMenuItemNewTransition);

    jMenuItemPaste.setText("Paste");
    jMenuItemPaste.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseReleased(java.awt.event.MouseEvent evt) {
        jMenuItemPasteMouseReleased(evt);
      }
    });
    jPopupMenu.add(jMenuItemPaste);

    setBackground(new java.awt.Color(255, 255, 255));
    setComponentPopupMenu(jPopupMenu);
    setOpaque(true);
    addMouseListener(new java.awt.event.MouseAdapter() {
      public void mousePressed(java.awt.event.MouseEvent evt) {
        formMousePressed(evt);
      }
      public void mouseClicked(java.awt.event.MouseEvent evt) {
        formMouseClicked(evt);
      }
    });
    addComponentListener(new java.awt.event.ComponentAdapter() {
      public void componentResized(java.awt.event.ComponentEvent evt) {
        formComponentResized(evt);
      }
    });
    addPropertyChangeListener(new java.beans.PropertyChangeListener() {
      public void propertyChange(java.beans.PropertyChangeEvent evt) {
        formPropertyChange(evt);
      }
    });
  }// </editor-fold>//GEN-END:initComponents

    // remove the current selection
    private void formMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_formMouseClicked
        keyFrameControlSelected(null);
        transitionControlSelected(null);
    }//GEN-LAST:event_formMouseClicked

    // creates new KeyFrame
    private void jMenuItemNewKeyFrameMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jMenuItemNewKeyFrameMouseReleased
        Point p = evt.getLocationOnScreen();
        SwingUtilities.convertPointFromScreen(p, this);
        if(motionNet == null)
            JOptionPane.showMessageDialog(this, 
                  "No MotionNet loaded.", "No MotionNet loaded.", JOptionPane.ERROR_MESSAGE);
        else
            motionNet.addKeyFrame(p.x-evt.getX(),p.y);
    }//GEN-LAST:event_jMenuItemNewKeyFrameMouseReleased

    private void jMenuItemNewTransitionMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jMenuItemNewTransitionMouseReleased
        createNewTransition = true;
    }//GEN-LAST:event_jMenuItemNewTransitionMouseReleased

    private void formMousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_formMousePressed
        if(this.createNewTransition)
        {
            cancelCreateNewTransition();
        }//end if
    }//GEN-LAST:event_formMousePressed

    private void jMenuItemPasteMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jMenuItemPasteMouseReleased

        String result;
        Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
        Transferable contents = clipboard.getContents(null);
        boolean hasTransferableText =
          (contents != null) &&
          contents.isDataFlavorSupported(DataFlavor.stringFlavor);
        
        if ( hasTransferableText ) {
          try {
            result = (String)contents.getTransferData(DataFlavor.stringFlavor);
            
            Point p = evt.getLocationOnScreen();
            SwingUtilities.convertPointFromScreen(p, this);
            if(motionNet == null)
                JOptionPane.showMessageDialog(this, 
                      "No MotionNet loaded.", "No MotionNet loaded.", JOptionPane.ERROR_MESSAGE);
            else
            {
                motionNet.addKeyFrame(p.x-evt.getX(),p.y).fromString(result);
            }
          }
          catch (UnsupportedFlavorException ex){
            ex.printStackTrace();
          }
          catch (IOException ex) {
            ex.printStackTrace();
          }
        }//end if
    }//GEN-LAST:event_jMenuItemPasteMouseReleased

    private void formPropertyChange(java.beans.PropertyChangeEvent evt) {//GEN-FIRST:event_formPropertyChange
    }//GEN-LAST:event_formPropertyChange

    private void formComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentResized
      for(TransitionControl t: transitionControlList)
      {
          t.setBounds(this.getBounds());
      }
    }//GEN-LAST:event_formComponentResized
  
    public void setMarker(int x,int y)
    {
        this.marker = new Marker(x, y, Color.red, 5);
        this.repaint();
    }
    
    
  @Override
    public void paint( Graphics g )
    {
        super.paint(g);
        if(this.marker != null)
        {
            marker.draw((Graphics2D)g);
        }
    }//end draw
    
     
  
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JMenuItem jMenuItemNewKeyFrame;
  private javax.swing.JMenuItem jMenuItemNewTransition;
  private javax.swing.JMenuItem jMenuItemPaste;
  private javax.swing.JPopupMenu jPopupMenu;
  // End of variables declaration//GEN-END:variables
    
}
