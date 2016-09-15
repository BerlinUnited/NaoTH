/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.file.GLFile;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.drawings.FieldDrawingSPL2013;
import de.naoth.rc.opengl.file.ObjFile;
import de.naoth.rc.opengl.model.GLDrawable;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.FloatBuffer;
import javax.imageio.ImageIO;

/**
 *
 * @author Robert
 */
public class FieldSPL2013 {
    protected GLTexturedModel field;
    
    public FieldSPL2013(GL3 gl) {        
        String objPath = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/res/";
        String shaderPath = "/src/de/naoth/rc/opengl/glsl/";
        
        
        FieldDrawingSPL2013 field2D = new FieldDrawingSPL2013();
        
        BufferedImage image = new BufferedImage(9000+2*700, 6000+2*700, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g2d = image.createGraphics();
        
        field2D.draw(g2d);
        
        Shader shader = new Shader(gl, shaderPath, "vertex_shader.glsl", "texture_FS.glsl");
        shader.setGlobalUniform("light.position", new float[]{100f, 50f, -40f});
        shader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});
        
        /*
        try {
	        image = ImageIO.read(new File(objPath + "ground.png"));
	    } catch (IOException io) {
	        System.out.println("Unable to find the texture file: ");
	    }
        */
        
        Texture texture = new Texture(image);
        
        this.field = new GLTexturedModel(gl, texture, new ObjFile(objPath, "plane.obj"), shader);
    }
    
    public GLObject getDrawable() {
        return (GLObject) field;
    }
    
}
