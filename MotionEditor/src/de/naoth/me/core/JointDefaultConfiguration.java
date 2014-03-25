/*
 * 
 */

package de.naoth.me.core;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

/**
 *
 * @author Heinrich Mellmann
 */
public class JointDefaultConfiguration extends JointPrototypeConfiguration
{
    public JointDefaultConfiguration()
    {
        File file = new File("joints.cfg");
        if(file.exists()) {
            loadFromFile(file);
            return;
        }
        
        addJointValuePrototype(new JointPrototype("HeadYaw", -120, 120, 0));
        addJointValuePrototype(new JointPrototype("HeadPitch", -40, 40, 0));
        
        addJointValuePrototype(new JointPrototype("RShoulderPitch", -120, 120, 119));
        addJointValuePrototype(new JointPrototype("LShoulderPitch", -120, 120, 119));
        addJointValuePrototype(new JointPrototype("RShoulderRoll", -95, 0, 0));
        addJointValuePrototype(new JointPrototype("LShoulderRoll", 0, 95, 0));
        
        addJointValuePrototype(new JointPrototype("RElbowYaw", -90, 120, 119));
        addJointValuePrototype(new JointPrototype("LElbowYaw", -120, 90, -119));
        addJointValuePrototype(new JointPrototype("RElbowRoll", 0, 90, 89));
        addJointValuePrototype(new JointPrototype("LElbowRoll", -90, 0, -89));
        
        addJointValuePrototype(new JointPrototype("RHipYawPitch", -65, 0, 0));
        addJointValuePrototype(new JointPrototype("RHipPitch", -90, 25, -31.5));
        addJointValuePrototype(new JointPrototype("RHipRoll", -45, 21, 0));
        addJointValuePrototype(new JointPrototype("RKneePitch", -5, 120, 63));
        addJointValuePrototype(new JointPrototype("RAnklePitch", -65, 45, -31.5));
        addJointValuePrototype(new JointPrototype("RAnkleRoll", -20, 25, 0));
        
        addJointValuePrototype(new JointPrototype("LHipYawPitch", -65, 0, 0));
        addJointValuePrototype(new JointPrototype("LHipPitch", -90, 25, -31.5));
        addJointValuePrototype(new JointPrototype("LHipRoll", -21, 45, 0));
        addJointValuePrototype(new JointPrototype("LKneePitch", -5, 120, 63));
        addJointValuePrototype(new JointPrototype("LAnklePitch", -65, 45, -31.5));
        addJointValuePrototype(new JointPrototype("LAnkleRoll", -25, 20, 0));
    }

    public void loadFromFile(File file) {
        
        try
        {
            BufferedReader reader = new BufferedReader(new FileReader(file));

            String line = reader.readLine();
            
            String name = null;
            double min = 0;
            double max = 0;
            
            while(line != null)
            {
                line = line.replace(";", "");
                line = line.trim();
                if(line.length() == 0 || line.trim().startsWith("#") || line.trim().startsWith("[")) {
                    line = reader.readLine();
                    continue;
                }
                
                String[] str = line.split("\\s*=\\s*");
                if(str.length != 2) {
                    throw new IOException("Unexpected line: " + line);
                }
                
                if(str[0].endsWith("Max")){
                    String name_max = str[0].replace("Max", "");
                    max = Double.parseDouble(str[1]);
                    
                    if(name != null) {
                        if(!name_max.equals(name)) {
                            throw new IOException("Expected: " + name + "Max" + " but fount " + name_max + "Max");
                        }
                        
                        addJointValuePrototype(new JointPrototype(name, min, max, min));
                        name = null;
                    } else {
                        name = name_max;
                    }
                }
                
                if(str[0].endsWith("Min")){
                    String name_min = str[0].replace("Min", "");
                    min = Double.parseDouble(str[1]);
                    
                    if(name != null) {
                        if(!name_min.equals(name)) {
                            throw new IOException("Expected: " + name + "Min" + " but fount " + name_min + "Min");
                        }
                        
                        addJointValuePrototype(new JointPrototype(name, min, max, min));
                        name = null;
                    } else {
                        name = name_min;
                    }
                }
                
                line = reader.readLine();
            }
        } catch(IOException ex) {
            ex.printStackTrace(System.err);
        }
    }
}//end ConfigurationLoader
