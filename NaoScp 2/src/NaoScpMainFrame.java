
import javax.swing.*;
import java.util.*;

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
  protected naoScpConfig config;  
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

  class remoteScriptRunner extends sshScriptRunner
  {
    public remoteScriptRunner(naoScpConfig config, String Ip, String sNaoNo, String sNaoByte, String scriptName, boolean reboot)
    {
      super(config, Ip, sNaoNo, sNaoByte, scriptName, reboot);
    }

    public remoteScriptRunner(naoScpConfig config, String Ip, String sNaoNo, String sNaoByte, String scriptName)
    {
      super(config, Ip, sNaoNo, sNaoByte, scriptName, false);
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

  class remoteSetupCopier extends sshSetupCopier
  {
    public remoteSetupCopier(naoScpConfig config, String sNaoNo, String sNaoByte)
    {
      super(config, sNaoNo, sNaoByte);
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
//        SwingUtilities.invokeLater
//        (
//          new Runnable()
//          {
//            public void run()
//            {
              String runFunction = "";
              if(mode.equals("full"))
              {
                runFunction = "initializeRobot";
              }
              else
              {
                runFunction = "setRobotNetworkConfig";
              }
              config.addresses.clear();
              remoteScriptRunner sLan = new remoteScriptRunner(config, config.actIp, "0", config.sNaoByte, runFunction, config.reboot);
              sLan.execute();
//              if(sLan.testConnection())
//              {
//                sLan.run();
//              }
//              else
//              {
//                actionDone("script");
//              }
//            }
//          }
//        );
      }
      actionDone(action);
    }

  }

  class remoteCopier extends sshCopier
  {
    public remoteCopier(naoScpConfig config, String sNaoNo, String sNaoByte)
    {
      super(config, sNaoNo, sNaoByte);
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
        if (config.restartNaoth)
        {
//          SwingUtilities.invokeLater
//          (
//            new Runnable()
//            {
//              public void run()
//              {
                remoteScriptRunner sLan = new remoteScriptRunner(config, config.actIp, config.sNaoNo, config.sNaoByte, "restartNaoTH", false);
                sLan.execute();
//                if(sLan.testConnection())
//                {
//                  sLan.execute();
//                }
//                else
//                {
//                  actionDone("script");
//                }
//              }
//            }
//          );
        }
        else
        {
          action = "all";
        }
      }
      actionDone(action);
    }
  }
}
