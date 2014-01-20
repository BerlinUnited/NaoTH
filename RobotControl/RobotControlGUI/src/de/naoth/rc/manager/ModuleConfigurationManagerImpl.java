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

    private final String commandString = "modules:list";
    private Command command = new Command(commandString);
    
    
  public ModuleConfigurationManagerImpl(){}

  @Override
  public ModuleConfiguration convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    ModuleConfiguration moduleConfiguration = new ModuleConfiguration();
    try
    {
      ModuleList list = ModuleList.parseFrom(result);

      for(Module m : list.getModulesList())
      {
        String str[] = m.getName().split("\\|"); // hack
        String simpleName = str[0];
        String name = simpleName;
        
        if(str.length > 1) {
            String lower_name = name.toLowerCase();
            String hname = str[1].replaceAll("\\\\|/", ":").replaceFirst("(?i):" + lower_name + ".*", "");
            name = hname + ":" + name;
        }
        
        Node moduleNode = moduleConfiguration.addNode(
                simpleName, name, ModuleConfiguration.NodeType.Module, m.getActive());

        for(String s : m.getProvidedRepresentationsList())
        {
          Node repNode = moduleConfiguration.addNode(
                  s, null, ModuleConfiguration.NodeType.Represenation, true);
          moduleConfiguration.addEdge(moduleNode,repNode);
        }
        
        for(String s : m.getUsedRepresentationsList())
        {
          Node repNode = moduleConfiguration.addNode(
                  s, null, ModuleConfiguration.NodeType.Represenation, true);
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
  public void setModuleOwner(String name)
  {
      if(name != null && name.length() > 0){
          command = new Command(name + ":" + commandString);
      }
  }
  
  @Override
  public Command getCurrentCommand()
  {
    return command;
  }//end getCurrentCommand
}//end class ModuleConfigurationManager
