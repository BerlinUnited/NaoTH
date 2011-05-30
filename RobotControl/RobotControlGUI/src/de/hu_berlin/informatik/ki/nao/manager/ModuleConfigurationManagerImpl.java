/*
 * 
 */
package de.hu_berlin.informatik.ki.nao.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.hu_berlin.informatik.ki.nao.dataformats.ModuleConfiguration;
import de.hu_berlin.informatik.ki.nao.dataformats.ModuleConfiguration.Node;
import de.hu_berlin.informatik.ki.nao.messages.Messages.ModuleList;
import de.hu_berlin.informatik.ki.nao.messages.Messages.Module;
import de.hu_berlin.informatik.ki.nao.server.Command;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class ModuleConfigurationManagerImpl extends AbstractManager<ModuleConfiguration>
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
          //moduleConfiguration.addEdge(new ModuleConfiguration.Edge(),moduleNode,repNode);
        }
        
        for(String s : m.getUsedRepresentationsList())
        {
          Node repNode = new Node(s, ModuleConfiguration.NodeType.Represenation, true);
          moduleConfiguration.addVertex(repNode);
          //moduleConfiguration.addEdge(new ModuleConfiguration.Edge(),repNode,moduleNode);
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
