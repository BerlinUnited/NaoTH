/*
 *
 */

package de.naoth.rc.dialogs;

import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich
 */
@RCDialog(category = RCDialog.Category.Log, name = "BehaviorAnalyzer")
@PluginImplementation
public class BehaviorAnalyzer extends AbstractDialog 
{

    @InjectPlugin
    public LogFileEventManager logFileEventManager;
    
    /**
     * Creates new form BehaviorAnalyzer
     */
    public BehaviorAnalyzer() {
        initComponents();
        fileChooser.setFileFilter(new LogFileFilter());
        
        this.jSlider1.addMouseWheelListener(new MouseWheelListener() {
            @Override
            public void mouseWheelMoved(MouseWheelEvent e) {
                    if(e.getWheelRotation() > 0)
                      jSlider1.setValue(jSlider1.getValue() + 1);
                    else
                      jSlider1.setValue(jSlider1.getValue() - 1);
            }
      });
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents()
  {

    fileChooser = new javax.swing.JFileChooser();
    open = new javax.swing.JButton();
    jSlider1 = new javax.swing.JSlider();

    open.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Open24.gif"))); // NOI18N
    open.setToolTipText("Open");
    open.setMaximumSize(new java.awt.Dimension(31, 31));
    open.setMinimumSize(new java.awt.Dimension(31, 31));
    open.setPreferredSize(new java.awt.Dimension(31, 31));
    open.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        openActionPerformed(evt);
      }
    });

    jSlider1.addChangeListener(new javax.swing.event.ChangeListener()
    {
      public void stateChanged(javax.swing.event.ChangeEvent evt)
      {
        jSlider1StateChanged(evt);
      }
    });

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addComponent(open, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jSlider1, javax.swing.GroupLayout.DEFAULT_SIZE, 363, Short.MAX_VALUE))
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(open, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jSlider1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addGap(0, 269, Short.MAX_VALUE))
    );
  }// </editor-fold>//GEN-END:initComponents

    LogFile logfile = null;
    private void openActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openActionPerformed
      if(fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION)
      {
        File openedFile = fileChooser.getSelectedFile();

        if(openedFile != null && openedFile.exists())
        {
            try
            {
                logfile = new LogFile(openedFile);
                
                this.jSlider1.setEnabled(true);
                this.jSlider1.setValue(0);
                this.jSlider1.setMinimum(0);
                this.jSlider1.setMaximum(logfile.getFrameCount()-1);
            }
            catch(IOException e)
            {
              e.printStackTrace(System.err);
            }
              //this.parent.setTriggerConnect(false);
        }
      }//end if
    }//GEN-LAST:event_openActionPerformed

    private void jSlider1StateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSlider1StateChanged
        int frameNumber = this.jSlider1.getValue();
        
        try
        {
          HashMap<String,LogDataFrame> frame = logfile.readFrame(frameNumber);
          if(logFileEventManager != null) {
            logFileEventManager.fireLogFrameEvent(frame.values());
          }
        }catch(IOException e)
        {
          System.err.println("Couldn't read the a frame: " + e);
        }
        
    }//GEN-LAST:event_jSlider1StateChanged

  private class LogFile
  {
      private int minFrame = Integer.MAX_VALUE;
      private int maxFrame = Integer.MIN_VALUE;
      private ArrayList<Frame> frameList = new ArrayList<Frame>();
      private HashMap<Integer, Integer> framePosition = new HashMap<Integer, Integer>();
      private BasicReader reader = null;
      
      public LogFile(File file) throws IOException
      {
          //FileInputStream file_input = new FileInputStream (file);
          //DataInputStream inputStream = new DataInputStream (file_input);
          reader = new BasicReader(new RandomAccessFile(file, "r"));
          scan(reader);
                //this.raf = new RandomAccessFile(this.openedFile, "r");
      }
      
      private void scan(BasicReader data_in) throws IOException
      {
          int currentFrameNumber = -1;
          int currentFrameSize = 0;
          int currentFramePos = 0;

          while(true)
          {
            try
            {
              int fragmentFrameSize = 0;

              int frameNumber = data_in.readInt();
              fragmentFrameSize += 4;

              // plausibility check
              if(currentFrameNumber > -1 && (frameNumber < currentFrameNumber || frameNumber < 0)) {
                throw new IOException("corrupt frame number: " + frameNumber + " after " + currentFrameNumber);
              }

              if(frameNumber - currentFrameNumber > 30) {
                System.out.println("frame jump: " + currentFrameNumber + " -> " + frameNumber);
              }

              String currentName = data_in.readString();
              fragmentFrameSize += currentName.length() + 1;

              int currentSize = data_in.readInt();
              fragmentFrameSize += 4;
              fragmentFrameSize += currentSize;

              this.minFrame = Math.min(this.minFrame, frameNumber);
              this.maxFrame = Math.max(this.maxFrame, frameNumber);

              if(currentFrameNumber != frameNumber && currentFrameNumber != -1)
              {
                Frame frame = new Frame(currentFrameNumber, currentFrameSize, currentFramePos);

                framePosition.put(frameList.size(), currentFrameNumber);
                frameList.add(frame);

                currentFramePos += currentFrameSize;
                currentFrameSize = 0;
              }//end if

              currentFrameSize += fragmentFrameSize;
              currentFrameNumber = frameNumber;
              
              long skippedSize = data_in.skip(currentSize);
            }
            catch (EOFException eof) 
            {
              Logger.getLogger(BehaviorAnalyzer.class.getName()).log(Level.FINE, "End of file", eof);
              break;
            }
          }//end while
        }//end parseLogFile
 
    private HashMap<String,LogDataFrame> readFrame(int frameId) throws IOException
  {
    Frame frame = this.frameList.get(frameId);
    if(frame == null) {
        return null;
    }
    
    // jump to the begin of the frame
    reader.seek(frame.position);

    int numberOfReadBytes = 0;

    HashMap<String,LogDataFrame> currentFrame = new HashMap<>();
    
    while(numberOfReadBytes < frame.size)
    {
      int frameNumber = reader.readInt();
      numberOfReadBytes += 4;

      if(frameNumber != frame.number) {
        throw new IOException("corrupt frame number: " + frameNumber + " expected " + frame.number);
      }
      
      String currentName = reader.readString();
      numberOfReadBytes += currentName.length()+1;
      int currentSize = reader.readInt();
      numberOfReadBytes += 4;

      byte[] buffer = new byte[currentSize];
      numberOfReadBytes += reader.read(buffer);
      
      LogDataFrame logDataFrame = new LogDataFrame(frameNumber, currentName, buffer);
      currentFrame.put(currentName, logDataFrame);
    }//end while
    
    return currentFrame;
  }//end readFrame
      
    class Frame
    {
      final int number;
      final int size;
      final int position;

      public Frame(int number, int size, int position) {
        this.number = number;
        this.size = size;
        this.position = position;
      }
    }//end class Frame

        public int getMinFrame() {
            return minFrame;
        }

        public int getMaxFrame() {
            return maxFrame;
        }
        
        public int getFrameCount() {
            return frameList.size();
        }
        
        public int getFrameNumber(int n) {
            Frame frame = this.frameList.get(n);
            if(frame == null) {
                return -1;
            }
            return frame.number;
        }
  }
  
  class BasicReader
  {
    private InputStream streamReader = null;
    private RandomAccessFile rafReader = null;
    
    public BasicReader(InputStream streamReader)
    {
      this.streamReader = streamReader;
    }
    
    public BasicReader(RandomAccessFile rafReader)
    {
      this.rafReader = rafReader;
    }
            
    public int read(byte[] buffer) throws IOException
    {
      return (streamReader == null)?this.rafReader.read(buffer):this.streamReader.read(buffer);
    }
    public int read()  throws IOException
    {
      return (streamReader == null)?this.rafReader.read():this.streamReader.read();
    }
    public long skip(int size) throws IOException
    {
      return (streamReader == null)?this.rafReader.skipBytes(size):this.streamReader.skip(size);
    }
    public void seek(long size) throws IOException
    {
      this.rafReader.seek(size);
    }
    
    public String readString() throws IOException, EOFException
    {
      StringBuilder sb = new StringBuilder();
      int c = this.read();
      while(((char)c) != '\0')
      {
        if(c == -1) throw new EOFException();
        sb.append((char)c);
        c = this.read();
      }

      return sb.toString();
    }
    
    public int readInt() throws IOException
    {
      byte[] buffer = new byte[4];
      int numberOfBytes = this.read(buffer);

      if(numberOfBytes != 4) throw new EOFException();

      int result =
          (0xff000000 & buffer[3] << 32)|
          (0x00ff0000 & buffer[2] << 16)|
          (0x0000ff00 & buffer[1] << 8) |
          (0x000000ff & buffer[0]);

      return result;
    }
  }//end class BasicReader
    
  private class LogFileFilter extends javax.swing.filechooser.FileFilter
  {

    @Override
    public boolean accept(File file)
    {
      if (file.isDirectory())
      {
        return true;
      }
      String filename = file.getName();
      return filename.toLowerCase().endsWith(".log");
    }

    @Override
    public String getDescription()
    {
      return "Logfile (*.log)";
    }

    @Override
    public String toString()
    {
      return ".log";
    }
  }//end class LogFileFilter

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JFileChooser fileChooser;
  private javax.swing.JSlider jSlider1;
  private javax.swing.JButton open;
  // End of variables declaration//GEN-END:variables
}
