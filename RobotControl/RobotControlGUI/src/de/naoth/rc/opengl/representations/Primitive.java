/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.representations;

import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.Texture;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author robert
 */
public class Primitive {
    
    public String id = null;
    
    public GLData mesh = null;
    
    public Matrix4 offset = null;
    
    public Texture texture = null;
    
    public String shader = null;
    
    public Map<String, Object> properties;
    
    public Primitive(String id, GLData mesh, String shader) {
        this.properties = new HashMap();
        
        this.id = id;
        this.mesh = mesh;
        this.shader = shader;
        this.offset = new Matrix4();
    }
    
    public Primitive(String id, GLData mesh, String shader, Matrix4 offset) {
        this(id, mesh, shader);
        this.offset = offset;
    }
    
    public Primitive(String id, GLData mesh, String shader, Matrix4 offset, Texture texture) {
        this(id, mesh, shader, offset);
        this.texture = texture;
    }
}
