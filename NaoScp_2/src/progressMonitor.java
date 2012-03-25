/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import com.jcraft.jsch.*;
import javax.swing.*;

/**
 *
 * @author claas
 */
class progressMonitor implements SftpProgressMonitor
{

  long count = 0;
  long max = 0;
  private long percent = -1;
  private JProgressBar progressbar;

  public progressMonitor(JProgressBar progressbar)
  {
    this.progressbar = progressbar;
  }

  public void init(int op, String src, String dest, long max)
  {
    this.max = max;
    progressbar.setIndeterminate(false);
    progressbar.setValue(0);
    progressbar.setMinimum(0);
    progressbar.setMaximum(100);
    progressbar.setEnabled(true);
  }

  public boolean count(long count)
  {
    this.count += count;
    percent = this.count * 100 / max;
    progressbar.setValue((int) percent);
    return true;
  }

  public void end()
  {
    progressbar.setEnabled(false);
  }
}//end MyProgressMonitor
