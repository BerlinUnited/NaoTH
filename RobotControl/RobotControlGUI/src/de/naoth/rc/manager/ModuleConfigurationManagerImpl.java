/*
 * 
 */
package de.naoth.rc.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.dataformats.ModuleConfiguration;
import de.naoth.rc.dataformats.ModuleConfiguration.Node;
import de.naoth.rc.messages.Messages.Modules;
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
    private Command command = new Command("Cognition:representation:get").addArg("Modules");
    
    
  public ModuleConfigurationManagerImpl(){}

  @Override
  public ModuleConfiguration convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    ModuleConfiguration moduleConfiguration = new ModuleConfiguration();
    try
    {
      Modules list = Modules.parseFrom(result);

      for(Module m : list.getModulesList())
      {
        String simpleName = m.getName();
        String name = simpleName;
        
        // if the module has a path, then attach it to the name of the node
        if(m.hasPath()) {
            String path = m.getPath(); 
            String lower_name = name.toLowerCase();
            
            // replace all / and \ by :
            path = path.replaceAll("\\\\|/", ":");
            // hack: remove the double name
            //path = path.replaceFirst("(?i):" + lower_name + ".*", "");
            path = path.replaceFirst("(?i):" + lower_name + "\\.(.*)", "");
            name = path.toLowerCase() + ":" + name;
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
          command = new Command(name + ":representation:get").addArg("Modules");
      }
  }
  
  @Override
  public Command getCurrentCommand()
  {
    return command;
  }
}//end class ModuleConfigurationManager
