/*
 * 
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.core.messages.Messages.Plots;
import de.naoth.rc.core.server.Command;
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
      Logger.getLogger(PlotDataManagerImpl.class.getName()).log(Level.SEVERE, new String(result), ex);
      return Plots.getDefaultInstance();
    }
  }

  @Override
  public Command getCurrentCommand()
  {
    return new Command("Cognition:representation:get").addArg("DebugPlot");
  }
}//end class PlotDataManager

