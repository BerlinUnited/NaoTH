package de.naoth.naoscp;


import java.awt.Color;
import javax.swing.*;
import javax.swing.text.*;
//import java.util.*;
import javax.jmdns.ServiceEvent;
//import javax.jmdns.ServiceInfo;
import javax.jmdns.ServiceListener;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author claas
 */
abstract class NaoScpMainFrame  extends javax.swing.JFrame
{
  protected abstract void addJmdnsListenerService(ServiceEvent event, int idx);
  protected abstract void resolveJmdnsListenerService(ServiceEvent event, int idx);
  protected abstract void removeJmdnsListenerService(ServiceEvent event, int idx);
  
  protected abstract void actionInfo(String debugtext);
  
  protected abstract void haveError(String sNaoNo, String error);
  protected abstract void haveError(int naoNo, String error);
  protected abstract void haveCopyError(String sNaoNo, String error);
  protected abstract void haveCopyError(int naoNo, String error);
  protected abstract void haveScriptError(String sNaoNo, String error);
  protected abstract void haveScriptError(int naoNo, String error);
  protected abstract void allIsDone(String sNaoNo);
  protected abstract void allIsDone(int naoNo);
  protected abstract void scriptIsDone(String sNaoNo);
  protected abstract void scriptIsDone(int naoNo);
  protected abstract void copyIsDone(String sNaoNo);
  protected abstract void copyIsDone(int naoNo);

  class remoteScriptRunner extends SshScriptRunner
  {
    public remoteScriptRunner(NaoScpConfig config, JProgressBar progressBar, String Ip, String sNaoNo, String sNaoByte, String scriptName, boolean reboot)
    {
      super(config, progressBar, Ip, sNaoNo, sNaoByte, scriptName, reboot);
    }

    public remoteScriptRunner(NaoScpConfig config, JProgressBar progressBar, String Ip, String sNaoNo, String sNaoByte, String scriptName)
    {
      super(config, progressBar, Ip, sNaoNo, sNaoByte, scriptName, false);
    }

    public remoteScriptRunner(NaoScpConfig config, JProgressBar progressBar, String sNaoNo, String sNaoByte, String scriptName, boolean reboot)
    {
      super(config, progressBar, sNaoNo, sNaoByte, scriptName, reboot);
    }

    public remoteScriptRunner(NaoScpConfig config, JProgressBar progressBar, String sNaoNo, String sNaoByte, String scriptName)
    {
      super(config, progressBar, sNaoNo, sNaoByte, scriptName, false);
    }

