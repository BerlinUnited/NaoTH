/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.opengl.representations.Primitive;

/**
 *
 * @author robert
 */
public abstract class GLDrawable2 {
    
    protected Primitive[] primitives = null;
    
    protected String pathToRES = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/res/";  
    
    public Primitive[] getPrimitives() {
        return primitives;
    }
    
    public void update() {
        
    }    
}
