/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.ObjFile;

/**
 *
 * @author robert
 */
public class RFoot extends GLDrawable {
        
    @Override
    public GLData getGLData() {
        return new ObjFile(pathToRES, "RFoot.obj").buildGLData();
    }
    
    @Override
    public String getName() {
        return "RFoot";
    }
}