    @Override
    protected void haveError(final String error)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            haveScriptError(config.sNaoNo, error);
          }
        }
      );
      hasError = true;
    }

    @Override
    protected void setInfo(final String info)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            actionInfo(info);
          }
        }
      );
    }

    @Override
    protected void actionDone(final String action)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            scriptIsDone(config.sNaoNo);
          }
        }
      );
    }
  }

  class remoteSetupCopier extends SshSetupCopier
  {
    public remoteSetupCopier(NaoScpConfig config, JProgressBar progressBar, String sNaoNo, String sNaoByte)
    {
      super(config, progressBar, sNaoNo, sNaoByte);
    }

    @Override
    protected void haveError(final String error)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            haveCopyError(config.sNaoNo, error);
            hasError = true;
          }
        }
      );
    }

    @Override
    protected void setInfo(final String info)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            actionInfo(info);
          }
        }
      );
    }

    @Override
    protected void actionDone(final String action)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            if(action.equalsIgnoreCase("copy"))
            {
              copyIsDone(config.sNaoNo);
            }
            else if(action.equalsIgnoreCase("script"))
            {
              scriptIsDone(config.sNaoNo);
            }
            else
            {
              allIsDone(config.sNaoNo);
            }
          }
        }
      );
    }

    @Override
    protected void done()
    {
      String action = "copy";
      if(hasError)
      {
        haveError(errors);
        action = "all";
      }
      else
      {
        String runFunction;
        if(mode.equals("full"))
        {
          runFunction = "initializeRobot";
        }
        else
        {
          runFunction = "setRobotNetworkConfig";
        }
        config.addresses.clear();
        remoteScriptRunner scriptRunner = new remoteScriptRunner(config, progressBar, config.actIp, "0", config.sNaoByte, runFunction, config.reboot);
        scriptRunner.execute();
      }
      actionDone(action);
    }

  }

  class remoteCopier extends SshCopier
  {
    public remoteCopier(NaoScpConfig config, JProgressBar progressBar, String sNaoNo, String sNaoByte)
    {
      super(config, progressBar, sNaoNo, sNaoByte);
    }

    @Override
    protected void haveError(final String error)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            if(!error.equals(""))
            {
              haveCopyError(config.sNaoNo, error);
            }
          }
        }
      );
      hasError = true;
    }

    @Override
    protected void setInfo(final String info)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            actionInfo("Nao " + config.sNaoNo + ": " + info);
          }
        }
      );
     }

    @Override
    protected void actionDone(final String action)
    {
      SwingUtilities.invokeLater
      (
        new Runnable()
        {
          public void run()
          {
            if(action.equalsIgnoreCase("copy"))
            {
              copyIsDone(config.sNaoNo);
            }
            else if(action.equalsIgnoreCase("script"))
            {
              scriptIsDone(config.sNaoNo);
            }
            else
            {
              allIsDone(config.sNaoNo);
            }
          }
        }
      );
    }

    @Override
    protected void done()
    {
      String action = "copy";
      if(hasError)
      {
        haveError(errors);
        action = "all";
      }
      else
      {
        if (config.restartNaoqi)
        {
          remoteScriptRunner scriptRunner = new remoteScriptRunner(config, progressBar, config.actIp, config.sNaoNo, config.sNaoByte, "restartNaoqi", false);
          scriptRunner.execute();
        }
        else if(config.restartNaoth)
        {
          remoteScriptRunner scriptRunner = new remoteScriptRunner(config, progressBar, config.actIp, config.sNaoNo, config.sNaoByte, "restartNaoTH", false);
          scriptRunner.execute();
        }
        else
        {
          action = "all";
        }
      }
      actionDone(action);
    }
  }

  class JmdnsServiceListener implements ServiceListener
  {
    private int idx;

    public JmdnsServiceListener(int idx) 
    {
      this.idx = idx;
    }
    
    @Override
    public void serviceAdded(final ServiceEvent event)
    {    
      SwingUtilities.invokeLater
      (
          new Runnable() 
          {
            public void run() 
            {
              addJmdnsListenerService(event, idx);
            }
          }
      );
    }

    @Override
    public void serviceRemoved(final ServiceEvent event)
    {
      
      SwingUtilities.invokeLater
      (
          new Runnable() 
          {
            public void run() 
            {
              removeJmdnsListenerService(event, idx);
            }
          }
      );
    }

    @Override
    public void serviceResolved(final ServiceEvent event)
    {
      SwingUtilities.invokeLater
      (
          new Runnable() 
          {
            public void run() 
            {
              resolveJmdnsListenerService(event, idx);
            }
          }
      );
    }
  }
  
  public class NoWrapParagraphView extends ParagraphView 
  {
    public NoWrapParagraphView(Element elem) 
    {
      super(elem);
    }

    @Override
    public void layout(int width, int height) 
    {
      super.layout(Short.MAX_VALUE, height);
    }

    @Override
    public float getMinimumSpan(int axis) 
    {
      return super.getPreferredSpan(axis);
    }
  }
  
  class WrapEditorKit extends StyledEditorKit 
  {
    ViewFactory defaultFactory=new WrapColumnFactory();
    @Override
    public ViewFactory getViewFactory() 
    {
      return defaultFactory;
    }
  }

  class WrapColumnFactory implements ViewFactory 
  {
    public View create(Element elem) 
    {
      String kind = elem.getName();
      if (kind != null) 
      {
        if (kind.equals(AbstractDocument.ContentElementName)) 
        {
            return new LabelView(elem);
        } 
        else if (kind.equals(AbstractDocument.ParagraphElementName)) 
        {
            return new NoWrapParagraphView(elem);
        } 
        else if (kind.equals(AbstractDocument.SectionElementName)) 
        {
            return new BoxView(elem, View.Y_AXIS);
        } 
        else if (kind.equals(StyleConstants.ComponentElementName)) 
        {
            return new ComponentView(elem);
        } 
        else if (kind.equals(StyleConstants.IconElementName)) 
        {
            return new IconView(elem);
        }
      }
      // default to text display
      return new LabelView(elem);
    }
  }

  protected Style addBashColorStyle(StyledDocument doc, String id)
  {
    Style defaultStyle = StyleContext.getDefaultStyleContext().getStyle(StyleContext.DEFAULT_STYLE);

    Style s = doc.addStyle("bash_" + id.replace(";", "_"), defaultStyle);
    StyleConstants.setFontFamily(defaultStyle, "SansSerif");
   
    //  # Background
    //  On_Black='\e[40m'       # Black
    //  On_Red='\e[41m'         # Red
    //  On_Green='\e[42m'       # Green
    //  On_Yellow='\e[43m'      # Yellow
    //  On_Blue='\e[44m'        # Blue
    //  On_Purple='\e[45m'      # Purple
    //  On_Cyan='\e[46m'        # Cyan
    //  On_White='\e[47m'       # White
    //
    //  # High Intensity backgrounds
    //  On_IBlack='\e[0;100m'   # Black
    //  On_IRed='\e[0;101m'     # Red
    //  On_IGreen='\e[0;102m'   # Green
    //  On_IYellow='\e[0;103m'  # Yellow
    //  On_IBlue='\e[0;104m'    # Blue
    //  On_IPurple='\e[10;95m'  # Purple
    //  On_ICyan='\e[0;106m'    # Cyan
    //  On_IWhite='\e[0;107m'   # White    
    if(id.equals("40") || id.equals("0;100") || id.equals("040") || id.equals("00;100"))
    {
      StyleConstants.setBackground(s, Color.BLACK);      
    }
    else if(id.equals("41") || id.equals("0;101") || id.equals("041") || id.equals("00;101"))
    {
      StyleConstants.setBackground(s, Color.RED);
    }
    else if(id.equals("42") || id.equals("0;102") || id.equals("042") || id.equals("00;102"))
    {
      StyleConstants.setBackground(s, Color.GREEN);
    }
    else if(id.equals("43") || id.equals("0;103") || id.equals("043") || id.equals("00;103"))
    {
      StyleConstants.setBackground(s, Color.YELLOW);
    }
    else if(id.equals("44") || id.equals("0;104") || id.equals("044") || id.equals("00;104"))
    {
      StyleConstants.setBackground(s, Color.BLUE);
    }
    else if(id.equals("45") || id.equals("0;105") || id.equals("045") || id.equals("00;105"))
    {
      StyleConstants.setBackground(s, Color.PINK);
    }
    else if(id.equals("46") || id.equals("0;106") || id.equals("046") || id.equals("00;106"))
    {
      StyleConstants.setBackground(s, Color.CYAN);
    }
    else if(id.equals("47") || id.equals("0;107") || id.equals("047") || id.equals("00;107"))
    {
      StyleConstants.setBackground(s, Color.WHITE);
    }       

    // # Regular Colors
    //  Black='\e[0;30m'        # Black
    //  Red='\e[0;31m'          # Red
    //  Green='\e[0;32m'        # Green
    //  Yellow='\e[0;33m'       # Yellow
    //  Blue='\e[0;34m'         # Blue
    //  Purple='\e[0;35m'       # Purple
    //  Cyan='\e[0;36m'         # Cyan
    //  White='\e[0;37m'        # White
    //
    //  # High Intensity
    //  IBlack='\e[0;90m'       # Black
    //  IRed='\e[0;91m'         # Red
    //  IGreen='\e[0;92m'       # Green
    //  IYellow='\e[0;93m'      # Yellow
    //  IBlue='\e[0;94m'        # Blue
    //  IPurple='\e[0;95m'      # Purple
    //  ICyan='\e[0;96m'        # Cyan
    //  IWhite='\e[0;97m'       # White
    if
    (
      id.equals("0;30") || id.equals("1;30") || id.equals("0;90") || id.equals("1;90") ||
      id.equals("00;30") || id.equals("01;30") || id.equals("00;90") || id.equals("01;90")
    )
    {
      StyleConstants.setForeground(s, Color.BLACK);      
    }
    else if
    (
      id.equals("0;31") || id.equals("1;31") || id.equals("0;91") || id.equals("1;91") ||
      id.equals("00;31") || id.equals("01;31") || id.equals("00;91") || id.equals("01;91")
    )
    {
      StyleConstants.setForeground(s, Color.RED);
    }
    else if
    (
      id.equals("0;32") || id.equals("1;32") || id.equals("0;92") || id.equals("1;92") ||
      id.equals("00;32") || id.equals("01;32") || id.equals("00;92") || id.equals("01;92")
    )
    {
      StyleConstants.setForeground(s, Color.GREEN);
    }
    else if
    (
      id.equals("0;33") || id.equals("1;33") || id.equals("0;93") || id.equals("1;93") ||
      id.equals("00;33") || id.equals("01;33") || id.equals("00;93") || id.equals("01;93")
    )
    {
      StyleConstants.setForeground(s, Color.YELLOW);
    }
    else if
    (
      id.equals("0;34") || id.equals("1;34") || id.equals("0;94") || id.equals("1;94") ||
      id.equals("00;34") || id.equals("01;34") || id.equals("00;94") || id.equals("01;94")
    )
    {
      StyleConstants.setForeground(s, Color.BLUE);
    }
    else if
    (
      id.equals("0;35") || id.equals("1;35") || id.equals("0;95") || id.equals("1;95") ||
      id.equals("00;35") || id.equals("01;35") || id.equals("00;95") || id.equals("01;95")
    )
    {
      StyleConstants.setForeground(s, Color.PINK);
    }
    else if
    (
      id.equals("0;36") || id.equals("1;36") || id.equals("0;96") || id.equals("1;96") ||
      id.equals("00;36") || id.equals("01;36") || id.equals("00;96") || id.equals("01;96")
    )
    {
      StyleConstants.setForeground(s, Color.CYAN);
    }
    else if
    (
      id.equals("0;37") || id.equals("1;37") || id.equals("0;97") || id.equals("1;97") ||
      id.equals("00;37") || id.equals("01;37") || id.equals("00;97") || id.equals("01;97")
    )
    {
      StyleConstants.setForeground(s, Color.WHITE);
    }       
    
    //  # Bold
    //  BBlack='\e[1;30m'       # Black
    //  BRed='\e[1;31m'         # Red
    //  BGreen='\e[1;32m'       # Green
    //  BYellow='\e[1;33m'      # Yellow
    //  BBlue='\e[1;34m'        # Blue
    //  BPurple='\e[1;35m'      # Purple
    //  BCyan='\e[1;36m'        # Cyan
    //  BWhite='\e[1;37m'       # White
    //
    //  # Bold High Intensity
    //  BIBlack='\e[1;90m'      # Black
    //  BIRed='\e[1;91m'        # Red
    //  BIGreen='\e[1;92m'      # Green
    //  BIYellow='\e[1;93m'     # Yellow
    //  BIBlue='\e[1;94m'       # Blue
    //  BIPurple='\e[1;95m'     # Purple
    //  BICyan='\e[1;96m'       # Cyan
    //  BIWhite='\e[1;97m'      # White
    if
    (
        id.equals("1;30") || id.equals("1;31") || id.equals("1;32") || 
        id.equals("1;33") || id.equals("1;34") || id.equals("1;35") || 
        id.equals("1;36") || id.equals("1;37") ||
        id.equals("01;30") || id.equals("01;31") || id.equals("01;32") || 
        id.equals("01;33") || id.equals("01;34") || id.equals("01;35") || 
        id.equals("01;36") || id.equals("01;37") ||
        id.equals("1;90") || id.equals("1;91") || id.equals("1;92") || 
        id.equals("1;93") || id.equals("1;94") || id.equals("1;95") || 
        id.equals("1;96") || id.equals("1;97") ||
        id.equals("01;90") || id.equals("01;91") || id.equals("01;92") || 
        id.equals("01;93") || id.equals("01;94") || id.equals("01;95") || 
        id.equals("01;96") || id.equals("01;97")
    )
    {
      StyleConstants.setBold(s, true);
    }
    
    //  # Underline
    //  UBlack='\e[4;30m'       # Black
    //  URed='\e[4;31m'         # Red
    //  UGreen='\e[4;32m'       # Green
    //  UYellow='\e[4;33m'      # Yellow
    //  UBlue='\e[4;34m'        # Blue
    //  UPurple='\e[4;35m'      # Purple
    //  UCyan='\e[4;36m'        # Cyan
    //  UWhite='\e[4;37m'       # White
    if
    (
        id.equals("4;30") || id.equals("4;31") || id.equals("4;32") || 
        id.equals("4;33") || id.equals("4;34") || id.equals("4;35") || 
        id.equals("4;36") || id.equals("4;37") ||
        id.equals("04;30") || id.equals("04;31") || id.equals("04;32") || 
        id.equals("04;33") || id.equals("04;34") || id.equals("04;35") || 
        id.equals("04;36") || id.equals("04;37") ||
        id.equals("4;90") || id.equals("4;91") || id.equals("4;92") || 
        id.equals("4;93") || id.equals("4;94") || id.equals("4;95") || 
        id.equals("4;96") || id.equals("4;97") ||
        id.equals("04;90") || id.equals("04;91") || id.equals("04;92") || 
        id.equals("04;93") || id.equals("04;94") || id.equals("04;95") || 
        id.equals("04;96") || id.equals("04;97")
    )
    {
      StyleConstants.setUnderline(s, true);
    }
    
    return s;
  }
   
  protected void addStylesToDocument(StyledDocument doc) 
  {
    String[] styleList = new String[]
      {
        "0", "00", 
        "40", "41", "42", "43", "44", "45", "46", "47",
        "040", "041", "042", "043", "044", "045", "046", "047",
        "0;30", "0;31", "0;32", "0;33", "0;34", "0;35", "0;36", "0;37",
        "00;30", "00;31", "00;32", "00;33", "00;34", "00;35", "00;36", "00;37",
        "1;30", "1;31", "1;32", "1;33", "1;34", "1;35", "1;36", "1;37",
        "01;30", "01;31", "01;32", "01;33", "01;34", "01;35", "01;36", "01;37",
        "4;30", "4;31", "4;32", "4;33", "4;34", "4;35", "4;36", "4;37",
        "04;30", "04;31", "04;32", "04;33", "04;34", "04;35", "04;36", "04;37",
        "0;90", "0;91", "0;92", "0;93", "0;94", "0;95", "0;96", "0;97",
        "00;90", "00;91", "00;92", "00;93", "00;94", "00;95", "00;96", "00;97",
        "1;90", "1;91", "1;92", "1;93", "1;94", "1;95", "1;96", "1;97",
        "01;90", "01;91", "01;92", "01;93", "01;94", "01;95", "01;96", "01;97",
        "4;90", "4;91", "4;92", "4;93", "4;94", "4;95", "4;96", "4;97",
        "04;90", "04;91", "04;92", "04;93", "04;94", "04;95", "04;96", "04;97",
        "0;100", "0;101", "0;102", "0;103", "0;104", "0;105", "0;106", "0;107",
        "00;100", "00;101", "00;102", "00;103", "00;104", "00;105", "00;106", "00;107"    
      };

    for(String id : styleList)
    {
      addBashColorStyle(doc, id);      
    }
  }

}
