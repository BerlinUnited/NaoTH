/*
 * 
 */
package de.naoth.rc.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.dataformats.ModuleConfiguration;
import de.naoth.rc.dataformats.ModuleConfiguration.Node;
import de.naoth.rc.messages.Messages.ModuleList;
import de.naoth.rc.messages.Messages.Module;
import de.naoth.rc.server.Command;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class ModuleConfigurationManagerImpl extends AbstractManagerPlugin<ModuleConfiguration>
        implements ModuleConfigurationManager
{

  public ModuleConfigurationManagerImpl()
  {
  }

  @Override
  public ModuleConfiguration convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    ModuleConfiguration moduleConfiguration = new ModuleConfiguration();
    try
    {
      ModuleList list = ModuleList.parseFrom(result);

      for(Module m : list.getModulesList())
      {
        Node moduleNode = new Node(m.getName(), ModuleConfiguration.NodeType.Module, m.getActive());
        moduleConfiguration.addVertex(moduleNode);

        for(String s : m.getProvidedRepresentationsList())
        {
          Node repNode = new Node(s, ModuleConfiguration.NodeType.Represenation, true);
          moduleConfiguration.addVertex(repNode);
          moduleConfiguration.addEdge(moduleNode,repNode);
        }
        
        for(String s : m.getUsedRepresentationsList())
        {
          Node repNode = new Node(s, ModuleConfiguration.NodeType.Represenation, true);
          moduleConfiguration.addVertex(repNode);
          moduleConfiguration.addEdge(repNode,moduleNode);
        }
      }

    }
    catch(InvalidProtocolBufferException ex)
    {
      Logger.getLogger(ModuleConfigurationManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
    }

    return moduleConfiguration;
  }//end convertByteArrayToType

  @Override
  public Command getCurrentCommand()
  {
    return new Command("modules:list");
  }//end getCurrentCommand
}//end class ModuleConfigurationManager
