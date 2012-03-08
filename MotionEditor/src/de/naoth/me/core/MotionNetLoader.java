package de.naoth.me.core;

//import de.hu_berlin.informatik.ki.motioneditor.core.*;
import java.io.*;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import javax.swing.JOptionPane;
import java.util.ArrayList;

/**
 * @author Heinrich Mellmann
 * @author Luisa Jahn (extended)
 */
public class MotionNetLoader
{
    
    public MotionNetLoader()//JointPrototypeConfiguration configuration)
    {}
    
    public MotionNet loadFromFile(File file) throws IOException
    {
        try
        {
            MotionNetParser parser = new MotionNetParser(file);
            return parser.parse();
        }
        catch(Exception e)
        {
            e.printStackTrace();

        }
        return null;
    }//end loadFromFile

    /**
     * load a MotionNet from a .txt-file
     * @param file
     * @return
     * @throws IOException
     */
    public MotionNet loadFromTxtFile(File file) throws IOException
    {
        try
        {
            MotionNetFromTxtParser parser = new MotionNetFromTxtParser(file);
            return parser.parse();
        }
        catch(Exception e)
        {
            e.printStackTrace();

        }
        return null;
    }//end loadFromFile

    public MotionNet importFromFile(File file) throws IOException
    {
        try
        {
            MotionNetImport importer = new MotionNetImport(file);
            return importer.parse();
        }
        catch(Exception e)
        {
            e.printStackTrace();

        }
        return null;
    }//end loadFromFile
    
    public void saveToFile(MotionNet motionNet, File file) throws IOException
    {
      // fure alle Keyframes des aktuellen Projektes alle Motorwerte(m) speichern
      try
      {
        FileOutputStream fos = new FileOutputStream(file);
        PrintStream output = new PrintStream(fos);
        output.print(motionNet);
        output.close();
      }

      catch (FileNotFoundException e)
      {
      JOptionPane.showMessageDialog(null,
      e.toString(), "Fehler", JOptionPane.ERROR_MESSAGE);
      }


  }//end saveToFile
    
   /**
    * save a MotionNet to a txt-file
    * @param motionNet
    * @param file
    * @throws IOException
    */
   public void saveToTxtFile(MotionNet motionNet, File file) throws IOException
    {
      // fuer alle Keyframes des aktuellen Projektes alle Motorwerte(m) speichern
      try
      {
        FileOutputStream fos = new FileOutputStream(file);
        PrintStream output = new PrintStream(fos);
        output.print(motionNet.toTxt());
        output.close();
      }

      catch (FileNotFoundException e)
      {
      JOptionPane.showMessageDialog(null,
      e.toString(), "Fehler", JOptionPane.ERROR_MESSAGE);
      }


  }//end saveToTxtFile  
    
  public void exportToFile(MotionNet motionNet, File file) throws IOException
  {
     // fure alle Keyframes des aktuellen Projektes alle Motorwerte(m) speichern
    try
    {
      FileOutputStream fos = new FileOutputStream(file);
      PrintStream output = new PrintStream(fos);
      ArrayList<String> JointList = new ArrayList<String>();
      String values = "";
      long  Duration = 0;
      int KeyFrameCount = 0;

      DecimalFormatSymbols dfs = new DecimalFormatSymbols();
      dfs.setDecimalSeparator('.');
      DecimalFormat numberFormat = new DecimalFormat( "##0.######", dfs);

      KeyFrame actualKeyFrame = null;
      KeyFrame nextKeyFrame = null;

      for(KeyFrame keyFrame: motionNet.getKeyFrameMap().values())
      {
        if(keyFrame.getId() == 0)
        {
          actualKeyFrame = keyFrame;
        }

        ArrayList<Transition> Transitions = keyFrame.getTransitions();
        if(Transitions.size() > 1 )
        {
          JOptionPane.showMessageDialog(null,
            "Error : this mesh can not be exported, because there are more then one transitions per node defined",
            "Error", JOptionPane.ERROR_MESSAGE);
         return;
        }
        for(int m = 0; m < 22; m++)
        {
         Joint jointValue = keyFrame.getJointValue(m);
         if(!JointList.contains(jointValue.getId()))
         {
           JointList.add(jointValue.getId());
         }
        }
      }
      output.print("#WEBOTS_MOTION,V1.0");
      for(String Joint: JointList)
      {
       output.print("," + Joint);
      }
      output.println();

      while(actualKeyFrame != null)
      {
        int Minutes = (int) Math.round(Duration / 60000);
        int Seconds = (int) Math.round(Duration / 1000  - Minutes * 60);
        int MilliSeconds = (int) Math.round(Duration - Minutes * 60000 - Seconds * 1000);

        values = add_leading_zeros(Minutes, 2) + ":" +
                add_leading_zeros(Seconds, 2) + ":" +
                add_leading_zeros(MilliSeconds, 3) + "," +
                "Pose " + ++KeyFrameCount;

        for(int m = 0; m < 22; m++)
        {
          Joint jointValue = actualKeyFrame.getJointValue(m);

          if(JointList.contains(jointValue.getId()))
          {
           double val = jointValue.getValue() * Math.PI / 180;
           values += "," + numberFormat.format(val); // formatiere die ausgabe (4 Stellen nach dem Komma)
          }
        }
        output.println(values);
        nextKeyFrame = null;
        for(Transition transition: actualKeyFrame.getTransitions())
        {
          Duration += (int) transition.getDuration();
          nextKeyFrame = transition.getToKeyFrame();
        }
        actualKeyFrame = nextKeyFrame;
      }
      output.close();
    }

    catch (FileNotFoundException e)
    {
       JOptionPane.showMessageDialog(null,
              e.toString(), "Fehler", JOptionPane.ERROR_MESSAGE);
    }
  }//end exportToFile

  private String add_leading_zeros(int Number, int NumberCount)
  {
    String Result = "";

    for(int i = NumberCount - 1 ;i >= 0;i--)
    {
      int Divider = (int) Math.pow(10, i);
      if(Number % Divider == 0)
      {
        int Num = (int) Math.ceil(Number / Divider);
        Result += Num;
        Number = (int) (Number - Num * Divider);
      }
      else if(i > 0)
      {
        Result += "0";
      }
    }

    return Result;
  }



}//end class MotionNetLoader