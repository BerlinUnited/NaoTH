/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import java.io.UnsupportedEncodingException;
import java.security.NoSuchAlgorithmException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JPanel;

/**
 *
 * @author thomas
 */
public abstract class AbstractDialog extends JPanel implements Dialog
{

  @Override
  public abstract JPanel getPanel();

  @Override
  public String getDisplayName()
  {
    return this.getClass().getSimpleName();
  }

  @Override
  public void dispose()
  {
    // do nothing per default
  }

}
