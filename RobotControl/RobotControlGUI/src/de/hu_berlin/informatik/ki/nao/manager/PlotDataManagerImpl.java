/*
 * 
 */
package de.hu_berlin.informatik.ki.nao.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.hu_berlin.informatik.ki.nao.messages.Messages.Plots;
import de.hu_berlin.informatik.ki.nao.server.Command;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class PlotDataManagerImpl extends AbstractManagerPlugin<Plots>
        implements PlotDataManager
{

  public PlotDataManagerImpl()
  {
  }

  @Override
  public Plots convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    try
    {
      return Plots.parseFrom(result);
    }
    catch (InvalidProtocolBufferException ex)
    {
      Logger.getLogger(PlotDataManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
      return Plots.getDefaultInstance();
    }
  }

  public Command getCurrentCommand()
  {
    return new Command("plot_values");
  }
}//end class PlotDataManager

